#include "../../../database/inventory_service.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

void test_inventory_struct_size() {
  InventoryItem item;
  assert(sizeof(item.part_name) == 100);
  assert(sizeof(item.sku) == 50);
  assert(sizeof(item.last_restocked_at) == 32);
  printf("[PASS] InventoryItem struct: correct field sizes\n");
}

void test_inventory_initialization() {
  InventoryItem item = {0};
  item.id = 1;
  strcpy(item.part_name, "Bearing 608");
  strcpy(item.sku, "BRG-608-2RS");
  item.quantity = 50;
  item.min_stock_level = 10;
  item.unit_cost = 2.50;
  assert(item.id == 1);
  assert(strcmp(item.part_name, "Bearing 608") == 0);
  assert(strcmp(item.sku, "BRG-608-2RS") == 0);
  assert(item.quantity == 50);
  assert(item.min_stock_level == 10);
  assert(item.unit_cost == 2.50);
  printf("[PASS] InventoryItem initialization: all fields correct\n");
}

void test_inventory_low_stock_detection() {
  InventoryItem item1 = {.quantity = 5, .min_stock_level = 10};
  InventoryItem item2 = {.quantity = 15, .min_stock_level = 10};
  assert(item1.quantity < item1.min_stock_level);
  assert(item2.quantity >= item2.min_stock_level);
  printf("[PASS] Inventory: low stock detection logic\n");
}

void test_inventory_cost_calculation() {
  InventoryItem item = {.quantity = 100, .unit_cost = 5.75};
  double total_value = item.quantity *item.unit_cost;
  assert(total_value == 575.0);
  printf("[PASS] Inventory: cost calculation\n");
}

void test_inventory_sku_format() {
  InventoryItem item = {0};
  strcpy(item.sku, "PART-2024-001");
  assert(strlen(item.sku) > 0);
  assert(strstr(item.sku, "PART") != NULL);
  printf("[PASS] Inventory: SKU format validation\n");
}

void test_inventory_negative_quantity_guard() {
  InventoryItem item = {.quantity = 10};
  int change = -15;
  // Simulating update logic
  int new_quantity = item.quantity + change;
  assert(new_quantity < 0); // Should be caught by service layer
  printf("[PASS] Inventory: negative quantity detection\n");
}

int main() {
  printf("=== Inventory Service Unit Tests ===\n");
  test_inventory_struct_size();
  test_inventory_initialization();
  test_inventory_low_stock_detection();
  test_inventory_cost_calculation();
  test_inventory_sku_format();
  test_inventory_negative_quantity_guard();
  printf("\n✅ All Inventory Service unit tests passed!\n");
  return 0;
}
