#include "machine_handler.h"
#include "../../database/api_handlers.h"
#include "../../database/machine_service.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

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
