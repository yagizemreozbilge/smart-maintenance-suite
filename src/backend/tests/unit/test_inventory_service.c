#include <stdio.h>
#include <string.h>
#include <assert.h>

#define TEST_MODE
#include "inventory_service.h"

void test_get_all_inventory_success() {
  InventoryItem items[5];
  int count = get_all_inventory(items, 5);
  assert(count == 1);
  assert(items[0].id == 1);
  assert(strcmp(items[0].part_name, "Motor Oil") == 0);
  assert(strcmp(items[0].sku, "MO-001") == 0);
  assert(items[0].quantity == 50);
  assert(items[0].min_stock_level == 10);
  assert(items[0].unit_cost == 25.5);
  assert(strcmp(items[0].last_restocked_at, "2025-01-01") == 0);
}

void test_get_all_inventory_null() {
  int count = get_all_inventory(NULL, 5);
  assert(count == 0);
}

void test_get_all_inventory_invalid_size() {
  InventoryItem items[1];
  int count = get_all_inventory(items, 0);
  assert(count == 0);
}

void test_update_inventory_quantity() {
  bool result = update_inventory_quantity(1, 5);
  assert(result == true);
}

void test_get_low_stock_items_success() {
  InventoryItem items[5];
  int count = get_low_stock_items(items, 5);
  assert(count == 1);
  assert(items[0].id == 2);
  assert(items[0].quantity == 5);
  assert(items[0].min_stock_level == 10);
}

void test_get_low_stock_items_null() {
  int count = get_low_stock_items(NULL, 5);
  assert(count == 0);
}

void test_get_low_stock_items_invalid_size() {
  InventoryItem items[1];
  int count = get_low_stock_items(items, 0);
  assert(count == 0);
}

int main() {
  test_get_all_inventory_success();
  test_get_all_inventory_null();
  test_get_all_inventory_invalid_size();
  test_update_inventory_quantity();
  test_get_low_stock_items_success();
  test_get_low_stock_items_null();
  test_get_low_stock_items_invalid_size();
  printf("All inventory service tests passed.\n");
  return 0;
}
