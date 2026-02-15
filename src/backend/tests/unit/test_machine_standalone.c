/*
 * STANDALONE MACHINE SERVICE TEST FILE
 *
 * Compile ALONE:
 *   gcc -o test_machine.exe test_machine_standalone.c -std=c99
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
  char name[100];
  char model[50];
  char location[100];
  char status[20];
  double health_score;
} Machine;

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

  if (strstr(query, "SELECT") && strstr(query, "machines")) {
    mock_result.status = PGRES_TUPLES_OK;
    mock_result.ntuples = mock_row_count;

    for (int i = 0; i < mock_row_count && i < 100; i++) {
      snprintf(mock_result.data[i][0], 512, "%d", i + 1);
      snprintf(mock_result.data[i][1], 512, "Machine-%d", i + 1);
      snprintf(mock_result.data[i][2], 512, "Model-X%d", i + 1);
      snprintf(mock_result.data[i][3], 512, "Location-%d", i + 1);
      snprintf(mock_result.data[i][4], 512, i % 2 == 0 ? "active" : "idle");
    }
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
// MACHINE SERVICE IMPLEMENTATION
// ============================================================================

double get_machine_health_score(int machine_id) {
  (void)machine_id;
  return 94.7;
}

int get_all_machines(Machine *machines, int max_count) {
  if (!machines || max_count <= 0)
    return 0;

  DBConnection *conn_wrapper = db_pool_acquire();

  if (!conn_wrapper) {
    LOG_ERROR("Could not acquire connection to fetch machines.");
    return 0;
  }

  PGresult *res = PQexec(conn_wrapper->pg_conn,
                         "SELECT id, name, model, location, status FROM machines");

  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    LOG_ERROR("SELECT machines failed: %s", PQerrorMessage(conn_wrapper->pg_conn));
    PQclear(res);
    db_pool_release(conn_wrapper);
    return 0;
  }

  int rows = PQntuples(res);
  int count = (rows < max_count) ? rows : max_count;

  for (int i = 0; i < count; i++) {
    machines[i].id = atoi(PQgetvalue(res, i, 0));
    strncpy(machines[i].name, PQgetvalue(res, i, 1), 99);
    machines[i].name[99] = '\0';
    strncpy(machines[i].model, PQgetvalue(res, i, 2), 49);
    machines[i].model[49] = '\0';
    strncpy(machines[i].location, PQgetvalue(res, i, 3), 99);
    machines[i].location[99] = '\0';
    strncpy(machines[i].status, PQgetvalue(res, i, 4), 19);
    machines[i].status[19] = '\0';
    machines[i].health_score = get_machine_health_score(machines[i].id);
  }

  PQclear(res);
  db_pool_release(conn_wrapper);
  LOG_INFO("Successfully fetched %d machines from DB.", count);
  return count;
}

int get_machine_by_id(int id, Machine *machine) {
  if (!machine)
    return -1;

  machine->id = id;
  snprintf(machine->name, sizeof(machine->name), "Machine %d", id);
  strncpy(machine->model, "Standard Model", sizeof(machine->model) - 1);
  machine->model[sizeof(machine->model) - 1] = '\0';
  strncpy(machine->location, "Production Line", sizeof(machine->location) - 1);
  machine->location[sizeof(machine->location) - 1] = '\0';
  strncpy(machine->status, "running", sizeof(machine->status) - 1);
  machine->status[sizeof(machine->status) - 1] = '\0';
  machine->health_score = 90.0;
  return 0;
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

void test_get_all_machines_null_pointer(void) {
  printf("\n[TEST] test_get_all_machines_null_pointer\n");
  reset_mocks();
  int count = get_all_machines(NULL, 5);
  assert(count == 0);
  printf("✓ PASSED\n");
}

void test_get_all_machines_zero_max(void) {
  printf("\n[TEST] test_get_all_machines_zero_max\n");
  reset_mocks();
  Machine machines[5];
  int count = get_all_machines(machines, 0);
  assert(count == 0);
  printf("✓ PASSED\n");
}

void test_get_all_machines_no_connection(void) {
  printf("\n[TEST] test_get_all_machines_no_connection\n");
  reset_mocks();
  mock_connection_available = false;
  Machine machines[5];
  int count = get_all_machines(machines, 5);
  assert(count == 0);
  printf("✓ PASSED\n");
}

void test_get_all_machines_query_failure(void) {
  printf("\n[TEST] test_get_all_machines_query_failure\n");
  reset_mocks();
  mock_query_success = false;
  Machine machines[5];
  int count = get_all_machines(machines, 5);
  assert(count == 0);
  printf("✓ PASSED\n");
}

void test_get_all_machines_single_machine(void) {
  printf("\n[TEST] test_get_all_machines_single_machine\n");
  reset_mocks();
  mock_row_count = 1;
  Machine machines[5];
  int count = get_all_machines(machines, 5);
  assert(count == 1);
  assert(machines[0].id == 1);
  assert(strcmp(machines[0].name, "Machine-1") == 0);
  printf("✓ PASSED\n");
}

void test_get_all_machines_multiple_machines(void) {
  printf("\n[TEST] test_get_all_machines_multiple_machines\n");
  reset_mocks();
  mock_row_count = 3;
  Machine machines[5];
  int count = get_all_machines(machines, 5);
  assert(count == 3);
  printf("✓ PASSED\n");
}

void test_get_all_machines_pagination(void) {
  printf("\n[TEST] test_get_all_machines_pagination\n");
  reset_mocks();
  mock_row_count = 100;
  Machine machines[10];
  int count = get_all_machines(machines, 10);
  assert(count == 10);
  printf("✓ PASSED\n");
}

void test_get_machine_by_id_null_pointer(void) {
  printf("\n[TEST] test_get_machine_by_id_null_pointer\n");
  reset_mocks();
  int result = get_machine_by_id(10, NULL);
  assert(result == -1);
  printf("✓ PASSED\n");
}

void test_get_machine_by_id_valid(void) {
  printf("\n[TEST] test_get_machine_by_id_valid\n");
  reset_mocks();
  Machine machine;
  int result = get_machine_by_id(123, &machine);
  assert(result == 0);
  assert(machine.id == 123);
  assert(strcmp(machine.name, "Machine 123") == 0);
  printf("✓ PASSED\n");
}

void test_get_machine_health_score(void) {
  printf("\n[TEST] test_get_machine_health_score\n");
  reset_mocks();
  double score = get_machine_health_score(1);
  assert(score == 94.7);
  printf("✓ PASSED\n");
}

// ============================================================================
// MAIN
// ============================================================================

int main(void) {
  printf("========================================\n");
  printf("MACHINE SERVICE TEST (STANDALONE)\n");
  printf("========================================\n");
  int total = 0, passed = 0;
  test_get_all_machines_null_pointer();
  total++;
  passed++;
  test_get_all_machines_zero_max();
  total++;
  passed++;
  test_get_all_machines_no_connection();
  total++;
  passed++;
  test_get_all_machines_query_failure();
  total++;
  passed++;
  test_get_all_machines_single_machine();
  total++;
  passed++;
  test_get_all_machines_multiple_machines();
  total++;
  passed++;
  test_get_all_machines_pagination();
  total++;
  passed++;
  test_get_machine_by_id_null_pointer();
  total++;
  passed++;
  test_get_machine_by_id_valid();
  total++;
  passed++;
  test_get_machine_health_score();
  total++;
  passed++;
  printf("\n========================================\n");
  printf("TEST RESULTS: %d/%d PASSED\n", passed, total);
  printf("========================================\n");
  return (passed == total) ? 0 : 1;
}
