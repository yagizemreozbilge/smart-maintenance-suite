#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// PostgreSQL Definitions (to avoid libpq-fe.h)
#define LIBPQ_FE_H
#define PGconn void
#define PGresult void
#define PGRES_COMMAND_OK 1
#define PGRES_TUPLES_OK 2
#define PGRES_FATAL_ERROR 3

#include "maintenance_service.h"
#include "db_connection.h"

// Logger mock
void LOG_INFO(const char *format, ...) {}

void LOG_ERROR(const char *format, ...) {}

// Mock State
static DBConnection mock_conn;
static int mock_acquire_success = 1;
static int mock_result_status = PGRES_COMMAND_OK;
static int mock_ntuples = 0;

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
  if (field == 0) return "1"; // ID

  if (field == 1) return "10"; // MachineID

  if (field == 2) return "TestTech";

  if (field == 3) return "2026-02-18";

  if (field == 4) return "Test Description";

  if (field == 5) return "150.50"; // Cost

  return "";
}

char *PQerrorMessage(const void *conn) {
  return "Mock Error Material";
}

/* ====================================================================
 * TEST CASES
 * ==================================================================== */

static void test_add_maintenance_log_success(void **state) {
  mock_acquire_success = 1;
  mock_result_status = PGRES_COMMAND_OK;
  bool result = add_maintenance_log(1, "Tech1", "Maintenance", 100.0);
  assert_true(result);
}

static void test_add_maintenance_log_fail(void **state) {
  mock_acquire_success = 0;
  bool result = add_maintenance_log(1, "Tech1", "Maintenance", 100.0);
  assert_false(result);
  // DB error
  mock_acquire_success = 1;
  mock_result_status = PGRES_FATAL_ERROR;
  result = add_maintenance_log(1, "Tech1", "Maintenance", 100.0);
  assert_false(result);
}

static void test_get_maintenance_history_paths(void **state) {
  // Success
  mock_acquire_success = 1;
  mock_result_status = PGRES_TUPLES_OK;
  mock_ntuples = 2;
  MaintenanceLog logs[5];
  int count = get_maintenance_history(1, logs, 5);
  assert_int_equal(count, 2);
  assert_string_equal(logs[0].technician_name, "TestTech");
  // Acquire Fail
  mock_acquire_success = 0;
  count = get_maintenance_history(1, logs, 5);
  assert_int_equal(count, 0);
  // Query Fail
  mock_acquire_success = 1;
  mock_result_status = PGRES_FATAL_ERROR;
  count = get_maintenance_history(1, logs, 5);
  assert_int_equal(count, 0);
}

static void test_get_all_maintenance_logs_paths(void **state) {
  mock_acquire_success = 1;
  mock_result_status = PGRES_TUPLES_OK;
  mock_ntuples = 1;
  MaintenanceLog items[10];
  int count = get_all_maintenance_logs(items, 10);
  assert_int_equal(count, 1);
  mock_acquire_success = 0;
  count = get_all_maintenance_logs(items, 10);
  assert_int_equal(count, 0);
}

static void test_update_maintenance_log_paths(void **state) {
  mock_acquire_success = 1;
  mock_result_status = PGRES_COMMAND_OK;
  bool res = update_maintenance_log(1, "Updated", 200.0);
  assert_true(res);
  mock_result_status = PGRES_FATAL_ERROR;
  res = update_maintenance_log(1, "Updated", 200.0);
  assert_false(res);
  mock_acquire_success = 0;
  res = update_maintenance_log(1, "Updated", 200.0);
  assert_false(res);
}

static void test_delete_maintenance_log_paths(void **state) {
  mock_acquire_success = 1;
  mock_result_status = PGRES_COMMAND_OK;
  bool res = delete_maintenance_log(1);
  assert_true(res);
  mock_acquire_success = 0;
  res = delete_maintenance_log(1);
  assert_false(res);
}

int main(void) {
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_add_maintenance_log_success),
    cmocka_unit_test(test_add_maintenance_log_fail),
    cmocka_unit_test(test_get_maintenance_history_paths),
    cmocka_unit_test(test_get_all_maintenance_logs_paths),
    cmocka_unit_test(test_update_maintenance_log_paths),
    cmocka_unit_test(test_delete_maintenance_log_paths),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
