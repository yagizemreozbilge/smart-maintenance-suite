#include "inventory_handler.h"
#include "../../database/api_handlers.h"
#include "../../database/cJSON.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char *serialize_inventory_to_json(void) {
  cJSON *root = cJSON_CreateObject();
  cJSON *item_array = cJSON_CreateArray();
  cJSON *item1 = cJSON_CreateObject();
  cJSON_AddNumberToObject(item1, "id", 1);
  cJSON_AddStringToObject(item1, "name", "Motor Oil");
  cJSON_AddNumberToObject(item1, "quantity", 25);
  cJSON_AddItemToArray(item_array, item1);
  cJSON_AddItemToObject(root, "inventory", item_array);
  char *json = cJSON_Print(root);
  cJSON_Delete(root);
  return json;
}

void handle_inventory_request(HttpRequest *req, HttpResponse *res) {
  if (strcmp(req->method, "GET") == 0) {
    char *json = serialize_inventory_to_json();
    res->status_code = 200;
    strcpy(res->content_type, "application/json");
    strncpy(res->body, json, 8191);
    free(json);
  } else {
    res->status_code = 405; // Method Not Allowed
    strcpy(res->body, "{\"error\": \"Method not supported for inventory\"}");
  }
}
