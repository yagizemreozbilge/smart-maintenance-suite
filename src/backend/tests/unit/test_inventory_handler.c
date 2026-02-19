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

  if (strstr(res.body, "part_name") == NULL) {
    printf("[DEBUG] Inventory GET Failed. Body: %s\n", res.body);
  }

  assert(strstr(res.body, "part_name") != NULL);
  printf("[PASS] Inventory GET\n");
}

void test_inventory_post_success() {
  HttpRequest req = {0};
  HttpResponse res = {0};
  strcpy(req.method, "POST");
  strcpy(req.body, "{\"part_name\": \"Bearing\", \"sku\": \"BRG-001\", \"quantity\": 10, \"min_stock\": 5, \"cost\": 15.0}");
  handle_inventory_request(&req, &res);
  assert(res.status_code == 201);
  assert(strstr(res.body, "success") != NULL);
  printf("[PASS] Inventory POST Success\n");
}

void test_inventory_invalid_method() {
  HttpRequest req = {0};
  HttpResponse res = {0};
  strcpy(req.method, "DELETE"); // POST artık desteklendiği için DELETE kullanıyoruz
  handle_inventory_request(&req, &res);
  assert(res.status_code == 405);
  assert(strstr(res.body, "Method not supported") != NULL);
  printf("[PASS] Inventory 405\n");
}

int main() {
  test_inventory_get();
  test_inventory_post_success();
  test_inventory_invalid_method();
  printf("\n✅ [PASS] Inventory handler test suite completed\n");
  return 0;
}
