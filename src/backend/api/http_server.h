#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <stdbool.h>

#ifdef _WIN32
  #include <winsock2.h>
#else
  #define SOCKET int
#endif

/**
 * @brief Simple request structure for modular handlers
 */
typedef struct {
  char method[10];
  char path[256];
  char query_params[512];
  char body[4096];
} HttpRequest;

/**
 * @brief Response structure
 */
typedef struct {
  int status_code;
  char content_type[64];
  char body[8192];
} HttpResponse;

/**
 * @brief Starts the HTTP server on the specified port.
 * @param port The port to listen on.
 * @return true if successful, false otherwise.
 */
bool start_http_server(int port);

/**
 * @brief Stops the running HTTP server.
 */
void stop_http_server(void);

#endif /* HTTP_SERVER_H */
