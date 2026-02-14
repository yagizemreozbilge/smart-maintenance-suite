#include "../../api/handlers/machine_handler.h"
#include "../../api/http_server.h"

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void test_serialize_machines() {
  char *json = serialize_machines_to_json();
  assert(json != NULL);
  assert(strstr(json, "CNC Machine") != NULL);
  assert(strstr(json, "running") != NULL);
  free(json);
  printf("[PASS] serialize_machines_to_json\n");
}

void test_handle_machine_get() {
  HttpRequest req = {0};
  HttpResponse res = {0};
  strcpy(req.method, "GET");
  handle_machine_request(&req, &res);
  assert(res.status_code == 200);
  assert(strcmp(res.content_type, "application/json") == 0);
  assert(strstr(res.body, "CNC Machine") != NULL);
  printf("[PASS] handle_machine_request GET\n");
}

void test_handle_machine_post() {
  HttpRequest req = {0};
  HttpResponse res = {0};
  strcpy(req.method, "POST");
  handle_machine_request(&req, &res);
  assert(res.status_code == 201);
  assert(strstr(res.body, "Machine onboarded") != NULL);
  printf("[PASS] handle_machine_request POST\n");
}

int main() {
  test_serialize_machines();
  test_handle_machine_get();
  test_handle_machine_post();
  return 0;
}
