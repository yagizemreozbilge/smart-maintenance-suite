#ifndef MAINTENANCE_HANDLER_H
#define MAINTENANCE_HANDLER_H

#include "../http_server.h"

void handle_maintenance_request(HttpRequest *req, HttpResponse *res);

#endif
