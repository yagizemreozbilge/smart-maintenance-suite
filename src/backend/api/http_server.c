#include "http_server.h"
#include "router.h"
#include "../database/logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifdef _WIN32
  #include <winsock2.h>
  #pragma comment(lib, "ws2_32.lib")
  typedef int socklen_t;
#else
  #include <unistd.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <pthread.h>
  #define closesocket close
  #define INVALID_SOCKET -1
  #define SOCKET_ERROR -1
#endif

static SOCKET server_fd = INVALID_SOCKET;
static bool running = false;

static void parse_request(const char *buffer, HttpRequest *req) {
  memset(req, 0, sizeof(HttpRequest));
  sscanf(buffer, "%s %s", req->method, req->path);
  char *query = strchr(req->path, '?');

  if (query) {
    strncpy(req->query_params, query + 1, 511);
    *query = '\0';
  }

  char *body_ptr = strstr(buffer, "\r\n\r\n");

  if (body_ptr) {
    strncpy(req->body, body_ptr + 4, 4095);
  }
}

static void send_modular_response(SOCKET client, HttpResponse *res) {
  char header[1024];
  const char *status_text = (res->status_code == 200) ? "OK" : (res->status_code == 201 ? "Created" : (res->status_code == 404 ? "Not Found" : "Error"));
  sprintf(header,
          "HTTP/1.1 %d %s\r\n"
          "Content-Type: %s\r\n"
          "Content-Length: %zu\r\n"
          "Access-Control-Allow-Origin: *\r\n"
          "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
          "Connection: close\r\n\r\n",
          res->status_code, status_text, res->content_type, strlen(res->body));
  send(client, header, (int)strlen(header), 0);
  send(client, res->body, (int)strlen(res->body), 0);
}

#ifdef _WIN32
DWORD WINAPI modular_handle_client(LPVOID client_ptr) {
#else
void *modular_handle_client(void *client_ptr) {
#endif
  SOCKET client_socket = (SOCKET)(uintptr_t)client_ptr;
  char buffer[8192] = {0};
  int bytes = recv(client_socket, buffer, sizeof(buffer) - 1, 0);

  if (bytes > 0) {
    HttpRequest req;
    HttpResponse res;
    memset(&res, 0, sizeof(HttpResponse));
    parse_request(buffer, &req);

    // Handle OPTIONS for CORS
    if (strcmp(req.method, "OPTIONS") == 0) {
      res.status_code = 200;
      strcpy(res.content_type, "text/plain");
      strcpy(res.body, "");
    } else {
      handle_route(&req, &res);
    }

    send_modular_response(client_socket, &res);
  }

  closesocket(client_socket);
  return 0;
}

#ifdef _WIN32
DWORD WINAPI modular_server_thread(LPVOID param) {
#else
void *modular_server_thread(void *param) {
#endif

  while (running) {
    struct sockaddr_in client;
    socklen_t c = sizeof(struct sockaddr_in);
    SOCKET client_socket = accept(server_fd, (struct sockaddr *)&client, &c);

    if (client_socket != INVALID_SOCKET && running) {
#ifdef _WIN32
      CreateThread(NULL, 0, modular_handle_client, (LPVOID)(uintptr_t)client_socket, 0, NULL);
#else
      pthread_t thread;
      pthread_create(&thread, NULL, modular_handle_client, (void *)(uintptr_t)client_socket);
      pthread_detach(thread);
#endif
    } else if (client_socket != INVALID_SOCKET) {
      closesocket(client_socket);
    }
  }

  return 0;
}

bool start_http_server(int port) {
#ifdef _WIN32
  WSADATA wsa;

  if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return false;

#endif
  server_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (server_fd == INVALID_SOCKET) return false;

  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(port);
  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt));

  if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
    closesocket(server_fd);
    return false;
  }

  listen(server_fd, 10);
  running = true;
#ifdef _WIN32
  CloseHandle(CreateThread(NULL, 0, modular_server_thread, NULL, 0, NULL));
#else
  pthread_t thread;
  pthread_create(&thread, NULL, modular_server_thread, NULL);
  pthread_detach(thread);
#endif
  LOG_INFO("Modular HTTP Server running on port %d", port);
  return true;
}

void stop_http_server(void) {
  running = false;

  if (server_fd != INVALID_SOCKET) closesocket(server_fd);

#ifdef _WIN32
  WSACleanup();
#endif
}
