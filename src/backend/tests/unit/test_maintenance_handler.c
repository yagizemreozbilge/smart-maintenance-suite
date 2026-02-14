#include "../../api/handlers/maintenance_handler.h"
#include "../../api/http_server.h"

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void test_serialize_maintenance() {
  char *json = serialize_maintenance_to_json();
  assert(json != NULL);
  assert(strstr(json, "Routine oil change") != NULL);
  assert(strstr(json, "completed") != NULL);
  free(json);
  printf("[PASS] serialize_maintenance_to_json\n");
}

void test_handle_maintenance_get() {
  HttpRequest req = {0};
  HttpResponse res = {0};
  strcpy(req.method, "GET");
  handle_maintenance_request(&req, &res);
  assert(res.status_code == 200);
  assert(strcmp(res.content_type, "application/json") == 0);
  assert(strstr(res.body, "Routine oil change") != NULL);
  printf("[PASS] handle_maintenance_request GET\n");
}

void test_handle_maintenance_not_allowed() {
  HttpRequest req = {0};
  HttpResponse res = {0};
  strcpy(req.method, "POST");  // GET dışında bir şey
  handle_maintenance_request(&req, &res);
  assert(res.status_code == 405);
  assert(strstr(res.body, "Only GET supported") != NULL);
  printf("[PASS] handle_maintenance_request 405\n");
}

int main() {
  test_serialize_maintenance();
  test_handle_maintenance_get();
  test_handle_maintenance_not_allowed();
  return 0;
}
