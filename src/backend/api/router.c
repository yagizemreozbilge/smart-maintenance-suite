#include "router.h"
#include "handlers/machine_handler.h"
#include "handlers/inventory_handler.h"
#include "handlers/auth_handler.h"
#include "handlers/maintenance_handler.h"
#include "handlers/report_handler.h"
#include <string.h>
#include <stdio.h>

void handle_route(HttpRequest *req, HttpResponse *res) {
  // Default 404
  res->status_code = 404;
  strcpy(res->content_type, "application/json");
  strcpy(res->body, "{\"error\": \"Route not found\"}");

  if (strstr(req->path, "/api/machines")) {
    handle_machine_request(req, res);
  } else if (strstr(req->path, "/api/inventory")) {
    handle_inventory_request(req, res);
  } else if (strstr(req->path, "/api/login")) {
    handle_auth_request(req, res);
  } else if (strstr(req->path, "/api/maintenance")) {
    handle_maintenance_request(req, res);
  } else if (strstr(req->path, "/api/reports")) {
    handle_report_request(req, res);
  } else if (strstr(req->path, "/api/alerts")) {
    // Simple inline for now since it's just a fetcher in legacy
    res->status_code = 200;
    char *json = serialize_alerts_to_json();
    strncpy(res->body, json, 8191);
    free(json);
  }
}
