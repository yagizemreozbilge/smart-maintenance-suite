#ifndef ROUTER_H
#define ROUTER_H

#include "http_server.h"

/**
 * @brief Routes the incoming request to the appropriate handler
 * @param req The incoming request
 * @param res The response to be filled
 */
void handle_route(HttpRequest *req, HttpResponse *res);

#endif /* ROUTER_H */
