#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// TEST_MODE'u kapat ki gerçek fonksiyonlar test edilsin
#undef TEST_MODE

// Header dosyalarını include et
#include "inventory_service.h"
#include "db_connection.h"
#include "logger.h"

/* ====================================================================
 * MOCK FUNCTIONS AND GLOBAL VARIABLES
 * ==================================================================== */

// Mock global variables
static DBConnection mock_conn;
static PGresult mock_result;
static int mock_acquire_success = 1;
static int mock_result_status = PGRES_TUPLES_OK;
static int mock_rows_returned = 0;
static char mock_error_message[256] = "";
static char mock_last_query[1024] = "";

// Mock call counters
static int mock_acquire_called = 0;
static int mock_release_called = 0;
static int mock_pqexec_called = 0;
static int mock_pqclear_called = 0;
static int mock_pqntuples_called = 0;
static int mock_pqgetvalue_called = 0;

// Mock row data
typedef struct {
  char id[10];
  char part_name[100];
  char sku[50];
  char quantity[10];
  char min_stock[10];
  char unit_cost[20];
  char last_restocked[32];
} MockInventoryRow;

static MockInventoryRow mock_rows[10];
static int mock_row_count = 0;

// Mock functions
DBConnection *db_pool_acquire(void) {
  mock_acquire_called++;

  if (mock_acquire_success) {
    mock_conn.pg_conn = (PGconn *)0x1234; // Non-null mock pointer
    return &mock_conn;
  }

  return NULL;
}

void db_pool_release(DBConnection *conn) {
  mock_release_called++;
  assert_true(conn == &mock_conn);
}

PGresult *PQexec(PGconn *conn, const char *query) {
  mock_pqexec_called++;
  strncpy(mock_last_query, query, sizeof(mock_last_query) - 1);
  return &mock_result;
}

ExecStatusType PQresultStatus(const PGresult *res) {
  return mock_result_status;
}

const char *PQerrorMessage(const PGconn *conn) {
  return mock_error_message;
}

void PQclear(PGresult *res) {
  mock_pqclear_called++;
  assert_true(res == &mock_result);
}

int PQntuples(const PGresult *res) {
  mock_pqntuples_called++;
  return mock_row_count;
}

char *PQgetvalue(const PGresult *res, int row, int field) {
  mock_pqgetvalue_called++;

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

// Logger mock
void LOG_ERROR(const char *format, ...) {
  // Just for coverage
}

void LOG_INFO(const char *format, ...) {
  // Just for coverage
}

/* ====================================================================
 * Test Setup/Teardown
 * ==================================================================== */

static void reset_mocks(void) {
  mock_acquire_success = 1;
  mock_result_status = PGRES_TUPLES_OK;
  mock_error_message[0] = '\0';
  mock_last_query[0] = '\0';
  mock_acquire_called = 0;
  mock_release_called = 0;
  mock_pqexec_called = 0;
  mock_pqclear_called = 0;
  mock_pqntuples_called = 0;
  mock_pqgetvalue_called = 0;
  mock_row_count = 0;
  memset(mock_rows, 0, sizeof(mock_rows));
}

static void setup_test_data(void) {
  // Setup mock inventory items
  mock_row_count = 3;
  // Item 1: Normal stock
  strcpy(mock_rows[0].id, "1");
  strcpy(mock_rows[0].part_name, "Motor Oil");
  strcpy(mock_rows[0].sku, "MO-001");
  strcpy(mock_rows[0].quantity, "50");
  strcpy(mock_rows[0].min_stock, "10");
  strcpy(mock_rows[0].unit_cost, "25.50");
  strcpy(mock_rows[0].last_restocked, "2025-01-01 10:00:00");
  // Item 2: Low stock
  strcpy(mock_rows[1].id, "2");
  strcpy(mock_rows[1].part_name, "Air Filter");
  strcpy(mock_rows[1].sku, "AF-002");
  strcpy(mock_rows[1].quantity, "5");
  strcpy(mock_rows[1].min_stock, "10");
  strcpy(mock_rows[1].unit_cost, "15.75");
  strcpy(mock_rows[1].last_restocked, "2025-01-02 14:30:00");
  // Item 3: Critical low stock
  strcpy(mock_rows[2].id, "3");
  strcpy(mock_rows[2].part_name, "Brake Pads");
  strcpy(mock_rows[2].sku, "BP-003");
  strcpy(mock_rows[2].quantity, "2");
  strcpy(mock_rows[2].min_stock, "15");
  strcpy(mock_rows[2].unit_cost, "45.00");
  strcpy(mock_rows[2].last_restocked, "2025-01-03 09:15:00");
}

/* ====================================================================
 * Test Cases for get_all_inventory
 * ==================================================================== */

static void test_get_all_inventory_success(void **state) {
  reset_mocks();
  setup_test_data();
  InventoryItem items[5];
  int count = get_all_inventory(items, 5);
  assert_int_equal(count, 3);
  assert_int_equal(mock_acquire_called, 1);
  assert_int_equal(mock_pqexec_called, 1);
  assert_int_equal(mock_pqntuples_called, 1);
  assert_int_equal(mock_pqgetvalue_called, 3 * 7); // 3 rows * 7 fields
  assert_int_equal(mock_pqclear_called, 1);
  assert_int_equal(mock_release_called, 1);
  // Verify query
  assert_non_null(strstr(mock_last_query, "SELECT id, part_name, sku"));
  assert_non_null(strstr(mock_last_query, "FROM inventory"));
  assert_non_null(strstr(mock_last_query, "ORDER BY id ASC"));
  // Verify first item
  assert_int_equal(items[0].id, 1);
  assert_string_equal(items[0].part_name, "Motor Oil");
  assert_string_equal(items[0].sku, "MO-001");
  assert_int_equal(items[0].quantity, 50);
  assert_int_equal(items[0].min_stock_level, 10);
  assert_float_equal(items[0].unit_cost, 25.50, 0.001);
  assert_string_equal(items[0].last_restocked_at, "2025-01-01 10:00:00");
  // Verify second item
  assert_int_equal(items[1].id, 2);
  assert_string_equal(items[1].part_name, "Air Filter");
  assert_string_equal(items[1].sku, "AF-002");
  assert_int_equal(items[1].quantity, 5);
  assert_int_equal(items[1].min_stock_level, 10);
  assert_float_equal(items[1].unit_cost, 15.75, 0.001);
  // Verify third item
  assert_int_equal(items[2].id, 3);
  assert_string_equal(items[2].part_name, "Brake Pads");
  assert_int_equal(items[2].quantity, 2);
  assert_int_equal(items[2].min_stock_level, 15);
  assert_float_equal(items[2].unit_cost, 45.00, 0.001);
}

static void test_get_all_inventory_null_items(void **state) {
  reset_mocks();
  int count = get_all_inventory(NULL, 5);
  assert_int_equal(count, 0);
  assert_int_equal(mock_acquire_called, 0); // Early return
}

static void test_get_all_inventory_max_items_zero(void **state) {
  reset_mocks();
  InventoryItem items[5];
  int count = get_all_inventory(items, 0);
  assert_int_equal(count, 0);
  assert_int_equal(mock_acquire_called, 0); // Early return
}

static void test_get_all_inventory_acquire_fails(void **state) {
  reset_mocks();
  mock_acquire_success = 0;
  InventoryItem items[5];
  int count = get_all_inventory(items, 5);
  assert_int_equal(count, 0);
  assert_int_equal(mock_acquire_called, 1);
  assert_int_equal(mock_pqexec_called, 0);
  assert_int_equal(mock_release_called, 0);
}

static void test_get_all_inventory_db_error(void **state) {
  reset_mocks();
  mock_result_status = PGRES_FATAL_ERROR;
  strcpy(mock_error_message, "Database connection error");
  InventoryItem items[5];
  int count = get_all_inventory(items, 5);
  assert_int_equal(count, 0);
  assert_int_equal(mock_acquire_called, 1);
  assert_int_equal(mock_pqexec_called, 1);
  assert_int_equal(mock_pqclear_called, 1);
  assert_int_equal(mock_release_called, 1);
}

static void test_get_all_inventory_more_rows_than_max(void **state) {
  reset_mocks();
  setup_test_data(); // 3 rows
  mock_row_count = 10; // More rows
  InventoryItem items[5];
  int count = get_all_inventory(items, 5);
  assert_int_equal(count, 5); // Should return max_items
  assert_int_equal(mock_pqgetvalue_called, 5 * 7); // Only 5 rows processed
}

static void test_get_all_inventory_empty_result(void **state) {
  reset_mocks();
  mock_row_count = 0;
  InventoryItem items[5];
  int count = get_all_inventory(items, 5);
  assert_int_equal(count, 0);
  assert_int_equal(mock_pqntuples_called, 1);
  assert_int_equal(mock_pqgetvalue_called, 0);
}

/* ====================================================================
 * Test Cases for update_inventory_quantity
 * ==================================================================== */

static void test_update_inventory_quantity_success(void **state) {
  reset_mocks();
  mock_result_status = PGRES_COMMAND_OK;
  bool result = update_inventory_quantity(1, 10);
  assert_true(result);
  assert_int_equal(mock_acquire_called, 1);
  assert_int_equal(mock_pqexec_called, 1);
  assert_int_equal(mock_pqclear_called, 1);
  assert_int_equal(mock_release_called, 1);
  // Verify query
  assert_non_null(strstr(mock_last_query, "UPDATE inventory"));
  assert_non_null(strstr(mock_last_query, "SET quantity = quantity + (10)"));
  assert_non_null(strstr(mock_last_query, "WHERE id = 1"));
}

static void test_update_inventory_quantity_decrease(void **state) {
  reset_mocks();
  mock_result_status = PGRES_COMMAND_OK;
  bool result = update_inventory_quantity(2, -5);
  assert_true(result);
  assert_non_null(strstr(mock_last_query, "quantity + (-5)"));
  assert_non_null(strstr(mock_last_query, "WHERE id = 2"));
}

static void test_update_inventory_quantity_acquire_fails(void **state) {
  reset_mocks();
  mock_acquire_success = 0;
  bool result = update_inventory_quantity(1, 10);
  assert_false(result);
  assert_int_equal(mock_acquire_called, 1);
  assert_int_equal(mock_pqexec_called, 0);
  assert_int_equal(mock_release_called, 0);
}

static void test_update_inventory_quantity_db_error(void **state) {
  reset_mocks();
  mock_result_status = PGRES_FATAL_ERROR;
  strcpy(mock_error_message, "Update failed");
  bool result = update_inventory_quantity(1, 10);
  assert_false(result);
  assert_int_equal(mock_acquire_called, 1);
  assert_int_equal(mock_pqexec_called, 1);
  assert_int_equal(mock_pqclear_called, 1);
  assert_int_equal(mock_release_called, 1);
}

static void test_update_inventory_quantity_non_existent(void **state) {
  reset_mocks();
  mock_result_status = PGRES_COMMAND_OK; // Even non-existent returns OK if syntax correct
  bool result = update_inventory_quantity(999, 10);
  assert_true(result); // PostgreSQL returns OK even if no rows updated
}

/* ====================================================================
 * Test Cases for get_low_stock_items
 * ==================================================================== */

static void test_get_low_stock_items_success(void **state) {
  reset_mocks();
  setup_test_data(); // Contains low stock items
  // Only low stock items (quantity < min_stock)
  // In real scenario, DB would filter, but we'll simulate
  mock_row_count = 2; // Items 2 and 3 are low stock
  InventoryItem items[5];
  int count = get_low_stock_items(items, 5);
  assert_int_equal(count, 2);
  assert_int_equal(mock_acquire_called, 1);
  assert_int_equal(mock_pqexec_called, 1);
  assert_int_equal(mock_pqclear_called, 1);
  assert_int_equal(mock_release_called, 1);
  // Verify query
  assert_non_null(strstr(mock_last_query, "SELECT id, part_name, sku"));
  assert_non_null(strstr(mock_last_query, "FROM inventory"));
  assert_non_null(strstr(mock_last_query, "WHERE quantity < min_stock_level"));
}

static void test_get_low_stock_items_null_items(void **state) {
  reset_mocks();
  int count = get_low_stock_items(NULL, 5);
  assert_int_equal(count, 0);
  assert_int_equal(mock_acquire_called, 0);
}

static void test_get_low_stock_items_max_items_zero(void **state) {
  reset_mocks();
  InventoryItem items[5];
  int count = get_low_stock_items(items, 0);
  assert_int_equal(count, 0);
  assert_int_equal(mock_acquire_called, 0);
}

static void test_get_low_stock_items_acquire_fails(void **state) {
  reset_mocks();
  mock_acquire_success = 0;
  InventoryItem items[5];
  int count = get_low_stock_items(items, 5);
  assert_int_equal(count, 0);
  assert_int_equal(mock_acquire_called, 1);
  assert_int_equal(mock_pqexec_called, 0);
}

static void test_get_low_stock_items_db_error(void **state) {
  reset_mocks();
  mock_result_status = PGRES_FATAL_ERROR;
  InventoryItem items[5];
  int count = get_low_stock_items(items, 5);
  assert_int_equal(count, 0);
  assert_int_equal(mock_acquire_called, 1);
  assert_int_equal(mock_pqexec_called, 1);
  assert_int_equal(mock_pqclear_called, 1);
  assert_int_equal(mock_release_called, 1);
}

static void test_get_low_stock_items_no_results(void **state) {
  reset_mocks();
  mock_row_count = 0;
  InventoryItem items[5];
  int count = get_low_stock_items(items, 5);
  assert_int_equal(count, 0);
  assert_int_equal(mock_pqntuples_called, 1);
  assert_int_equal(mock_pqgetvalue_called, 0);
}

static void test_get_low_stock_items_more_than_max(void **state) {
  reset_mocks();
  setup_test_data();
  mock_row_count = 10; // 10 low stock items
  InventoryItem items[5];
  int count = get_low_stock_items(items, 5);
  assert_int_equal(count, 5);
  assert_int_equal(mock_pqgetvalue_called, 5 * 7);
}

/* ====================================================================
 * Test Cases for edge cases and data conversion
 * ==================================================================== */

static void test_inventory_item_string_bounds(void **state) {
  reset_mocks();
  // Setup item with boundary values
  mock_row_count = 1;
  // Create very long strings
  char long_part_name[200];
  char long_sku[100];
  memset(long_part_name, 'A', 150);
  long_part_name[150] = '\0';
  memset(long_sku, 'B', 60);
  long_sku[60] = '\0';
  strcpy(mock_rows[0].id, "999");
  strcpy(mock_rows[0].part_name, long_part_name);
  strcpy(mock_rows[0].sku, long_sku);
  strcpy(mock_rows[0].quantity, "2147483647"); // Max int
  strcpy(mock_rows[0].min_stock, "2147483647");
  strcpy(mock_rows[0].unit_cost, "999999.99");
  strcpy(mock_rows[0].last_restocked, "2025-12-31 23:59:59");
  InventoryItem items[1];
  int count = get_all_inventory(items, 1);
  assert_int_equal(count, 1);
  assert_int_equal(items[0].id, 999);
  assert_int_equal(items[0].quantity, 2147483647);
  assert_int_equal(items[0].min_stock_level, 2147483647);
  assert_float_equal(items[0].unit_cost, 999999.99, 0.01);
  // Check string truncation (should be null-terminated)
  assert_int_equal(strlen(items[0].part_name), 99); // Truncated to 99 + null
  assert_int_equal(strlen(items[0].sku), 49); // Truncated to 49 + null
}

static void test_inventory_item_invalid_numbers(void **state) {
  reset_mocks();
  mock_row_count = 1;
  // Invalid number strings
  strcpy(mock_rows[0].id, "abc");
  strcpy(mock_rows[0].part_name, "Test");
  strcpy(mock_rows[0].sku, "TEST");
  strcpy(mock_rows[0].quantity, "not_a_number");
  strcpy(mock_rows[0].min_stock, "invalid");
  strcpy(mock_rows[0].unit_cost, "xyz");
  strcpy(mock_rows[0].last_restocked, "2025-01-01");
  InventoryItem items[1];
  int count = get_all_inventory(items, 1);
  assert_int_equal(count, 1);
  assert_int_equal(items[0].id, 0); // atoi returns 0 for invalid
  assert_int_equal(items[0].quantity, 0);
  assert_int_equal(items[0].min_stock_level, 0);
  assert_float_equal(items[0].unit_cost, 0.0, 0.001);
}

/* ====================================================================
 * Test Cases for null terminator checks
 * ==================================================================== */

static void test_string_terminators(void **state) {
  reset_mocks();
  mock_row_count = 1;
  strcpy(mock_rows[0].id, "1");
  strcpy(mock_rows[0].part_name, "Test Part");
  strcpy(mock_rows[0].sku, "TEST-SKU");
  strcpy(mock_rows[0].quantity, "10");
  strcpy(mock_rows[0].min_stock, "5");
  strcpy(mock_rows[0].unit_cost, "10.50");
  strcpy(mock_rows[0].last_restocked, "2025-01-01");
  InventoryItem items[1];
  get_all_inventory(items, 1);
  // Check that strings are properly null-terminated
  assert_int_equal(items[0].part_name[99], '\0');
  assert_int_equal(items[0].sku[49], '\0');
  assert_int_equal(items[0].last_restocked_at[31], '\0');
}

/* ====================================================================
 * Test Cases for multiple calls and state management
 * ==================================================================== */

static void test_multiple_calls(void **state) {
  reset_mocks();
  setup_test_data();
  InventoryItem items1[5];
  InventoryItem items2[5];
  // First call
  int count1 = get_all_inventory(items1, 5);
  assert_int_equal(count1, 3);
  // Reset call counters but keep mock data
  mock_acquire_called = 0;
  mock_pqexec_called = 0;
  // Second call
  int count2 = get_all_inventory(items2, 5);
  assert_int_equal(count2, 3);
  assert_int_equal(mock_acquire_called, 1);
  assert_int_equal(mock_pqexec_called, 1);
}

/* ====================================================================
 * Test Cases for update_inventory_quantity with various values
 * ==================================================================== */

static void test_update_inventory_quantity_edge_values(void **state) {
  reset_mocks();
  mock_result_status = PGRES_COMMAND_OK;
  // Test with max int
  bool result1 = update_inventory_quantity(1, 2147483647);
  assert_true(result1);
  assert_non_null(strstr(mock_last_query, "2147483647"));
  // Test with min int
  bool result2 = update_inventory_quantity(1, -2147483647);
  assert_true(result2);
  assert_non_null(strstr(mock_last_query, "-2147483647"));
  // Test with zero
  bool result3 = update_inventory_quantity(1, 0);
  assert_true(result3);
  assert_non_null(strstr(mock_last_query, "0"));
}

/* ====================================================================
 * Main test runner
 * ==================================================================== */

int main(void) {
  const struct CMUnitTest tests[] = {
    // get_all_inventory tests
    cmocka_unit_test(test_get_all_inventory_success),
    cmocka_unit_test(test_get_all_inventory_null_items),
    cmocka_unit_test(test_get_all_inventory_max_items_zero),
    cmocka_unit_test(test_get_all_inventory_acquire_fails),
    cmocka_unit_test(test_get_all_inventory_db_error),
    cmocka_unit_test(test_get_all_inventory_more_rows_than_max),
    cmocka_unit_test(test_get_all_inventory_empty_result),

    // update_inventory_quantity tests
    cmocka_unit_test(test_update_inventory_quantity_success),
    cmocka_unit_test(test_update_inventory_quantity_decrease),
    cmocka_unit_test(test_update_inventory_quantity_acquire_fails),
    cmocka_unit_test(test_update_inventory_quantity_db_error),
    cmocka_unit_test(test_update_inventory_quantity_non_existent),
    cmocka_unit_test(test_update_inventory_quantity_edge_values),

    // get_low_stock_items tests
    cmocka_unit_test(test_get_low_stock_items_success),
    cmocka_unit_test(test_get_low_stock_items_null_items),
    cmocka_unit_test(test_get_low_stock_items_max_items_zero),
    cmocka_unit_test(test_get_low_stock_items_acquire_fails),
    cmocka_unit_test(test_get_low_stock_items_db_error),
    cmocka_unit_test(test_get_low_stock_items_no_results),
    cmocka_unit_test(test_get_low_stock_items_more_than_max),

    // Edge cases
    cmocka_unit_test(test_inventory_item_string_bounds),
    cmocka_unit_test(test_inventory_item_invalid_numbers),
    cmocka_unit_test(test_string_terminators),
    cmocka_unit_test(test_multiple_calls),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
