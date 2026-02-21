#include "report_handler.h"
#include "../../database/report_service.h"
#include "../../database/cJSON.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Original mock functions removed. Using real service implementations from database/report_service.c

void handle_report_request(HttpRequest *req, HttpResponse *res) {
  char *data;

  if (strstr(req->path, "csv")) {
    // Return CSV
    if (strstr(req->path, "maintenance")) {
      data = generate_maintenance_csv_report();
    } else {
      data = generate_inventory_csv_report();
    }

    res->status_code = 200;
    strcpy(res->content_type, "text/csv");
  } else {
    // Return XML
    if (strstr(req->path, "maintenance")) {
      data = generate_maintenance_xml_report();
    } else {
      data = generate_inventory_xml_report();
    }

    res->status_code = 200;
    strcpy(res->content_type, "application/xml");
  }

  strncpy(res->body, data, 8191);
  res->body[8191] = '\0';
  free(data);
}
