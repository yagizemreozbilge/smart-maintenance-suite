#ifndef INVENTORY_SERVICE_H
#define INVENTORY_SERVICE_H

#include <stdbool.h>

typedef struct {
  int id;
  char part_name[100];
  char sku[50];
  int quantity;
  int min_stock_level;
  double unit_cost;
  char last_restocked_at[32];
} InventoryItem;

/**
 * Fetches all inventory items from the database.
 * @param out_items  Pointer to array to store items.
 * @param max_items  Maximum capacity of the array.
 * @return Number of items fetched.
 */
int get_all_inventory(InventoryItem *out_items, int max_items);

/**
 * Updates the quantity of a specific part.
 * @param part_id   The ID of the inventory item.
 * @param change    The amount to add (positive) or subtract (negative).
 * @return true on success, false otherwise.
 */
bool update_inventory_quantity(int part_id, int change);

/**
 * Checks for low stock items (quantity < min_stock_level).
 * @param out_items  Array to store low stock items.
 * @param max_items  Maximum capacity.
 * @return Number of low stock items found.
 */
int get_low_stock_items(InventoryItem *out_items, int max_items);

#endif
