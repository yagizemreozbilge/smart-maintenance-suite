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
#define PGRES_TUPLES_OK 2
#define PGRES_FATAL_ERROR 3

#include "machine_service.h"
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
  if (field == 0) return "1"; // ID

  if (field == 1) return "CNC-01"; // Name

  if (field == 2) return "X100"; // Model

  if (field == 3) return "FloorB"; // Location

  if (field == 4) return "running"; // Status

  return "";
}

char *PQerrorMessage(const void *conn) {
  return "Machine Mock Error";
}

/* ====================================================================
 * TEST CASES
 * ==================================================================== */

static void test_get_all_machines_paths(void **state) {
  mock_acquire_success = 1;
  mock_result_status = PGRES_TUPLES_OK;
  mock_ntuples = 1;
  Machine items[5];
  int count = get_all_machines(items, 5);
  assert_int_equal(count, 1);
  assert_string_equal(items[0].name, "CNC-01");
  // Test truncation
  mock_ntuples = 10;
  count = get_all_machines(items, 5);
  assert_int_equal(count, 5);
  // Acquire Fail
  mock_acquire_success = 0;
  count = get_all_machines(items, 5);
  assert_int_equal(count, 0);
  // Query Fail
  mock_acquire_success = 1;
  mock_result_status = PGRES_FATAL_ERROR;
  count = get_all_machines(items, 5);
  assert_int_equal(count, 0);
  // Param fail
  assert_int_equal(get_all_machines(NULL, 5), 0);
  assert_int_equal(get_all_machines(items, 0), 0);
}

static void test_get_machine_by_id_simple(void **state) {
  Machine m;
  int res = get_machine_by_id(10, &m);
  assert_int_equal(res, 0);
  assert_int_equal(m.id, 10);
  assert_int_equal(get_machine_by_id(1, NULL), -1);
}

static void test_get_health_score(void **state) {
  double score = get_machine_health_score(1);
  assert_true(score > 0.0);
}

int main(void) {
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_get_all_machines_paths),
    cmocka_unit_test(test_get_machine_by_id_simple),
    cmocka_unit_test(test_get_health_score),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
