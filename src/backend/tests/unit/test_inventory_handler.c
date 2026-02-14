#include "../../api/handlers/inventory_handler.h"
#include "../../api/http_server.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

void test_inventory_get() {
  HttpRequest req = {0};
  HttpResponse res = {0};
  strcpy(req.method, "GET");
  handle_inventory_request(&req, &res);
  assert(res.status_code == 200);
  assert(strstr(res.body, "inventory") != NULL);
}

void test_inventory_invalid_method() {
  HttpRequest req = {0};
  HttpResponse res = {0};
  strcpy(req.method, "POST");
  handle_inventory_request(&req, &res);
  assert(res.status_code == 405);
}

int main() {
  test_inventory_get();
  test_inventory_invalid_method();
  printf("[PASS] Inventory handler\n");
  return 0;
}
