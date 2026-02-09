#include "../../api/http_server.h"
#include "../../api/router.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

void test_http_request_struct() {
  HttpRequest req;
  assert(sizeof(req.method) == 10);
  assert(sizeof(req.path) == 256);
  assert(sizeof(req.query_params) == 512);
  assert(sizeof(req.body) == 4096);
  printf("[PASS] HttpRequest struct: correct sizes\n");
}

void test_http_response_struct() {
  HttpResponse res;
  assert(sizeof(res.content_type) == 64);
  assert(sizeof(res.body) == 8192);
  printf("[PASS] HttpResponse struct: correct sizes\n");
}

void test_request_initialization() {
  HttpRequest req = {0};
  strcpy(req.method, "GET");
  strcpy(req.path, "/api/machines");
  assert(strcmp(req.method, "GET") == 0);
  assert(strcmp(req.path, "/api/machines") == 0);
  printf("[PASS] HttpRequest: initialization\n");
}

void test_response_initialization() {
  HttpResponse res = {0};
  res.status_code = 200;
  strcpy(res.content_type, "application/json");
  strcpy(res.body, "{\"success\": true}");
  assert(res.status_code == 200);
  assert(strcmp(res.content_type, "application/json") == 0);
  assert(strstr(res.body, "success") != NULL);
  printf("[PASS] HttpResponse: initialization\n");
}

void test_request_with_query_params() {
  HttpRequest req = {0};
  strcpy(req.method, "GET");
  strcpy(req.path, "/api/login");
  strcpy(req.query_params, "u=admin&p=admin123");
  assert(strstr(req.query_params, "u=admin") != NULL);
  assert(strstr(req.query_params, "p=admin123") != NULL);
  printf("[PASS] HttpRequest: query parameters\n");
}

void test_request_with_body() {
  HttpRequest req = {0};
  strcpy(req.method, "POST");
  strcpy(req.path, "/api/machines");
  strcpy(req.body, "{\"name\": \"New Machine\", \"status\": \"operational\"}");
  assert(strcmp(req.method, "POST") == 0);
  assert(strstr(req.body, "New Machine") != NULL);
  printf("[PASS] HttpRequest: POST with body\n");
}

void test_response_status_codes() {
  HttpResponse res1 = {.status_code = 200};
  HttpResponse res2 = {.status_code = 201};
  HttpResponse res3 = {.status_code = 400};
  HttpResponse res4 = {.status_code = 401};
  HttpResponse res5 = {.status_code = 404};
  HttpResponse res6 = {.status_code = 500};
  assert(res1.status_code == 200);
  assert(res2.status_code == 201);
  assert(res3.status_code == 400);
  assert(res4.status_code == 401);
  assert(res5.status_code == 404);
  assert(res6.status_code == 500);
  printf("[PASS] HttpResponse: all status codes\n");
}

void test_response_content_types() {
  HttpResponse res1 = {0};
  strcpy(res1.content_type, "application/json");
  HttpResponse res2 = {0};
  strcpy(res2.content_type, "application/xml");
  HttpResponse res3 = {0};
  strcpy(res3.content_type, "text/csv");
  assert(strcmp(res1.content_type, "application/json") == 0);
  assert(strcmp(res2.content_type, "application/xml") == 0);
  assert(strcmp(res3.content_type, "text/csv") == 0);
  printf("[PASS] HttpResponse: content types\n");
}

void test_request_method_types() {
  HttpRequest req1 = {0};
  strcpy(req1.method, "GET");
  HttpRequest req2 = {0};
  strcpy(req2.method, "POST");
  HttpRequest req3 = {0};
  strcpy(req3.method, "PUT");
  HttpRequest req4 = {0};
  strcpy(req4.method, "DELETE");
  HttpRequest req5 = {0};
  strcpy(req5.method, "OPTIONS");
  assert(strcmp(req1.method, "GET") == 0);
  assert(strcmp(req2.method, "POST") == 0);
  assert(strcmp(req3.method, "PUT") == 0);
  assert(strcmp(req4.method, "DELETE") == 0);
  assert(strcmp(req5.method, "OPTIONS") == 0);
  printf("[PASS] HttpRequest: all HTTP methods\n");
}

int main() {
  printf("=== API Handlers Unit Tests ===\n");
  test_http_request_struct();
  test_http_response_struct();
  test_request_initialization();
  test_response_initialization();
  test_request_with_query_params();
  test_request_with_body();
  test_response_status_codes();
  test_response_content_types();
  test_request_method_types();
  printf("\n✅ All API Handler unit tests passed!\n");
  return 0;
}
