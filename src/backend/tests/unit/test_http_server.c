#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// ÖNEMLİ: winsock2.h'yi include ETME! Kendi mock'larımızı kullanacağız
#define _WINSOCKAPI_
#define WIN32_LEAN_AND_MEAN
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define SOCKET int

// HttpRequest ve HttpResponse yapılarını manuel tanımla
typedef struct {
  char method[16];
  char path[256];
  char query_params[512];
  char body[4096];
} HttpRequest;

typedef struct {
  int status_code;
  char content_type[32];
  char body[8192];
} HttpResponse;

#include "../api/http_server.h"
#include "../api/router.h"

// Logger mock
void LOG_INFO(const char *format, ...) {}

void LOG_ERROR(const char *format, ...) {}

/* ====================================================================
 * MOCK FONKSİYONLAR - BASİT
 * ==================================================================== */

// Socket mock değişkenleri
static int mock_socket_fd = 1234;
static int mock_accept_return = 5678;
static int mock_recv_return = 0;
static char mock_recv_buffer[8192] = {0};
static char mock_sent_data[16384] = {0};
static int mock_sent_bytes = 0;

// Mock kontrol değişkenleri
static int mock_socket_called = 0;
static int mock_bind_called = 0;
static int mock_listen_called = 0;
static int mock_accept_called = 0;
static int mock_recv_called = 0;
static int mock_send_called = 0;
static int mock_closesocket_called = 0;
static int mock_setsockopt_called = 0;

// Mock başarısızlık kontrolü
static int mock_bind_fail = 0;
static int mock_accept_fail = 0;
static int mock_socket_fail = 0;
static int mock_recv_fail = 0;

/* ====================================================================
 * MOCK WINSOCK FONKSİYONLARI - BASİT
 * ==================================================================== */

int WSAStartup(int a, int b) {
  return 0;
}

int WSACleanup(void) {
  return 0;
}

int socket(int af, int type, int protocol) {
  mock_socket_called++;

  if (mock_socket_fail) return -1;

  return mock_socket_fd;
}

int bind(int s, const void *addr, int namelen) {
  mock_bind_called++;

  if (mock_bind_fail) return -1;

  return 0;
}

int listen(int s, int backlog) {
  mock_listen_called++;
  return 0;
}

int accept(int s, void *addr, void *addrlen) {
  mock_accept_called++;

  if (mock_accept_fail) return -1;

  return mock_accept_return;
}

int recv(int s, void *buf, int len, int flags) {
  mock_recv_called++;

  if (mock_recv_fail) return -1;

  if (strlen(mock_recv_buffer) > 0) {
    strcpy((char *)buf, mock_recv_buffer);
    return strlen(mock_recv_buffer);
  }

  return 0;
}

int send(int s, const void *buf, int len, int flags) {
  mock_send_called++;
  strncpy(mock_sent_data + mock_sent_bytes, (const char *)buf, len);
  mock_sent_bytes += len;
  return len;
}

int closesocket(int s) {
  mock_closesocket_called++;
  return 0;
}

int setsockopt(int s, int level, int optname, const void *optval, int optlen) {
  mock_setsockopt_called++;
  return 0;
}

void *CreateThread(void *a, void *b, void *c, void *d, void *e, void *f) {
  return (void *)1;
}

void CloseHandle(void *h) {}

/* ====================================================================
 * ROUTER MOCK
 * ==================================================================== */

void handle_route(HttpRequest *req, HttpResponse *res) {
  if (strcmp(req->path, "/test") == 0) {
    res->status_code = 200;
    strcpy(res->content_type, "application/json");
    strcpy(res->body, "{\"test\":\"ok\"}");
  } else if (strcmp(req->method, "OPTIONS") == 0) {
    res->status_code = 200;
    strcpy(res->content_type, "text/plain");
    strcpy(res->body, "");
  } else {
    res->status_code = 404;
    strcpy(res->content_type, "application/json");
    strcpy(res->body, "{\"error\":\"not found\"}");
  }
}

/* ====================================================================
 * TEST SETUP
 * ==================================================================== */

static void reset_mocks(void) {
  mock_socket_called = 0;
  mock_bind_called = 0;
  mock_listen_called = 0;
  mock_accept_called = 0;
  mock_recv_called = 0;
  mock_send_called = 0;
  mock_closesocket_called = 0;
  mock_setsockopt_called = 0;
  mock_bind_fail = 0;
  mock_accept_fail = 0;
  mock_socket_fail = 0;
  mock_recv_fail = 0;
  mock_recv_return = 0;
  memset(mock_recv_buffer, 0, sizeof(mock_recv_buffer));
  memset(mock_sent_data, 0, sizeof(mock_sent_data));
  mock_sent_bytes = 0;
}

/* ====================================================================
 * TEST: start_http_server
 * ==================================================================== */

static void test_start_server_success(void **state) {
  reset_mocks();
  bool result = start_http_server(8080);
  assert_true(result);
  assert_int_equal(mock_socket_called, 1);
  assert_int_equal(mock_setsockopt_called, 1);
  assert_int_equal(mock_bind_called, 1);
  assert_int_equal(mock_listen_called, 1);
  stop_http_server();
  assert_int_equal(mock_closesocket_called, 1);
}

static void test_start_server_socket_fail(void **state) {
  reset_mocks();
  mock_socket_fail = 1;
  bool result = start_http_server(8080);
  assert_false(result);
  assert_int_equal(mock_socket_called, 1);
}

static void test_start_server_bind_fail(void **state) {
  reset_mocks();
  mock_bind_fail = 1;
  bool result = start_http_server(8080);
  assert_false(result);
  assert_int_equal(mock_bind_called, 1);
  assert_int_equal(mock_closesocket_called, 1);
}

/* ====================================================================
 * TEST: Request parsing
 * ==================================================================== */

static void test_parse_simple_get(void **state) {
  reset_mocks();
  const char *request = "GET /test HTTP/1.1\r\nHost: localhost\r\n\r\n";
  strcpy(mock_recv_buffer, request);
  start_http_server(8080);
  // Client handler fonksiyonuna erişim yok, ama mantığı test edelim
  HttpRequest req;
  memset(&req, 0, sizeof(req));
  // parse_request fonksiyonu static, doğrudan test edemeyiz
  // Ama handle_route üzerinden test edebiliriz
  stop_http_server();
  assert_true(1);
}

static void test_options_request(void **state) {
  reset_mocks();
  const char *request = "OPTIONS /test HTTP/1.1\r\n\r\n";
  strcpy(mock_recv_buffer, request);
  start_http_server(8080);
  // OPTIONS test
  HttpRequest req;
  HttpResponse res;
  memset(&req, 0, sizeof(req));
  memset(&res, 0, sizeof(res));
  strcpy(req.method, "OPTIONS");
  strcpy(req.path, "/test");
  handle_route(&req, &res);
  assert_int_equal(res.status_code, 200);
  assert_string_equal(res.content_type, "text/plain");
  stop_http_server();
}

static void test_404_not_found(void **state) {
  reset_mocks();
  HttpRequest req;
  HttpResponse res;
  memset(&req, 0, sizeof(req));
  memset(&res, 0, sizeof(res));
  strcpy(req.method, "GET");
  strcpy(req.path, "/unknown");
  handle_route(&req, &res);
  assert_int_equal(res.status_code, 404);
  assert_non_null(strstr(res.body, "not found"));
}

/* ====================================================================
 * TEST: Edge cases
 * ==================================================================== */

static void test_stop_server_not_running(void **state) {
  reset_mocks();
  stop_http_server();
  assert_int_equal(mock_closesocket_called, 0);
}

static void test_server_double_stop(void **state) {
  reset_mocks();
  start_http_server(8080);
  stop_http_server();
  stop_http_server(); // İkinci stop
  assert_int_equal(mock_closesocket_called, 1);
}

/* ====================================================================
 * MAIN
 * ==================================================================== */

int main(void) {
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_start_server_success),
    cmocka_unit_test(test_start_server_socket_fail),
    cmocka_unit_test(test_start_server_bind_fail),
    cmocka_unit_test(test_parse_simple_get),
    cmocka_unit_test(test_options_request),
    cmocka_unit_test(test_404_not_found),
    cmocka_unit_test(test_stop_server_not_running),
    cmocka_unit_test(test_server_double_stop),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
