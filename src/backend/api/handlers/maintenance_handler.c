#include "maintenance_handler.h"
#include "../../database/maintenance_service.h"
#include "../../database/cJSON.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char *serialize_maintenance_to_json(void) {
  MaintenanceLog logs[50];
  int count = get_all_maintenance_logs(logs, 50);
  cJSON *root = cJSON_CreateArray(); // Artık bir obje değil, doğrudan bir dizi (array) dönüyoruz.

  for (int i = 0; i < count; i++) {
    cJSON *log_item = cJSON_CreateObject();
    cJSON_AddNumberToObject(log_item, "id", logs[i].id);
    cJSON_AddNumberToObject(log_item, "machine_id", logs[i].machine_id);
    cJSON_AddStringToObject(log_item, "performer", logs[i].technician_name);
    cJSON_AddStringToObject(log_item, "description", logs[i].description);
    cJSON_AddNumberToObject(log_item, "cost", logs[i].cost);
    cJSON_AddStringToObject(log_item, "date", logs[i].log_date);
    cJSON_AddItemToArray(root, log_item);
  }

  char *json = cJSON_Print(root);
  cJSON_Delete(root);
  return json;
}

void handle_maintenance_request(HttpRequest *req, HttpResponse *res) {
  if (strcmp(req->method, "GET") == 0) {
    char *json = serialize_maintenance_to_json();
    res->status_code = 200;
    strcpy(res->content_type, "application/json");
    strncpy(res->body, json ? json : "[]", sizeof(res->body) - 1);
    res->body[sizeof(res->body) - 1] = '\0';

    if (json) free(json);
  } else {
    res->status_code = 405;
    strcpy(res->body, "{\"error\": \"Only GET supported for maintenance\"}");
  }
}
