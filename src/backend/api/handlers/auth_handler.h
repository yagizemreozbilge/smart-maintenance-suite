#ifndef AUTH_HANDLER_H
#define AUTH_HANDLER_H

#include "../http_server.h"

void handle_auth_request(HttpRequest *req, HttpResponse *res);

#endif
