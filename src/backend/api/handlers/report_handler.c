#include "report_handler.h"
#include "../../database/report_service.h"
#include "../../database/cJSON.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Original mock functions removed. Using real service implementations from database/report_service.c

void handle_report_request(HttpRequest *req, HttpResponse *res) {
  char *xml;

  if (strstr(req->path, "maintenance")) {
    xml = generate_maintenance_xml_report();
  } else {
    xml = generate_inventory_xml_report();
  }

  res->status_code = 200;
  strcpy(res->content_type, "application/xml");
  strncpy(res->body, xml, 8191);
  res->body[8191] = '\0';
  free(xml);
}
