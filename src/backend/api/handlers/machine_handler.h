#ifndef MACHINE_HANDLER_H
#define MACHINE_HANDLER_H

#include "../http_server.h"

void handle_machine_request(HttpRequest *req, HttpResponse *res);
char *serialize_machines_to_json(void);

#endif
