#include "../../api/handlers/report_handler.h"
#include "../../api/http_server.h"

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void test_generate_maintenance_xml() {
  char *xml = generate_maintenance_xml_report();
  assert(xml != NULL);
  assert(strstr(xml, "MaintenanceReport") != NULL);
  free(xml);
  printf("[PASS] generate_maintenance_xml_report\n");
}

void test_generate_inventory_xml() {
  char *xml = generate_inventory_xml_report();
  assert(xml != NULL);
  assert(strstr(xml, "InventoryReport") != NULL);
  free(xml);
  printf("[PASS] generate_inventory_xml_report\n");
}

void test_handle_report_maintenance() {
  HttpRequest req = {0};
  HttpResponse res = {0};
  strcpy(req.path, "/api/report/maintenance");
  handle_report_request(&req, &res);
  assert(res.status_code == 200);
  assert(strcmp(res.content_type, "application/xml") == 0);
  assert(strstr(res.body, "MaintenanceReport") != NULL);
  printf("[PASS] handle_report_request maintenance\n");
}

void test_handle_report_inventory() {
  HttpRequest req = {0};
  HttpResponse res = {0};
  strcpy(req.path, "/api/report/inventory");
  handle_report_request(&req, &res);
  assert(res.status_code == 200);
  assert(strcmp(res.content_type, "application/xml") == 0);
  assert(strstr(res.body, "InventoryReport") != NULL);
  printf("[PASS] handle_report_request inventory\n");
}

int main() {
  test_generate_maintenance_xml();
  test_generate_inventory_xml();
  test_handle_report_maintenance();
  test_handle_report_inventory();
  return 0;
}
