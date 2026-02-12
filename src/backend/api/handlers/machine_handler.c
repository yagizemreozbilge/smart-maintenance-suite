#include "machine_handler.h"
#include "../../database/api_handlers.h"
#include "../../database/machine_service.h"
#include "../../database/cJSON.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>



char *serialize_machines_to_json(void) {
  cJSON *root = cJSON_CreateObject();
  cJSON *items = cJSON_CreateArray();
  cJSON *item1 = cJSON_CreateObject();
  cJSON_AddNumberToObject(item1, "id", 1);
  cJSON_AddStringToObject(item1, "name", "CNC Machine");
  cJSON_AddStringToObject(item1, "status", "running");
  cJSON_AddItemToArray(items, item1);
  cJSON_AddItemToObject(root, "machines", items);
  char *json = cJSON_Print(root);
  cJSON_Delete(root);
  return json;
}

void handle_machine_request(HttpRequest *req, HttpResponse *res) {
  if (strcmp(req->method, "GET") == 0) {
    char *json = serialize_machines_to_json();
    res->status_code = 200;
    strcpy(res->content_type, "application/json");
    strncpy(res->body, json, 8191);
    free(json);
  } else if (strcmp(req->method, "POST") == 0) {
    // Simple mock for onboarding since we need more robust JSON parsing for real POST
    // In a real scenario, we'd parse req->body here
    res->status_code = 201;
    strcpy(res->body, "{\"success\": true, \"message\": \"Machine onboarded\"}");
  }
}
