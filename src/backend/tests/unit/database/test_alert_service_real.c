#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// PostgreSQL Definitions
#define LIBPQ_FE_H
#define PGconn void
#define PGresult void
#define PGRES_COMMAND_OK 1
#define PGRES_TUPLES_OK 2
#define PGRES_FATAL_ERROR 3

#include "alert_service.h"
#include "db_connection.h"

// Logger mock
void LOG_INFO(const char *format, ...) {}

void LOG_ERROR(const char *format, ...) {}

// Mock State
static DBConnection mock_conn;
static int mock_acquire_success = 1;
static int mock_result_status = PGRES_TUPLES_OK;
static int mock_ntuples = 1;

// Mock DB functions
DBConnection *db_pool_acquire(void) {
  if (mock_acquire_success) {
    mock_conn.pg_conn = (void *)0x1234;
    return &mock_conn;
  }

  return NULL;
}

void db_pool_release(DBConnection *conn) {}

void *PQexec(void *conn, const char *query) {
  return (void *)0x5678;
}

int PQresultStatus(const void *res) {
  return mock_result_status;
}

void PQclear(void *res) {}

int PQntuples(const void *res) {
  return mock_ntuples;
}

char *PQgetvalue(const void *res, int row, int field) {
  if (field == 0) return "100"; // ID

  if (field == 1) return "1"; // SensorID

  if (field == 2) return "CRITICAL";

  if (field == 3) return "Mock Message";

  if (field == 4) return "2026-02-18 22:40:00";

  return "";
}

char *PQerrorMessage(const void *conn) {
  return "Alert Mock Error";
}

/* ====================================================================
 * TEST CASES
 * ==================================================================== */

static void test_create_alert_paths(void **state) {
  mock_acquire_success = 1;
  mock_result_status = PGRES_COMMAND_OK;
  assert_true(create_alert(1, SEVERITY_WARNING, "Test Message"));
  mock_result_status = PGRES_FATAL_ERROR;
  assert_false(create_alert(1, SEVERITY_WARNING, "Test Message"));
  mock_acquire_success = 0;
  assert_false(create_alert(1, SEVERITY_WARNING, "Test Message"));
}

static void test_check_and_trigger_logic(void **state) {
  mock_acquire_success = 1;
  mock_result_status = PGRES_COMMAND_OK;
  // Temperature paths
  check_and_trigger_alerts(1, "Temperature", 95.0); // Critical
  check_and_trigger_alerts(1, "Temperature", 80.0); // Warning
  check_and_trigger_alerts(1, "Temperature", 50.0); // None
  // Vibration
  check_and_trigger_alerts(2, "Vibration", 6.0); // Critical
  check_and_trigger_alerts(2, "Vibration", 1.0); // None
  // Pressure
  check_and_trigger_alerts(3, "Pressure", 15.0); // Warning
  check_and_trigger_alerts(3, "Pressure", 5.0); // None
  // Unknown
  check_and_trigger_alerts(4, "Unknown", 100.0);
}

static void test_get_recent_alerts_paths(void **state) {
  mock_acquire_success = 1;
  mock_result_status = PGRES_TUPLES_OK;
  mock_ntuples = 2;
  AlertInfo arr[10];
  assert_int_equal(get_recent_alerts(arr, 10), 2);
  mock_acquire_success = 0;
  assert_int_equal(get_recent_alerts(arr, 10), 0);
}

static void test_serialize_alerts(void **state) {
  mock_acquire_success = 1;
  mock_result_status = PGRES_TUPLES_OK;
  mock_ntuples = 1;
  char *json = alert_service_serialize_alerts();
  assert_non_null(json);
  assert_true(strstr(json, "CRITICAL") != NULL);
  free(json);
}

int main(void) {
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_create_alert_paths),
    cmocka_unit_test(test_check_and_trigger_logic),
    cmocka_unit_test(test_get_recent_alerts_paths),
    cmocka_unit_test(test_serialize_alerts),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
