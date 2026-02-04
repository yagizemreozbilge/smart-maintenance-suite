#include "http_server.h"
#include "api_handlers.h"
#include "logger.h"
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma comment(lib, "ws2_32.lib")

static SOCKET server_fd = INVALID_SOCKET;
static bool running = false;

// HTTP Response Helper
void send_response(SOCKET client_socket, const char *content_type, const char *body, int status_code) {
  char header[512];
  const char *status_text = (status_code == 200) ? "OK" : "Not Found";
  sprintf(header,
          "HTTP/1.1 %d %s\r\n"
          "Content-Type: %s\r\n"
          "Content-Length: %zu\r\n"
          "Access-Control-Allow-Origin: *\r\n"
          "Connection: close\r\n\r\n",
          status_code, status_text, content_type, strlen(body));
  send(client_socket, header, (int)strlen(header), 0);
  send(client_socket, body, (int)strlen(body), 0);
}

// Thread function for each request
DWORD WINAPI handle_request(LPVOID client_ptr) {
  SOCKET client_socket = (SOCKET)client_ptr;
  char buffer[2048] = {0};
  int bytes = recv(client_socket, buffer, sizeof(buffer) - 1, 0);

  if (bytes > 0) {
    buffer[bytes] = '\0';

    // Simple Routing
    if (strstr(buffer, "GET /api/machines")) {
      char *json = serialize_machines_to_json();
      send_response(client_socket, "application/json", json, 200);
      free(json);
    } else if (strstr(buffer, "GET /api/inventory")) {
      char *json = serialize_inventory_to_json();
      send_response(client_socket, "application/json", json, 200);
      free(json);
    } else if (strstr(buffer, "GET /api/alerts")) {
      char *json = serialize_alerts_to_json();
      send_response(client_socket, "application/json", json, 200);
      free(json);
    } else if (strstr(buffer, "GET /api/sensors")) {
      char *json = serialize_sensors_to_json();
      send_response(client_socket, "application/json", json, 200);
      free(json);
    } else {
      const char *not_found = "{\"error\": \"Not Found\"}";
      send_response(client_socket, "application/json", not_found, 404);
    }
  }

  closesocket(client_socket);
  return 0;
}

// Main server loop thread
DWORD WINAPI server_thread_func(LPVOID param) {
  (void)param;

  while (running) {
    struct sockaddr_in client;
    int c = sizeof(struct sockaddr_in);
    SOCKET client_socket = accept(server_fd, (struct sockaddr *)&client, &c);

    if (client_socket != INVALID_SOCKET) {
      if (running) {
        CreateThread(NULL, 0, handle_request, (LPVOID)client_socket, 0, NULL);
      } else {
        closesocket(client_socket);
      }
    }
  }

  return 0;
}

bool start_http_server(int port) {
  WSADATA wsa;

  if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return false;

  server_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (server_fd == INVALID_SOCKET) {
    WSACleanup();
    return false;
  }

  struct sockaddr_in server;

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(port);
  // Set socket option so we can reuse the port immediately
  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt));

  if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
    LOG_ERROR("Bind failed on port %d. Error code: %d", port, WSAGetLastError());
    closesocket(server_fd);
    WSACleanup();
    return false;
  }

  listen(server_fd, 5);
  running = true;
  // Start the background server thread correctly for C
  HANDLE thread = CreateThread(NULL, 0, server_thread_func, NULL, 0, NULL);

  if (thread == NULL) {
    LOG_ERROR("Failed to create server thread.");
    running = false;
    closesocket(server_fd);
    WSACleanup();
    return false;
  }

  CloseHandle(thread); // We don't need the handle
  LOG_INFO("Native HTTP API Server running on port %d", port);
  return true;
}

void stop_http_server() {
  running = false;

  if (server_fd != INVALID_SOCKET) {
    closesocket(server_fd);
  }

  WSACleanup();
}
