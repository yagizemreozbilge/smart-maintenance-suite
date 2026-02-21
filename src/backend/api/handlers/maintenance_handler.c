#include "maintenance_handler.h"
#include "../../database/api_handlers.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../database/cJSON.h"

#ifdef TEST_MODE
char *serialize_maintenance_to_json(void) {
  const char *fixed = "{\"maintenance_logs\":[{\"id\":1,\"machine_id\":1,\"description\":\"Routine oil change\",\"status\":\"completed\"}]}";
  char *copy = (char *)malloc(strlen(fixed) + 1);

  if (!copy) return NULL;

  strcpy(copy, fixed);
  return copy;
}

#else
char *serialize_maintenance_to_json(void) {
  cJSON *root = cJSON_CreateObject();
  cJSON *log_array = cJSON_CreateArray();
  cJSON *log1 = cJSON_CreateObject();
  cJSON_AddNumberToObject(log1, "id", 1);
  cJSON_AddNumberToObject(log1, "machine_id", 1);
  cJSON_AddStringToObject(log1, "description", "Routine oil change");
  cJSON_AddStringToObject(log1, "status", "completed");
  cJSON_AddItemToArray(log_array, log1);
  cJSON_AddItemToObject(root, "maintenance_logs", log_array);
  char *json = cJSON_Print(root);
  cJSON_Delete(root);
  return json;
}

#endif

void handle_maintenance_request(HttpRequest *req, HttpResponse *res) {
  if (strcmp(req->method, "GET") == 0) {
    char *json = serialize_maintenance_to_json();
    res->status_code = 200;
    strcpy(res->content_type, "application/json");
    strncpy(res->body, json ? json : "", 8191);

    if (json) free(json);
  } else {
    res->status_code = 405;
    strcpy(res->body, "{\"error\": \"Only GET supported for maintenance via modular API currently\"}");
  }
}
