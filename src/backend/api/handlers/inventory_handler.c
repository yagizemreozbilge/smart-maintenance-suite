#include "inventory_handler.h"
#include "../../database/api_handlers.h"
#include "../../database/cJSON.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Using definition from database/api_handlers.c

#include "../../database/inventory_service.h"

void handle_inventory_request(HttpRequest *req, HttpResponse *res) {
  if (strcmp(req->method, "GET") == 0) {
    char *json = serialize_inventory_to_json();
    res->status_code = 200;
    strcpy(res->content_type, "application/json");
    strncpy(res->body, json, 8191);
    res->body[8191] = '\0';
    free(json);
  } else if (strcmp(req->method, "POST") == 0) {
    cJSON *root = cJSON_Parse(req->body);

    if (root) {
      cJSON *name_item = cJSON_GetObjectItem(root, "part_name");
      cJSON *sku_item = cJSON_GetObjectItem(root, "sku");
      cJSON *qty_item = cJSON_GetObjectItem(root, "quantity");
      cJSON *min_item = cJSON_GetObjectItem(root, "min_stock");
      cJSON *cost_item = cJSON_GetObjectItem(root, "cost");

      if (name_item && sku_item) {
        const char *name = name_item->valuestring;
        const char *sku = sku_item->valuestring;
        int qty = qty_item ? qty_item->valueint : 0;
        int min = min_item ? min_item->valueint : 5;
        double cost = cost_item ? cost_item->valuedouble : 0.0;
        // --- DUPLICATE CHECK (SKU) ---
        InventoryItem items[100];
        int count = get_all_inventory(items, 100);
        bool duplicate = false;

        for (int i = 0; i < count; i++) {
          if (strcmp(items[i].sku, sku) == 0) {
            duplicate = true;
            break;
          }
        }

        if (duplicate) {
          res->status_code = 409;
          strcpy(res->content_type, "application/json");
          strcpy(res->body, "{\"error\": \"An item with this SKU already exists in inventory.\"}");
          cJSON_Delete(root);
          return;
        }

        // -----------------------------

        if (add_inventory_item(name, sku, qty, min, cost)) {
          res->status_code = 201;
          strcpy(res->content_type, "application/json");
          strcpy(res->body, "{\"success\": true, \"message\": \"Inventory item added successfully\"}");
        } else {
          res->status_code = 500;
          strcpy(res->body, "{\"error\": \"Database error while adding inventory item\"}");
        }
      } else {
        res->status_code = 400;
        strcpy(res->body, "{\"error\": \"Missing required fields: part_name, sku\"}");
      }

      cJSON_Delete(root);
    } else {
      res->status_code = 400;
      strcpy(res->body, "{\"error\": \"Invalid JSON\"}");
    }
  } else {
    res->status_code = 405;
    strcpy(res->body, "{\"error\": \"Method not supported\"}");
  }
}
