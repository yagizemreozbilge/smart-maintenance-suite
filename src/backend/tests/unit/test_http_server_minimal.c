#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Deactivite Winsock
#define _WINSOCKAPI_
#define WIN32_LEAN_AND_MEAN

#include "../api/http_server.h"
#include "../api/router.h"

void LOG_INFO(const char *format, ...) {}

void LOG_ERROR(const char *format, ...) {}

void handle_route(HttpRequest *req, HttpResponse *res) {
  res->status_code = 200;
  strcpy(res->content_type, "application/json");
  strcpy(res->body, "{}");
}

static void test_server_exists(void **state) {
  // Just test that the file compiles and links
  assert_true(1);
  printf("[PASS] http_server minimal test\n");
}

int main(void) {
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_server_exists),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
