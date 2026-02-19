#include "machine_handler.h"
#include "../../database/api_handlers.h"
#include "../../database/machine_service.h"
#include "../../database/cJSON.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>



// Serialization is handled by api_handlers.c void serialize_machines_to_json is already defined there.

void handle_machine_request(HttpRequest *req, HttpResponse *res) {
  if (strcmp(req->method, "GET") == 0) {
    char *json = serialize_machines_to_json();
    res->status_code = 200;
    strcpy(res->content_type, "application/json");
    strncpy(res->body, json, 8191);
    res->body[8191] = '\0';
    free(json);
  } else if (strcmp(req->method, "POST") == 0) {
    cJSON *root = cJSON_Parse(req->body);

    if (root) {
      cJSON *name_item = cJSON_GetObjectItem(root, "name");
      cJSON *model_item = cJSON_GetObjectItem(root, "model");
      cJSON *location_item = cJSON_GetObjectItem(root, "location");
      cJSON *status_item = cJSON_GetObjectItem(root, "status");

      if (name_item && status_item) {
        const char *name = name_item->valuestring;
        const char *model = model_item ? model_item->valuestring : "Standard";
        const char *location = location_item ? location_item->valuestring : "Main Hall";
        const char *status = status_item->valuestring;
        // --- DUPLICATE CHECK ---
        Machine list[50];
        int count = get_all_machines(list, 50);
        bool duplicate = false;

        for (int i = 0; i < count; i++) {
          if (strcmp(list[i].name, name) == 0) {
            duplicate = true;
            break;
          }
        }

        if (duplicate) {
          res->status_code = 409;
          strcpy(res->content_type, "application/json");
          strcpy(res->body, "{\"error\": \"A machine with this name already exists in the registry.\"}");
          cJSON_Delete(root);
          return;
        }

        // ------------------------

        if (add_machine(name, model, location, status)) {
          res->status_code = 201;
          strcpy(res->content_type, "application/json");
          strcpy(res->body, "{\"success\": true, \"message\": \"Machine onboarded successfully\"}");
        } else {
          res->status_code = 500;
          strcpy(res->body, "{\"error\": \"Database error while onboarding machine\"}");
        }
      } else {
        res->status_code = 400;
        strcpy(res->body, "{\"error\": \"Missing required fields: name, status\"}");
      }

      cJSON_Delete(root);
    } else {
      res->status_code = 400;
      strcpy(res->body, "{\"error\": \"Invalid JSON\"}");
    }
  }
}
