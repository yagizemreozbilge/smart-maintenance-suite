#include "http_server.h"
#include "api_handlers.h"
#include "jwt.h"
#include "logger.h"
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
  #include <arpa/inet.h>
  #include <pthread.h>
  #define SOCKET int
  #define INVALID_SOCKET -1
  #define SOCKET_ERROR -1
  #define closesocket close
#endif

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

#ifdef _WIN32
DWORD WINAPI handle_request(LPVOID client_ptr) {
#else
void *handle_request(void *client_ptr) {
#endif
  SOCKET client_socket = (SOCKET)(uintptr_t)client_ptr;
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
    } else if (strstr(buffer, "GET /api/login")) {
      // Basitlik için username/password field'larını query string'den okuyoruz
      // Gerçekten güvenli olması için POST olmalı, ancak student-style şimdilik bu şekilde ilerliyoruz.
      char username[50] = {0}, password[50] = {0}, role[20] = {0};
      // Örnek: GET /api/login?u=admin&p=admin123
      char *u_ptr = strstr(buffer, "u=");
      char *p_ptr = strstr(buffer, "p=");

      if (u_ptr && p_ptr) {
        sscanf(u_ptr, "u=%[^& \r\n]", username);
        sscanf(p_ptr, "p=%[^& \r\n]", password);
        int uid = verify_user_credentials(username, password, role);

        if (uid != -1) {
          char *token = generate_auth_token(uid, username, role);
          char response[256];
          snprintf(response, sizeof(response),
                   "{\"success\": true, \"token\": \"%s\", \"role\": \"%s\", \"username\": \"%s\"}",
                   token, role, username);
          send_response(client_socket, "application/json", response, 200);
          free(token);
        } else {
          send_response(client_socket, "application/json", "{\"success\": false, \"message\": \"Hatali giris\"}", 401);
        }
      } else {
        send_response(client_socket, "application/json", "{\"error\": \"Eksik parametre\"}", 400);
      }
    } else {
      const char *not_found = "{\"error\": \"Not Found\"}";
      send_response(client_socket, "application/json", not_found, 404);
    }
  }

  closesocket(client_socket);
  return 0;
}

#ifdef _WIN32
DWORD WINAPI server_thread_func(LPVOID param) {
#else
void *server_thread_func(void *param) {
#endif
  (void)param;

  while (running) {
    struct sockaddr_in client;
    socklen_t c = sizeof(struct sockaddr_in);
    SOCKET client_socket = accept(server_fd, (struct sockaddr *)&client, &c);

    if (client_socket != INVALID_SOCKET) {
      if (running) {
#ifdef _WIN32
        CreateThread(NULL, 0, handle_request, (LPVOID)(uintptr_t)client_socket, 0, NULL);
#else
        pthread_t thread;
        pthread_create(&thread, NULL, handle_request, (void *)(uintptr_t)client_socket);
        pthread_detach(thread);
#endif
      } else {
        closesocket(client_socket);
      }
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

  if (server_fd == INVALID_SOCKET) {
#ifdef _WIN32
    WSACleanup();
#endif
    return false;
  }

  struct sockaddr_in server;

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(port);
  int opt = 1;
#ifdef _WIN32
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt));
#else
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif

  if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
    LOG_ERROR("Bind failed on port %d.", port);
    closesocket(server_fd);
#ifdef _WIN32
    WSACleanup();
#endif
    return false;
  }

  listen(server_fd, 5);
  running = true;
#ifdef _WIN32
  HANDLE thread = CreateThread(NULL, 0, server_thread_func, NULL, 0, NULL);

  if (thread == NULL) {
    LOG_ERROR("Failed to create server thread.");
    running = false;
    closesocket(server_fd);
    WSACleanup();
    return false;
  }

  CloseHandle(thread);
#else
  pthread_t thread;

  if (pthread_create(&thread, NULL, server_thread_func, NULL) != 0) {
    LOG_ERROR("Failed to create server thread.");
    running = false;
    closesocket(server_fd);
    return false;
  }

  pthread_detach(thread);
#endif
  LOG_INFO("Native HTTP API Server running on port %d", port);
  return true;
}

void stop_http_server() {
  running = false;

  if (server_fd != INVALID_SOCKET) {
    closesocket(server_fd);
  }

#ifdef _WIN32
  WSACleanup();
#endif
}
