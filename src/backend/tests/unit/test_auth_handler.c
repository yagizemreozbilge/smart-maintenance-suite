#include "../../api/handlers/auth_handler.h"
#include "../../api/http_server.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

void test_auth_handler() {
  HttpRequest req = {0};
  HttpResponse res = {0};
  handle_auth_request(&req, &res);
  assert(res.status_code == 200);
  assert(strcmp(res.content_type, "application/json") == 0);
  assert(strstr(res.body, "authenticated") != NULL);
  assert(strstr(res.body, "admin") != NULL);
  printf("[PASS] Auth handler\n");
}

int main() {
  test_auth_handler();
  return 0;
}
