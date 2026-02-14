#ifndef REPORT_HANDLER_H
#define REPORT_HANDLER_H

#include "../http_server.h"

void handle_report_request(HttpRequest *req, HttpResponse *res);
char *generate_maintenance_xml_report(void);
char *generate_inventory_xml_report(void);

#endif
