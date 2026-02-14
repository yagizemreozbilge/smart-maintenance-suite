#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// PostgreSQL'i tamamen devre dışı bırak
#define LIBPQ_FE_H
#define PGconn void
#define PGresult void

// Header dosyaları
#include "machine_service.h"
#include "db_connection.h"

// Logger mock
void LOG_INFO(const char *format, ...) {}

void LOG_ERROR(const char *format, ...) {}

// Mock DB functions
static DBConnection mock_conn;
static int mock_acquire_success = 1;

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
  return 1;  // PGRES_COMMAND_OK
}

void PQclear(void *res) {}

int PQntuples(const void *res) {
  return 0;
}

char *PQgetvalue(const void *res, int row, int field) {
  return "";
}

char *PQerrorMessage(const void *conn) {
  return "";
}

/* ====================================================================
 * TEST CASES
 * ==================================================================== */

static void test_get_all_machines(void **state) {
  mock_acquire_success = 1;
  Machine items[10];
  int count = get_all_machines(items, 10);
  assert_int_equal(count, 0);
}

static void test_get_machine_by_id_not_found(void **state) {
  Machine item;
  int found = get_machine_by_id(999, &item);
  assert_int_equal(found, 0);
}

int main(void) {
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_get_all_machines),
    cmocka_unit_test(test_get_machine_by_id_not_found),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
