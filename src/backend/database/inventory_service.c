#include "inventory_service.h"
#include "db_connection.h"
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef TEST_MODE

int get_all_inventory(InventoryItem *out_items, int max_items) {
  if (!out_items || max_items <= 0)
    return 0;

  DBConnection *conn_wrapper = db_pool_acquire();

  if (!conn_wrapper)
    return 0;

  const char *query =
    "SELECT id, part_name, sku, quantity, min_stock_level, unit_cost, "
    "to_char(last_restocked_at, 'YYYY-MM-DD HH24:MI:SS') "
    "FROM inventory ORDER BY id ASC;";
  PGresult *res = PQexec(conn_wrapper->pg_conn, query);

  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    LOG_ERROR("Failed to fetch inventory: %s",
              PQerrorMessage(conn_wrapper->pg_conn));
    PQclear(res);
    db_pool_release(conn_wrapper);
    return 0;
  }

  int rows = PQntuples(res);
  int count = (rows < max_items) ? rows : max_items;

  for (int i = 0; i < count; i++) {
    out_items[i].id = atoi(PQgetvalue(res, i, 0));
    strncpy(out_items[i].part_name, PQgetvalue(res, i, 1), 99);
    out_items[i].part_name[99] = '\0';
    strncpy(out_items[i].sku, PQgetvalue(res, i, 2), 49);
    out_items[i].sku[49] = '\0';
    out_items[i].quantity = atoi(PQgetvalue(res, i, 3));
    out_items[i].min_stock_level = atoi(PQgetvalue(res, i, 4));
    out_items[i].unit_cost = atof(PQgetvalue(res, i, 5));
    strncpy(out_items[i].last_restocked_at,
            PQgetvalue(res, i, 6), 31);
    out_items[i].last_restocked_at[31] = '\0';
  }

  PQclear(res);
  db_pool_release(conn_wrapper);
  return count;
}

bool update_inventory_quantity(int part_id, int change) {
  DBConnection *conn_wrapper = db_pool_acquire();

  if (!conn_wrapper)
    return false;

  char query[256];
  snprintf(query, sizeof(query),
           "UPDATE inventory SET quantity = quantity + (%d) WHERE id = %d;",
           change, part_id);
  PGresult *res = PQexec(conn_wrapper->pg_conn, query);
  bool success = (PQresultStatus(res) == PGRES_COMMAND_OK);

  if (!success) {
    LOG_ERROR("Failed to update inventory: %s",
              PQerrorMessage(conn_wrapper->pg_conn));
  }

  PQclear(res);
  db_pool_release(conn_wrapper);
  return success;
}

int get_low_stock_items(InventoryItem *out_items, int max_items) {
  if (!out_items || max_items <= 0)
    return 0;

  DBConnection *conn_wrapper = db_pool_acquire();

  if (!conn_wrapper)
    return 0;

  const char *query =
    "SELECT id, part_name, sku, quantity, min_stock_level, unit_cost, "
    "to_char(last_restocked_at, 'YYYY-MM-DD HH24:MI:SS') "
    "FROM inventory WHERE quantity < min_stock_level;";
  PGresult *res = PQexec(conn_wrapper->pg_conn, query);

  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    PQclear(res);
    db_pool_release(conn_wrapper);
    return 0;
  }

  int rows = PQntuples(res);
  int count = (rows < max_items) ? rows : max_items;

  for (int i = 0; i < count; i++) {
    out_items[i].id = atoi(PQgetvalue(res, i, 0));
    strncpy(out_items[i].part_name, PQgetvalue(res, i, 1), 99);
    out_items[i].part_name[99] = '\0';
    strncpy(out_items[i].sku, PQgetvalue(res, i, 2), 49);
    out_items[i].sku[49] = '\0';
    out_items[i].quantity = atoi(PQgetvalue(res, i, 3));
    out_items[i].min_stock_level = atoi(PQgetvalue(res, i, 4));
    out_items[i].unit_cost = atof(PQgetvalue(res, i, 5));
    strncpy(out_items[i].last_restocked_at,
            PQgetvalue(res, i, 6), 31);
    out_items[i].last_restocked_at[31] = '\0';
  }

  PQclear(res);
  db_pool_release(conn_wrapper);
  return count;
}

bool add_inventory_item(const char *name, const char *sku, int quantity, int min_stock, double cost) {
  DBConnection *conn_wrapper = db_pool_acquire();

  if (!conn_wrapper) return false;

  char query[1024];
  snprintf(query, sizeof(query),
           "INSERT INTO inventory (part_name, sku, quantity, min_stock_level, unit_cost) "
           "VALUES ('%s', '%s', %d, %d, %.2f);",
           name, sku, quantity, min_stock, cost);
  PGresult *res = PQexec(conn_wrapper->pg_conn, query);
  bool success = (PQresultStatus(res) == PGRES_COMMAND_OK);
  PQclear(res);
  db_pool_release(conn_wrapper);
  return success;
}

#endif  // TEST_MODE


// ====================================================================
// MOCK IMPLEMENTATIONS FOR TESTING
// ====================================================================

#ifdef TEST_MODE

int get_all_inventory(InventoryItem *items, int max_count) {
  if (!items || max_count <= 0)
    return 0;

  items[0].id = 1;
  strcpy(items[0].part_name, "Motor Oil");
  strcpy(items[0].sku, "MO-001");
  items[0].quantity = 50;
  items[0].min_stock_level = 10;
  items[0].unit_cost = 25.5;
  strcpy(items[0].last_restocked_at, "2025-01-01");
  return 1;
}

bool update_inventory_quantity(int part_id, int change) {
  (void)part_id;
  (void)change;
  return true;
}

int get_low_stock_items(InventoryItem *items, int max_count) {
  if (!items || max_count <= 0)
    return 0;

  items[0].id = 2;
  items[0].quantity = 5;
  items[0].min_stock_level = 10;
  return 1;
}

bool add_inventory_item(const char *name, const char *sku, int quantity, int min_stock, double cost) {
  (void)name;
  (void)sku;
  (void)quantity;
  (void)min_stock;
  (void)cost;
  return true;
}

#endif
