/*
 * STANDALONE MAINTENANCE SERVICE TEST FILE
 *
 * Compile ALONE:
 *   gcc -o test_maintenance.exe test_maintenance_standalone.c -std=c99
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

// ============================================================================
// TYPE DEFINITIONS
// ============================================================================

typedef struct {
  void *pg_conn;
} DBConnection;

typedef struct {
  int id;
  int machine_id;
  char technician_name[100];
  char log_date[32];
  char description[512];
  double cost;
} MaintenanceLog;

typedef struct {
  int status;
  int ntuples;
  char data[100][10][512];
} PGresult;

// ============================================================================
// MOCK STATE
// ============================================================================

static DBConnection mock_connection = {.pg_conn = (void *)0x1234};
static bool mock_connection_available = true;
static bool mock_query_success = true;
static int mock_row_count = 0;
static PGresult mock_result;

#define PGRES_COMMAND_OK 1
#define PGRES_TUPLES_OK 2

// ============================================================================
// MOCK FUNCTIONS
// ============================================================================

DBConnection *db_pool_acquire(void) {
  return mock_connection_available ? &mock_connection : NULL;
}

void db_pool_release(DBConnection *conn) {
  (void)conn;
}

PGresult *PQexec(void *conn, const char *query) {
  (void)conn;

  if (!mock_query_success) {
    mock_result.status = -1;
    return &mock_result;
  }

  if (strstr(query, "INSERT")) {
    mock_result.status = PGRES_COMMAND_OK;
    mock_result.ntuples = 0;
  } else if (strstr(query, "SELECT")) {
    mock_result.status = PGRES_TUPLES_OK;
    mock_result.ntuples = mock_row_count;

    for (int i = 0; i < mock_row_count && i < 100; i++) {
      snprintf(mock_result.data[i][0], 512, "%d", i + 1);
      snprintf(mock_result.data[i][1], 512, "%d", 101 + i);
      snprintf(mock_result.data[i][2], 512, "John");
      snprintf(mock_result.data[i][3], 512, "2025-02-15");
      snprintf(mock_result.data[i][4], 512, "Routine maintenance");
      snprintf(mock_result.data[i][5], 512, "250.50");
    }
  } else if (strstr(query, "UPDATE") || strstr(query, "DELETE")) {
    mock_result.status = PGRES_COMMAND_OK;
    mock_result.ntuples = 0;
  }

  return &mock_result;
}

int PQresultStatus(PGresult *res) {
  return res->status;
}

int PQntuples(PGresult *res) {
  return res->ntuples;
}

char *PQgetvalue(PGresult *res, int row, int col) {
  return mock_result.data[row][col];
}

void PQclear(PGresult *res) {
  (void)res;
}

char *PQerrorMessage(void *conn) {
  (void)conn;
  return "Mock error";
}

#define LOG_ERROR(fmt, ...) printf("[ERROR] " fmt "\n", ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) printf("[INFO] " fmt "\n", ##__VA_ARGS__)

// ============================================================================
// MAINTENANCE SERVICE IMPLEMENTATION
// ============================================================================

bool add_maintenance_log(int machine_id, const char *technician, const char *description, double cost) {
  DBConnection *conn_wrapper = db_pool_acquire();

  if (!conn_wrapper) {
    LOG_ERROR("Could not acquire connection to add maintenance log.");
    return false;
  }

  char query[1024];
  snprintf(query, sizeof(query),
           "INSERT INTO maintenance_logs (machine_id, performer, description, cost) "
           "VALUES (%d, '%s', '%s', %.2f);",
           machine_id, technician, description, cost);
  PGresult *res = PQexec(conn_wrapper->pg_conn, query);

  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    LOG_ERROR("Failed to insert maintenance log: %s", PQerrorMessage(conn_wrapper->pg_conn));
    PQclear(res);
    db_pool_release(conn_wrapper);
    return false;
  }

  PQclear(res);
  db_pool_release(conn_wrapper);
  LOG_INFO("Maintenance log added for Machine %d.", machine_id);
  return true;
}

int get_maintenance_history(int machine_id, MaintenanceLog *out_logs, int max_logs) {
  DBConnection *conn_wrapper = db_pool_acquire();

  if (!conn_wrapper) {
    LOG_ERROR("Could not acquire connection to fetch maintenance history.");
    return 0;
  }

  char query[256];
  snprintf(query, sizeof(query),
           "SELECT id, machine_id, performer, to_char(ts, 'YYYY-MM-DD'), description, cost "
           "FROM maintenance_logs WHERE machine_id = %d ORDER BY ts DESC;",
           machine_id);
  PGresult *res = PQexec(conn_wrapper->pg_conn, query);

  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    LOG_ERROR("Failed to fetch maintenance history: %s", PQerrorMessage(conn_wrapper->pg_conn));
    PQclear(res);
    db_pool_release(conn_wrapper);
    return 0;
  }

  int rows = PQntuples(res);
  int count = (rows < max_logs) ? rows : max_logs;

  for (int i = 0; i < count; i++) {
    out_logs[i].id = atoi(PQgetvalue(res, i, 0));
    out_logs[i].machine_id = atoi(PQgetvalue(res, i, 1));
    strncpy(out_logs[i].technician_name, PQgetvalue(res, i, 2), 99);
    out_logs[i].technician_name[99] = '\0';
    strncpy(out_logs[i].log_date, PQgetvalue(res, i, 3), 31);
    out_logs[i].log_date[31] = '\0';
    strncpy(out_logs[i].description, PQgetvalue(res, i, 4), 511);
    out_logs[i].description[511] = '\0';
    out_logs[i].cost = atof(PQgetvalue(res, i, 5));
  }

  PQclear(res);
  db_pool_release(conn_wrapper);
  LOG_INFO("Fetched %d maintenance logs for Machine %d.", count, machine_id);
  return count;
}

int get_all_maintenance_logs(MaintenanceLog *out_logs, int max_logs) {
  DBConnection *conn_wrapper = db_pool_acquire();

  if (!conn_wrapper) return 0;

  const char *query = "SELECT id, machine_id, performer, to_char(ts, 'YYYY-MM-DD'), description, cost "
                      "FROM maintenance_logs ORDER BY ts DESC;";
  PGresult *res = PQexec(conn_wrapper->pg_conn, query);

  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    PQclear(res);
    db_pool_release(conn_wrapper);
    return 0;
  }

  int rows = PQntuples(res);
  int count = (rows < max_logs) ? rows : max_logs;

  for (int i = 0; i < count; i++) {
    out_logs[i].id = atoi(PQgetvalue(res, i, 0));
    out_logs[i].machine_id = atoi(PQgetvalue(res, i, 1));
    strncpy(out_logs[i].technician_name, PQgetvalue(res, i, 2), 99);
    out_logs[i].technician_name[99] = '\0';
    strncpy(out_logs[i].log_date, PQgetvalue(res, i, 3), 31);
    out_logs[i].log_date[31] = '\0';
    strncpy(out_logs[i].description, PQgetvalue(res, i, 4), 511);
    out_logs[i].description[511] = '\0';
    out_logs[i].cost = atof(PQgetvalue(res, i, 5));
  }

  PQclear(res);
  db_pool_release(conn_wrapper);
  return count;
}

bool update_maintenance_log(int log_id, const char *description, double cost) {
  DBConnection *conn_wrapper = db_pool_acquire();

  if (!conn_wrapper) return false;

  char query[1024];
  snprintf(query, sizeof(query),
           "UPDATE maintenance_logs SET description = '%s', cost = %.2f WHERE id = %d;",
           description, cost, log_id);
  PGresult *res = PQexec(conn_wrapper->pg_conn, query);
  bool success = (PQresultStatus(res) == PGRES_COMMAND_OK);

  if (!success) {
    LOG_ERROR("Failed to update log: %s", PQerrorMessage(conn_wrapper->pg_conn));
  } else {
    LOG_INFO("Maintenance log %d updated successfully.", log_id);
  }

  PQclear(res);
  db_pool_release(conn_wrapper);
  return success;
}

bool delete_maintenance_log(int log_id) {
  DBConnection *conn_wrapper = db_pool_acquire();

  if (!conn_wrapper) return false;

  char query[128];
  snprintf(query, sizeof(query), "DELETE FROM maintenance_logs WHERE id = %d;", log_id);
  PGresult *res = PQexec(conn_wrapper->pg_conn, query);
  bool success = (PQresultStatus(res) == PGRES_COMMAND_OK);

  if (!success) {
    LOG_ERROR("Failed to delete log: %s", PQerrorMessage(conn_wrapper->pg_conn));
  } else {
    LOG_INFO("Maintenance log %d deleted.", log_id);
  }

  PQclear(res);
  db_pool_release(conn_wrapper);
  return success;
}

// ============================================================================
// TEST HELPER
// ============================================================================

void reset_mocks(void) {
  mock_connection_available = true;
  mock_query_success = true;
  mock_row_count = 0;
  memset(&mock_result, 0, sizeof(mock_result));
}

// ============================================================================
// TESTS
// ============================================================================

void test_add_maintenance_log_success(void) {
  printf("\n[TEST] test_add_maintenance_log_success\n");
  reset_mocks();
  bool result = add_maintenance_log(101, "John Doe", "Oil change", 150.50);
  assert(result == true);
  printf("✓ PASSED\n");
}

void test_add_maintenance_log_no_connection(void) {
  printf("\n[TEST] test_add_maintenance_log_no_connection\n");
  reset_mocks();
  mock_connection_available = false;
  bool result = add_maintenance_log(101, "John", "Test", 100.0);
  assert(result == false);
  printf("✓ PASSED\n");
}

void test_add_maintenance_log_query_failure(void) {
  printf("\n[TEST] test_add_maintenance_log_query_failure\n");
  reset_mocks();
  mock_query_success = false;
  bool result = add_maintenance_log(101, "John", "Test", 100.0);
  assert(result == false);
  printf("✓ PASSED\n");
}

void test_get_maintenance_history_success(void) {
  printf("\n[TEST] test_get_maintenance_history_success\n");
  reset_mocks();
  mock_row_count = 1;
  MaintenanceLog logs[10];
  int count = get_maintenance_history(101, logs, 10);
  assert(count == 1);
  assert(logs[0].id == 1);
  assert(logs[0].machine_id == 101);
  printf("✓ PASSED\n");
}

void test_get_maintenance_history_no_connection(void) {
  printf("\n[TEST] test_get_maintenance_history_no_connection\n");
  reset_mocks();
  mock_connection_available = false;
  MaintenanceLog logs[10];
  int count = get_maintenance_history(101, logs, 10);
  assert(count == 0);
  printf("✓ PASSED\n");
}

void test_get_all_maintenance_logs_success(void) {
  printf("\n[TEST] test_get_all_maintenance_logs_success\n");
  reset_mocks();
  mock_row_count = 3;
  MaintenanceLog logs[10];
  int count = get_all_maintenance_logs(logs, 10);
  assert(count == 3);
  printf("✓ PASSED\n");
}

void test_update_maintenance_log_success(void) {
  printf("\n[TEST] test_update_maintenance_log_success\n");
  reset_mocks();
  bool result = update_maintenance_log(1, "Updated", 200.0);
  assert(result == true);
  printf("✓ PASSED\n");
}

void test_delete_maintenance_log_success(void) {
  printf("\n[TEST] test_delete_maintenance_log_success\n");
  reset_mocks();
  bool result = delete_maintenance_log(1);
  assert(result == true);
  printf("✓ PASSED\n");
}

// ============================================================================
// MAIN
// ============================================================================

int main(void) {
  printf("========================================\n");
  printf("MAINTENANCE SERVICE TEST (STANDALONE)\n");
  printf("========================================\n");
  int total = 0, passed = 0;
  test_add_maintenance_log_success();
  total++;
  passed++;
  test_add_maintenance_log_no_connection();
  total++;
  passed++;
  test_add_maintenance_log_query_failure();
  total++;
  passed++;
  test_get_maintenance_history_success();
  total++;
  passed++;
  test_get_maintenance_history_no_connection();
  total++;
  passed++;
  test_get_all_maintenance_logs_success();
  total++;
  passed++;
  test_update_maintenance_log_success();
  total++;
  passed++;
  test_delete_maintenance_log_success();
  total++;
  passed++;
  printf("\n========================================\n");
  printf("TEST RESULTS: %d/%d PASSED\n", passed, total);
  printf("========================================\n");
  return (passed == total) ? 0 : 1;
}
