#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <stdbool.h>

/**
 * Starts the HTTP server on the specified port.
 * @param port The port to listen on (e.g., 8080).
 * @return true if started successfully, false otherwise.
 */
bool start_http_server(int port);

/**
 * Stops the running HTTP server.
 */
void stop_http_server();

#endif
