#include "router.h"
#include "handlers/machine_handler.h"
#include "handlers/inventory_handler.h"
#include "handlers/auth_handler.h"
#include "handlers/maintenance_handler.h"
#include "handlers/report_handler.h"
#include "handlers/fault_handler.h"
#include "../database/alert_service.h"
#include "../database/api_handlers.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>



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
  } else if (strstr(req->path, "/api/faults")) {
    handle_fault_request(req, res);
  } else if (strstr(req->path, "/api/sensors")) {
    // Sensör verilerini getiren rota
    int mid = 1;

    if (req->query_params[0] != '\0') {
      sscanf(req->query_params, "id=%d", &mid);
    }

    char *json = serialize_sensors_to_json(mid);
    res->status_code = 200;
    strcpy(res->content_type, "application/json");
    strncpy(res->body, json ? json : "[]", sizeof(res->body) - 1);
    res->body[sizeof(res->body) - 1] = '\0';

    if (json) free(json);
  } else if (strstr(req->path, "/api/alerts")) {
    res->status_code = 200;
    strcpy(res->content_type, "application/json");
    char *json = alert_service_serialize_alerts();
    strncpy(res->body, json, sizeof(res->body) - 1);
    res->body[sizeof(res->body) - 1] = '\0';
    free(json);
  }
}
