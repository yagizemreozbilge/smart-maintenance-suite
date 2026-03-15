#include "report_handler.h"
#include "../../database/report_service.h"
#include "../../database/cJSON.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Original mock functions removed. Using real service implementations from database/report_service.c

void handle_report_request(HttpRequest *req, HttpResponse *res) {
  char *data = NULL;
  bool is_xml = strstr(req->path, "/xml") != NULL;

  if (is_xml) {
    // XML reports
    if (strstr(req->path, "maintenance")) {
      data = generate_maintenance_xml_report();
    } else {
      data = generate_inventory_xml_report();
    }

    res->status_code = 200;
    strcpy(res->content_type, "application/xml");
  } else {
    // CSV reports (Default)
    if (strstr(req->path, "maintenance")) {
      data = generate_maintenance_csv_report();
    } else {
      data = generate_inventory_csv_report();
    }

    res->status_code = 200;
    strcpy(res->content_type, "text/csv");
  }

  if (data) {
    strncpy(res->body, data, 8191);
    res->body[8191] = '\0';
    free(data);
  } else {
    res->status_code = 500;
    strcpy(res->body, "{\"error\": \"Failed to generate report\"}");
  }
}
