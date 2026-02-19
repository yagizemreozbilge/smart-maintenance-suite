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

#include "sensor_service.h"
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
  if (field == 0) return "101"; // ID

  if (field == 1) return "Temperature"; // Type

  if (field == 2) return "45.5"; // Value

  if (field == 3) return "C"; // Unit

  if (field == 4) return "2026-02-18 12:00:00"; // Timestamp

  return "";
}

char *PQerrorMessage(const void *conn) {
  return "Sensor Mock Error";
}

// Mock alert engine to avoid linking
void check_and_trigger_alerts(int sensor_id, const char *sensor_type, double value) {}

/* ====================================================================
 * TEST CASES
 * ==================================================================== */

static void test_get_sensor_stats_success(void **state) {
  mock_acquire_success = 1;
  mock_result_status = PGRES_TUPLES_OK;
  mock_ntuples = 1;
  SensorStatus stats[5];
  int count = get_machine_sensor_stats(1, stats, 5);
  assert_int_equal(count, 1);
  assert_int_equal(stats[0].sensor_id, 101);
  assert_string_equal(stats[0].sensor_type, "Temperature");
  assert_float_equal(stats[0].last_value, 45.5, 0.01);
}

static void test_get_sensor_stats_fail_paths(void **state) {
  SensorStatus stats[5];
  // Acquire fail
  mock_acquire_success = 0;
  assert_int_equal(get_machine_sensor_stats(1, stats, 5), 0);
  // Query fail
  mock_acquire_success = 1;
  mock_result_status = PGRES_FATAL_ERROR;
  assert_int_equal(get_machine_sensor_stats(1, stats, 5), 0);
}

static void test_add_sensor_reading_success(void **state) {
  mock_acquire_success = 1;
  mock_result_status = PGRES_COMMAND_OK;
  bool res = add_sensor_reading(1, "Temperature", 42.0);
  assert_true(res);
}

static void test_add_sensor_reading_fail(void **state) {
  // Acquire fail
  mock_acquire_success = 0;
  assert_false(add_sensor_reading(1, "Temperature", 42.0));
  // Command fail
  mock_acquire_success = 1;
  mock_result_status = PGRES_FATAL_ERROR;
  assert_false(add_sensor_reading(1, "Temperature", 42.0));
}

int main(void) {
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_get_sensor_stats_success),
    cmocka_unit_test(test_get_sensor_stats_fail_paths),
    cmocka_unit_test(test_add_sensor_reading_success),
    cmocka_unit_test(test_add_sensor_reading_fail),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
