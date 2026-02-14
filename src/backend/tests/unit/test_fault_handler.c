#include "../../api/handlers/fault_handler.h"
#include "../../api/http_server.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

void test_fault_get_all() {
  HttpRequest req = {0};
  HttpResponse res = {0};
  strcpy(req.method, "GET");
  strcpy(req.path, "/api/faults");
  handle_fault_request(&req, &res);
  assert(res.status_code == 200);
  assert(strstr(res.body, "faults") != NULL);
}

void test_fault_get_single() {
  HttpRequest req = {0};
  HttpResponse res = {0};
  strcpy(req.method, "GET");
  strcpy(req.path, "/api/faults/1");
  handle_fault_request(&req, &res);
  assert(res.status_code == 200);
  assert(strstr(res.body, "CNC Machine") != NULL);
}

void test_fault_post() {
  HttpRequest req = {0};
  HttpResponse res = {0};
  strcpy(req.method, "POST");
  handle_fault_request(&req, &res);
  assert(res.status_code == 201);
}

void test_fault_put_ack() {
  HttpRequest req = {0};
  HttpResponse res = {0};
  strcpy(req.method, "PUT");
  strcpy(req.path, "/api/faults/1/acknowledge");
  handle_fault_request(&req, &res);
  assert(res.status_code == 200);
  assert(strstr(res.body, "acknowledged") != NULL);
}

void test_fault_put_resolve() {
  HttpRequest req = {0};
  HttpResponse res = {0};
  strcpy(req.method, "PUT");
  strcpy(req.path, "/api/faults/1/resolve");
  handle_fault_request(&req, &res);
  assert(res.status_code == 200);
  assert(strstr(res.body, "resolved") != NULL);
}

void test_fault_invalid_method() {
  HttpRequest req = {0};
  HttpResponse res = {0};
  strcpy(req.method, "DELETE");
  handle_fault_request(&req, &res);
  assert(res.status_code == 405);
}

int main() {
  test_fault_get_all();
  test_fault_get_single();
  test_fault_post();
  test_fault_put_ack();
  test_fault_put_resolve();
  test_fault_invalid_method();
  printf("[PASS] Fault handler\n");
  return 0;
}
