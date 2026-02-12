#include "inventory_service.h"
#include "db_connection.h"
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int get_all_inventory(InventoryItem *out_items, int max_items) {
  DBConnection *conn_wrapper = db_pool_acquire();

  if (!conn_wrapper) return 0;

  const char *query = "SELECT id, part_name, sku, quantity, min_stock_level, unit_cost, "
                      "to_char(last_restocked_at, 'YYYY-MM-DD HH24:MI:SS') FROM inventory ORDER BY id ASC;";
  PGresult *res = PQexec(conn_wrapper->pg_conn, query);

  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    LOG_ERROR("Failed to fetch inventory: %s", PQerrorMessage(conn_wrapper->pg_conn));
    PQclear(res);
    db_pool_release(conn_wrapper);
    return 0;
  }

  int rows = PQntuples(res);
  int count = (rows < max_items) ? rows : max_items;

  for (int i = 0; i < count; i++) {
    out_items[i].id = atoi(PQgetvalue(res, i, 0));
    strncpy(out_items[i].part_name, PQgetvalue(res, i, 1), 99);
    strncpy(out_items[i].sku, PQgetvalue(res, i, 2), 49);
    out_items[i].quantity = atoi(PQgetvalue(res, i, 3));
    out_items[i].min_stock_level = atoi(PQgetvalue(res, i, 4));
    out_items[i].unit_cost = atof(PQgetvalue(res, i, 5));
    strncpy(out_items[i].last_restocked_at, PQgetvalue(res, i, 6), 31);
  }

  PQclear(res);
  db_pool_release(conn_wrapper);
  return count;
}

bool update_inventory_quantity(int part_id, int change) {
  DBConnection *conn_wrapper = db_pool_acquire();

  if (!conn_wrapper) return false;

  char query[256];
  snprintf(query, sizeof(query),
           "UPDATE inventory SET quantity = quantity + (%d) WHERE id = %d;",
           change, part_id);
  PGresult *res = PQexec(conn_wrapper->pg_conn, query);
  bool success = (PQresultStatus(res) == PGRES_COMMAND_OK);

  if (!success) {
    LOG_ERROR("Failed to update inventory: %s", PQerrorMessage(conn_wrapper->pg_conn));
  }

  PQclear(res);
  db_pool_release(conn_wrapper);
  return success;
}

int get_low_stock_items(InventoryItem *out_items, int max_items) {
  DBConnection *conn_wrapper = db_pool_acquire();

  if (!conn_wrapper) return 0;

  const char *query = "SELECT id, part_name, sku, quantity, min_stock_level, unit_cost, "
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
    strncpy(out_items[i].sku, PQgetvalue(res, i, 2), 49);
    out_items[i].quantity = atoi(PQgetvalue(res, i, 3));
    out_items[i].min_stock_level = atoi(PQgetvalue(res, i, 4));
    out_items[i].unit_cost = atof(PQgetvalue(res, i, 5));
    strncpy(out_items[i].last_restocked_at, PQgetvalue(res, i, 6), 31);
  }

  PQclear(res);
  db_pool_release(conn_wrapper);
  return count;
}

// ====================================================================
// MOCK IMPLEMENTATIONS FOR TESTING
// ====================================================================

#ifdef TEST_MODE

int get_all_inventory(InventoryItem *items, int max_count) {
  if (!items || max_count <= 0) return 0;

  items[0].id = 1;
  strcpy(items[0].name, "Motor Oil");
  items[0].quantity = 25;
  items[0].min_quantity = 10;
  items[1].id = 2;
  strcpy(items[1].name, "Bearings");
  items[1].quantity = 50;
  items[1].min_quantity = 20;
  items[2].id = 3;
  strcpy(items[2].name, "Filters");
  items[2].quantity = 15;
  items[2].min_quantity = 5;
  return 3;
}

#endif /* TEST_MODE */
