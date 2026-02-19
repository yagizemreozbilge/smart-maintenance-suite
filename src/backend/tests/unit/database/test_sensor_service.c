#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include <libpq-fe.h>          // GERÇEK libpq header
#include "sensor_service.h"    // Senin servis

/* ==========================================================
   GLOBAL MOCK STATE
   ========================================================== */

static int mock_db_available;
static int mock_query_success;
static int mock_rows;
static int mock_alert_called;
static int last_query_type;   /* 1=SELECT, 2=INSERT */

/* ==========================================================
   RESET
   ========================================================== */

void reset_mocks() {
  mock_db_available = 1;
  mock_query_success = 1;
  mock_rows = 1;
  mock_alert_called = 0;
  last_query_type = 0;
}

/* ==========================================================
   FAKE DB CONNECTION
   ========================================================== */

typedef struct {
  PGconn *pg_conn;
} DBConnection;

static DBConnection fake_conn;
static PGresult *fake_res = (PGresult *)0x1;
/* ==========================================================
   MOCK DB POOL
   ========================================================== */

DBConnection *db_pool_acquire() {
  if (!mock_db_available)
    return NULL;

  return &fake_conn;
}

void db_pool_release(DBConnection *conn) {
  (void)conn;
}

/* ==========================================================
   MOCK LIBPQ FUNCTIONS
   ========================================================== */

PGresult *PQexec(PGconn *conn, const char *query) {
  (void)conn;

  if (strstr(query, "SELECT"))
    last_query_type = 1;
  else if (strstr(query, "INSERT"))
    last_query_type = 2;
  else
    last_query_type = 0;

  return fake_res;
}

ExecStatusType PQresultStatus(const PGresult *res) {
  (void)res;

  if (!mock_query_success)
    return PGRES_FATAL_ERROR;

  if (last_query_type == 1)
    return PGRES_TUPLES_OK;

  if (last_query_type == 2)
    return PGRES_COMMAND_OK;

  return PGRES_FATAL_ERROR;
}

int PQntuples(const PGresult *res) {
  (void)res;
  return mock_rows;
}

char *PQgetvalue(const PGresult *res, int row, int col) {
  (void)res;
  (void)row;
  static char buffer[64];

  switch (col) {
    case 0:
      strcpy(buffer, "101");
      break;

    case 1:
      strcpy(buffer, "Temperature");
      break;

    case 2:
      strcpy(buffer, "42.5");
      break;

    case 3:
      strcpy(buffer, "C");
      break;

    case 4:
      strcpy(buffer, "2025-01-01 12:00:00");
      break;

    default:
      strcpy(buffer, "");
      break;
  }

  return buffer;
}

void PQclear(PGresult *res) {
  (void)res;
}

char *PQerrorMessage(const PGconn *conn) {
  (void)conn;
  return "Mock error";
}

/* ==========================================================
   MOCK ALERT
   ========================================================== */

void check_and_trigger_alerts(int sensor_id,
                              const char *sensor_type,
                              double value) {
  (void)sensor_id;
  (void)sensor_type;
  (void)value;
  mock_alert_called = 1;
}

/* ==========================================================
   TESTS
   ========================================================== */

void test_connection_failure() {
  reset_mocks();
  mock_db_available = 0;
  SensorStatus stats[5];
  int result = get_machine_sensor_stats(1, stats, 5);
  assert(result == 0);
  printf("✔ test_connection_failure\n");
}

void test_query_failure() {
  reset_mocks();
  mock_query_success = 0;
  SensorStatus stats[5];
  int result = get_machine_sensor_stats(1, stats, 5);
  assert(result == 0);
  printf("✔ test_query_failure\n");
}

void test_zero_rows() {
  reset_mocks();
  mock_rows = 0;
  SensorStatus stats[5];
  int result = get_machine_sensor_stats(1, stats, 5);
  assert(result == 0);
  printf("✔ test_zero_rows\n");
}

void test_successful_fetch() {
  reset_mocks();
  mock_rows = 1;
  SensorStatus stats[5];
  int result = get_machine_sensor_stats(1, stats, 5);
  assert(result == 1);
  assert(stats[0].sensor_id == 101);
  assert(strcmp(stats[0].sensor_type, "Temperature") == 0);
  printf("✔ test_successful_fetch\n");
}

void test_truncate_rows() {
  reset_mocks();
  mock_rows = 10;
  SensorStatus stats[2];
  int result = get_machine_sensor_stats(1, stats, 2);
  assert(result == 2);
  printf("✔ test_truncate_rows\n");
}

void test_insert_connection_fail() {
  reset_mocks();
  mock_db_available = 0;
  int result = add_sensor_reading(1, "Temperature", 50.0);
  assert(result == 0);
  printf("✔ test_insert_connection_fail\n");
}

void test_insert_query_fail() {
  reset_mocks();
  mock_query_success = 0;
  int result = add_sensor_reading(1, "Temperature", 50.0);
  assert(result == 0);
  printf("✔ test_insert_query_fail\n");
}

void test_insert_success_alert_called() {
  reset_mocks();
  int result = add_sensor_reading(1, "Temperature", 50.0);
  assert(result == 1);
  assert(mock_alert_called == 1);
  printf("✔ test_insert_success_alert_called\n");
}

/* ==========================================================
   MAIN
   ========================================================== */

int main() {
  test_connection_failure();
  test_query_failure();
  test_zero_rows();
  test_successful_fetch();
  test_truncate_rows();
  test_insert_connection_fail();
  test_insert_query_fail();
  test_insert_success_alert_called();
  printf("\nAll sensor_service tests passed.\n");
  return 0;
}
