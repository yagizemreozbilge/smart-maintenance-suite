#include "maintenance_handler.h"
#include "../../database/api_handlers.h"
#include "../../database/maintenance_service.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../database/cJSON.h"

// Implemented in database/api_handlers.c

void handle_maintenance_request(HttpRequest *req, HttpResponse *res) {
  if (strcmp(req->method, "GET") == 0) {
    char *json = serialize_maintenance_to_json();
    res->status_code = 200;
    strcpy(res->content_type, "application/json");
    strncpy(res->body, json, 8191);
    res->body[8191] = '\0';
    free(json);
  } else if (strcmp(req->method, "POST") == 0) {
    printf("\n[DEBUG] Maintenance POST Received!\n");
    cJSON *root = cJSON_Parse(req->body);

    if (root) {
      cJSON *mid_item = cJSON_GetObjectItemCaseSensitive(root, "machine_id");
      cJSON *tech_item = cJSON_GetObjectItemCaseSensitive(root, "technician");
      cJSON *desc_item = cJSON_GetObjectItemCaseSensitive(root, "description");
      cJSON *cost_item = cJSON_GetObjectItemCaseSensitive(root, "cost");

      if (mid_item && tech_item && desc_item) {
        int mid = cJSON_IsNumber(mid_item) ? mid_item->valueint : atoi(mid_item->valuestring);
        const char *tech = tech_item->valuestring;
        const char *desc = desc_item->valuestring;
        double cost = (cost_item && cJSON_IsNumber(cost_item)) ? cost_item->valuedouble : 0.0;
        // --- DUPLICATE CHECK (Çift Kayıt Kontrolü) ---
        MaintenanceLog existing_logs[50];
        int count = get_maintenance_history(mid, existing_logs, 50);
        bool is_duplicate = false;

        for (int i = 0; i < count; i++) {
          if (strcmp(existing_logs[i].technician_name, tech) == 0 &&
              strcmp(existing_logs[i].description, desc) == 0) {
            is_duplicate = true;
            break;
          }
        }

        if (is_duplicate) {
          res->status_code = 409; // Conflict
          strcpy(res->content_type, "application/json");
          strcpy(res->body, "{\"error\": \"Duplicate entry detected. This specific maintenance log already exists.\"}");
          cJSON_Delete(root);
          return;
        }

        // ---------------------------------------------

        if (add_maintenance_log(mid, tech, desc, cost)) {
          res->status_code = 201;
          strcpy(res->content_type, "application/json");
          strcpy(res->body, "{\"success\": true, \"message\": \"Maintenance log saved to database!\"}");
        } else {
          res->status_code = 500;
          strcpy(res->body, "{\"error\": \"Database insertion failed (Check if Machine ID exists)\"}");
        }
      } else {
        res->status_code = 400;
        strcpy(res->body, "{\"error\": \"Missing fields: machine_id, technician, description\"}");
      }

      cJSON_Delete(root);
    } else {
      res->status_code = 400;
      strcpy(res->body, "{\"error\": \"Invalid JSON format\"}");
    }
  } else {
    res->status_code = 405;
    strcpy(res->body, "{\"error\": \"Method not allowed\"}");
  }
}
