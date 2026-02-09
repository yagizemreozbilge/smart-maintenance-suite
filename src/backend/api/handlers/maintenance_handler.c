#include "maintenance_handler.h"
#include "../../database/api_handlers.h"
#include <string.h>
#include <stdlib.h>

void handle_maintenance_request(HttpRequest *req, HttpResponse *res) {
  if (strcmp(req->method, "GET") == 0) {
    char *json = serialize_maintenance_to_json();
    res->status_code = 200;
    strcpy(res->content_type, "application/json");
    strncpy(res->body, json, 8191);
    free(json);
  } else {
    res->status_code = 405;
    strcpy(res->body, "{\"error\": \"Only GET supported for maintenance via modular API currently\"}");
  }
}
