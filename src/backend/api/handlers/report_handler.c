#include "report_handler.h"
#include "../../database/report_service.h"
#include <string.h>
#include <stdlib.h>

void handle_report_request(HttpRequest *req, HttpResponse *res) {
  if (strstr(req->path, "/maintenance/xml")) {
    char *data = generate_maintenance_xml_report();
    res->status_code = 200;
    strcpy(res->content_type, "application/xml");
    strncpy(res->body, data, 8191);
    free(data);
  } else if (strstr(req->path, "/inventory/xml")) {
    char *data = generate_inventory_xml_report();
    res->status_code = 200;
    strcpy(res->content_type, "application/xml");
    strncpy(res->body, data, 8191);
    free(data);
  } else {
    res->status_code = 404;
    strcpy(res->body, "{\"error\": \"Report type not found\"}");
  }
}
