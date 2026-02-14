#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// ÖNEMLİ: libpq-fe.h'i include ETME!
#define PGconn void
#define PGresult void

#define LIBPQ_FE_H
#define PQconninfoOption void
#define ConnStatusType int
#define ExecStatusType int
#define CONNECTION_OK 0
#define CONNECTION_BAD 1
#define PGRES_COMMAND_OK 1
#define PGRES_TUPLES_OK 2
#define PGRES_FATAL_ERROR 3

#include "inventory_service.h"
#include "db_connection.h"

// Logger mock'larını manuel tanımla
#undef LOG_ERROR
#undef LOG_INFO
#define LOG_ERROR(msg, ...)
#define LOG_INFO(msg, ...)

/* ====================================================================
 * MOCK FONKSİYONLAR
 * ==================================================================== */

static DBConnection mock_conn;
static void *mock_result;
static int mock_acquire_success = 1;
static int mock_result_status = 2; // PGRES_TUPLES_OK
static char mock_error_message[256] = "";
static char mock_query[1024] = "";

static int mock_acquire_called = 0;
static int mock_release_called = 0;
static int mock_pqexec_called = 0;
static int mock_pqclear_called = 0;
static int mock_pqntuples_called = 0;

typedef struct {
  char id[10];
  char part_name[100];
  char sku[50];
  char quantity[10];
  char min_stock[10];
  char unit_cost[20];
  char last_restocked[32];
} MockRow;

static MockRow mock_rows[10];
static int mock_row_count = 0;

DBConnection *db_pool_acquire(void) {
  mock_acquire_called++;

  if (mock_acquire_success) {
    mock_conn.pg_conn = (void *)0x1234;
    return &mock_conn;
  }

  return NULL;
}

void db_pool_release(DBConnection *conn) {
  mock_release_called++;
}

void *PQexec(void *conn, const char *query) {
  mock_pqexec_called++;
  strncpy(mock_query, query, sizeof(mock_query) - 1);
  return mock_result;
}

int PQresultStatus(const void *res) {
  return mock_result_status;
}

char *PQerrorMessage(const void *conn) {
  return mock_error_message;
}

void PQclear(void *res) {
  mock_pqclear_called++;
}

int PQntuples(const void *res) {
  mock_pqntuples_called++;
  return mock_row_count;
}

char *PQgetvalue(const void *res, int row, int field) {
  if (row < mock_row_count) {
    switch (field) {
      case 0:
        return mock_rows[row].id;

      case 1:
        return mock_rows[row].part_name;

      case 2:
        return mock_rows[row].sku;

      case 3:
        return mock_rows[row].quantity;

      case 4:
        return mock_rows[row].min_stock;

      case 5:
        return mock_rows[row].unit_cost;

      case 6:
        return mock_rows[row].last_restocked;
    }
  }

  return "";
}

/* ====================================================================
 * TEST SETUP
 * ==================================================================== */

static void reset_mocks(void) {
  mock_acquire_success = 1;
  mock_result_status = 2; // PGRES_TUPLES_OK
  mock_error_message[0] = '\0';
  mock_query[0] = '\0';
  mock_acquire_called = 0;
  mock_release_called = 0;
  mock_pqexec_called = 0;
  mock_pqclear_called = 0;
  mock_pqntuples_called = 0;
  mock_row_count = 0;
  memset(mock_rows, 0, sizeof(mock_rows));
}

static void setup_test_data(void) {
  mock_row_count = 2;
  strcpy(mock_rows[0].id, "1");
  strcpy(mock_rows[0].part_name, "Motor Oil");
  strcpy(mock_rows[0].sku, "MO-001");
  strcpy(mock_rows[0].quantity, "50");
  strcpy(mock_rows[0].min_stock, "10");
  strcpy(mock_rows[0].unit_cost, "25.50");
  strcpy(mock_rows[0].last_restocked, "2025-01-01");
  strcpy(mock_rows[1].id, "2");
  strcpy(mock_rows[1].part_name, "Air Filter");
  strcpy(mock_rows[1].sku, "AF-002");
  strcpy(mock_rows[1].quantity, "5");
  strcpy(mock_rows[1].min_stock, "10");
  strcpy(mock_rows[1].unit_cost, "15.75");
  strcpy(mock_rows[1].last_restocked, "2025-01-02");
}

/* ====================================================================
 * TEST CASES
 * ==================================================================== */

static void test_get_all_inventory_success(void **state) {
  reset_mocks();
  setup_test_data();
  InventoryItem items[5];
  int count = get_all_inventory(items, 5);
  assert_int_equal(count, 2);
  assert_int_equal(mock_acquire_called, 1);
  assert_int_equal(mock_pqexec_called, 1);
  assert_int_equal(items[0].id, 1);
  assert_string_equal(items[0].part_name, "Motor Oil");
  assert_int_equal(items[0].quantity, 50);
}

static void test_get_all_inventory_null_items(void **state) {
  reset_mocks();
  int count = get_all_inventory(NULL, 5);
  assert_int_equal(count, 0);
  assert_int_equal(mock_acquire_called, 0);
}

static void test_get_all_inventory_acquire_fails(void **state) {
  reset_mocks();
  mock_acquire_success = 0;
  InventoryItem items[5];
  int count = get_all_inventory(items, 5);
  assert_int_equal(count, 0);
}

static void test_update_inventory_success(void **state) {
  reset_mocks();
  mock_result_status = 1; // PGRES_COMMAND_OK
  bool result = update_inventory_quantity(1, 10);
  assert_true(result);
  assert_int_equal(mock_pqexec_called, 1);
}

static void test_get_low_stock_success(void **state) {
  reset_mocks();
  setup_test_data();
  InventoryItem items[5];
  int count = get_low_stock_items(items, 5);
  assert_int_equal(count, 2);
}

/* ====================================================================
 * MAIN
 * ==================================================================== */

int main(void) {
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_get_all_inventory_success),
    cmocka_unit_test(test_get_all_inventory_null_items),
    cmocka_unit_test(test_get_all_inventory_acquire_fails),

    cmocka_unit_test(test_update_inventory_success),

    cmocka_unit_test(test_get_low_stock_success),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
