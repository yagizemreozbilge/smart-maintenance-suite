#include "report_handler.h"
#include "../../database/cJSON.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char *generate_maintenance_xml_report(void) {
  char *xml = (char *)malloc(512);
  strcpy(xml, "<?xml version=\"1.0\"?><report><maintenance><log>Maintenance Report - All equipment running normally</log></maintenance></report>");
  return xml;
}

char *generate_inventory_xml_report(void) {
  char *xml = (char *)malloc(512);
  strcpy(xml, "<?xml version=\"1.0\"?><report><inventory><item>Inventory Report - Stock levels normal</item></inventory></report>");
  return xml;
}

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
