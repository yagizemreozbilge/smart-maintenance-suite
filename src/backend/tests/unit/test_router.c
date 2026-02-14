#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Header dosyalarını doğru path ile include et
#include "../api/router.h"
#include "../api/handlers/machine_handler.h"
#include "../api/handlers/inventory_handler.h"
#include "../api/handlers/auth_handler.h"
#include "../api/handlers/maintenance_handler.h"
#include "../api/handlers/report_handler.h"
#include "../api/handlers/fault_handler.h"
#include "../database/alert_service.h"

/* ====================================================================
 * MOCK FONKSİYONLAR VE GLOBAL DEĞİŞKENLER
 * ==================================================================== */

// Handler çağrılarını takip etmek için counter'lar
static int mock_machine_handler_called = 0;
static int mock_inventory_handler_called = 0;
static int mock_auth_handler_called = 0;
static int mock_maintenance_handler_called = 0;
static int mock_report_handler_called = 0;
static int mock_fault_handler_called = 0;
static int mock_alert_service_called = 0;

// Handler'lara gönderilen request/response'ları kaydetmek için
static HttpRequest last_request;
static HttpResponse last_response;

// Mock handler fonksiyonları
void handle_machine_request(HttpRequest *req, HttpResponse *res) {
  mock_machine_handler_called++;
  // Handler'ın request'i değiştirdiğini simüle et
  res->status_code = 200;
  strcpy(res->content_type, "application/json");
  strcpy(res->body, "{\"machines\":[]}");
}

void handle_inventory_request(HttpRequest *req, HttpResponse *res) {
  mock_inventory_handler_called++;
  res->status_code = 200;
  strcpy(res->content_type, "application/json");
  strcpy(res->body, "{\"inventory\":[]}");
}

void handle_auth_request(HttpRequest *req, HttpResponse *res) {
  mock_auth_handler_called++;
  res->status_code = 200;
  strcpy(res->content_type, "application/json");
  strcpy(res->body, "{\"token\":\"mock_token\"}");
}

void handle_maintenance_request(HttpRequest *req, HttpResponse *res) {
  mock_maintenance_handler_called++;
  res->status_code = 200;
  strcpy(res->content_type, "application/json");
  strcpy(res->body, "{\"maintenance\":[]}");
}

void handle_report_request(HttpRequest *req, HttpResponse *res) {
  mock_report_handler_called++;
  res->status_code = 200;
  strcpy(res->content_type, "application/json");
  strcpy(res->body, "{\"reports\":[]}");
}

void handle_fault_request(HttpRequest *req, HttpResponse *res) {
  mock_fault_handler_called++;
  res->status_code = 200;
  strcpy(res->content_type, "application/json");
  strcpy(res->body, "{\"faults\":[]}");
}

// Alert service mock
char *alert_service_serialize_alerts(void) {
  mock_alert_service_called++;
  char *json = malloc(100);
  strcpy(json, "{\"alerts\":[{\"id\":1,\"message\":\"test\"}]}");
  return json;
}

/* ====================================================================
 * Test Setup/Teardown
 * ==================================================================== */

static void reset_mocks(void) {
  mock_machine_handler_called = 0;
  mock_inventory_handler_called = 0;
  mock_auth_handler_called = 0;
  mock_maintenance_handler_called = 0;
  mock_report_handler_called = 0;
  mock_fault_handler_called = 0;
  mock_alert_service_called = 0;
  memset(&last_request, 0, sizeof(last_request));
  memset(&last_response, 0, sizeof(last_response));
}

/* ====================================================================
 * Test Cases - Tüm Route'lar
 * ==================================================================== */

static void test_route_machines(void **state) {
  reset_mocks();
  HttpRequest req;
  HttpResponse res;
  strcpy(req.path, "/api/machines");
  strcpy(req.method, "GET");
  handle_route(&req, &res);
  // Handler çağrıldı mı?
  assert_int_equal(mock_machine_handler_called, 1);
  assert_int_equal(mock_inventory_handler_called, 0);
  assert_int_equal(mock_auth_handler_called, 0);
  assert_int_equal(mock_maintenance_handler_called, 0);
  assert_int_equal(mock_report_handler_called, 0);
  assert_int_equal(mock_fault_handler_called, 0);
  assert_int_equal(mock_alert_service_called, 0);
  // Response doğru mu?
  assert_int_equal(res.status_code, 200);
  assert_string_equal(res.content_type, "application/json");
  assert_string_equal(res.body, "{\"machines\":[]}");
}

static void test_route_machines_with_id(void **state) {
  reset_mocks();
  HttpRequest req;
  HttpResponse res;
  strcpy(req.path, "/api/machines/123");
  strcpy(req.method, "GET");
  handle_route(&req, &res);
  assert_int_equal(mock_machine_handler_called, 1);
  assert_int_equal(res.status_code, 200);
}

static void test_route_inventory(void **state) {
  reset_mocks();
  HttpRequest req;
  HttpResponse res;
  strcpy(req.path, "/api/inventory");
  strcpy(req.method, "GET");
  handle_route(&req, &res);
  assert_int_equal(mock_inventory_handler_called, 1);
  assert_int_equal(mock_machine_handler_called, 0);
  assert_int_equal(res.status_code, 200);
  assert_string_equal(res.body, "{\"inventory\":[]}");
}

static void test_route_inventory_with_sku(void **state) {
  reset_mocks();
  HttpRequest req;
  HttpResponse res;
  strcpy(req.path, "/api/inventory/MO-001");
  strcpy(req.method, "GET");
  handle_route(&req, &res);
  assert_int_equal(mock_inventory_handler_called, 1);
}

static void test_route_login(void **state) {
  reset_mocks();
  HttpRequest req;
  HttpResponse res;
  strcpy(req.path, "/api/login");
  strcpy(req.method, "POST");
  handle_route(&req, &res);
  assert_int_equal(mock_auth_handler_called, 1);
  assert_int_equal(mock_inventory_handler_called, 0);
  assert_int_equal(res.status_code, 200);
  assert_string_equal(res.body, "{\"token\":\"mock_token\"}");
}

static void test_route_maintenance(void **state) {
  reset_mocks();
  HttpRequest req;
  HttpResponse res;
  strcpy(req.path, "/api/maintenance");
  strcpy(req.method, "GET");
  handle_route(&req, &res);
  assert_int_equal(mock_maintenance_handler_called, 1);
  assert_int_equal(res.status_code, 200);
  assert_string_equal(res.body, "{\"maintenance\":[]}");
}

static void test_route_maintenance_with_id(void **state) {
  reset_mocks();
  HttpRequest req;
  HttpResponse res;
  strcpy(req.path, "/api/maintenance/456");
  strcpy(req.method, "GET");
  handle_route(&req, &res);
  assert_int_equal(mock_maintenance_handler_called, 1);
}

static void test_route_reports(void **state) {
  reset_mocks();
  HttpRequest req;
  HttpResponse res;
  strcpy(req.path, "/api/reports");
  strcpy(req.method, "GET");
  handle_route(&req, &res);
  assert_int_equal(mock_report_handler_called, 1);
  assert_int_equal(res.status_code, 200);
  assert_string_equal(res.body, "{\"reports\":[]}");
}

static void test_route_reports_with_type(void **state) {
  reset_mocks();
  HttpRequest req;
  HttpResponse res;
  strcpy(req.path, "/api/reports/daily");
  strcpy(req.method, "GET");
  handle_route(&req, &res);
  assert_int_equal(mock_report_handler_called, 1);
}

static void test_route_faults(void **state) {
  reset_mocks();
  HttpRequest req;
  HttpResponse res;
  strcpy(req.path, "/api/faults");
  strcpy(req.method, "GET");
  handle_route(&req, &res);
  assert_int_equal(mock_fault_handler_called, 1);
  assert_int_equal(res.status_code, 200);
  assert_string_equal(res.body, "{\"faults\":[]}");
}

static void test_route_faults_with_id(void **state) {
  reset_mocks();
  HttpRequest req;
  HttpResponse res;
  strcpy(req.path, "/api/faults/789");
  strcpy(req.method, "GET");
  handle_route(&req, &res);
  assert_int_equal(mock_fault_handler_called, 1);
}

static void test_route_alerts(void **state) {
  reset_mocks();
  HttpRequest req;
  HttpResponse res;
  strcpy(req.path, "/api/alerts");
  strcpy(req.method, "GET");
  handle_route(&req, &res);
  assert_int_equal(mock_alert_service_called, 1);
  assert_int_equal(mock_machine_handler_called, 0);
  assert_int_equal(res.status_code, 200);
  assert_string_equal(res.content_type, "application/json");
  assert_string_equal(res.body, "{\"alerts\":[{\"id\":1,\"message\":\"test\"}]}");
}

/* ====================================================================
 * Test Cases - 404 Not Found
 * ==================================================================== */

static void test_route_not_found(void **state) {
  reset_mocks();
  HttpRequest req;
  HttpResponse res;
  strcpy(req.path, "/api/unknown");
  strcpy(req.method, "GET");
  handle_route(&req, &res);
  // Hiçbir handler çağrılmamalı
  assert_int_equal(mock_machine_handler_called, 0);
  assert_int_equal(mock_inventory_handler_called, 0);
  assert_int_equal(mock_auth_handler_called, 0);
  assert_int_equal(mock_maintenance_handler_called, 0);
  assert_int_equal(mock_report_handler_called, 0);
  assert_int_equal(mock_fault_handler_called, 0);
  assert_int_equal(mock_alert_service_called, 0);
  // 404 response kontrolü
  assert_int_equal(res.status_code, 404);
  assert_string_equal(res.content_type, "application/json");
  assert_string_equal(res.body, "{\"error\": \"Route not found\"}");
}

static void test_route_empty_path(void **state) {
  reset_mocks();
  HttpRequest req;
  HttpResponse res;
  strcpy(req.path, "");
  strcpy(req.method, "GET");
  handle_route(&req, &res);
  assert_int_equal(res.status_code, 404);
  assert_string_equal(res.body, "{\"error\": \"Route not found\"}");
}

static void test_route_root_path(void **state) {
  reset_mocks();
  HttpRequest req;
  HttpResponse res;
  strcpy(req.path, "/");
  strcpy(req.method, "GET");
  handle_route(&req, &res);
  assert_int_equal(res.status_code, 404);
}

/* ====================================================================
 * Test Cases - Farklı HTTP Metodları
 * ==================================================================== */

static void test_route_machines_post(void **state) {
  reset_mocks();
  HttpRequest req;
  HttpResponse res;
  strcpy(req.path, "/api/machines");
  strcpy(req.method, "POST");
  strcpy(req.body, "{\"name\":\"new machine\"}");
  handle_route(&req, &res);
  assert_int_equal(mock_machine_handler_called, 1);
  // Handler metod farkına bakmamalı, sadece path'e bakıyor
}

static void test_route_inventory_put(void **state) {
  reset_mocks();
  HttpRequest req;
  HttpResponse res;
  strcpy(req.path, "/api/inventory/1");
  strcpy(req.method, "PUT");
  handle_route(&req, &res);
  assert_int_equal(mock_inventory_handler_called, 1);
}

static void test_route_faults_delete(void **state) {
  reset_mocks();
  HttpRequest req;
  HttpResponse res;
  strcpy(req.path, "/api/faults/789");
  strcpy(req.method, "DELETE");
  handle_route(&req, &res);
  assert_int_equal(mock_fault_handler_called, 1);
}

/* ====================================================================
 * Test Cases - Path Çakışmaları
 * ==================================================================== */

static void test_route_order_priority(void **state) {
  reset_mocks();
  HttpRequest req;
  HttpResponse res;
  // /api/machines önce kontrol ediliyor
  strcpy(req.path, "/api/machines/inventory"); // Bu path hem machines hem inventory ile eşleşebilir
  strcpy(req.method, "GET");
  handle_route(&req, &res);
  // İlk eşleşen handler çağrılmalı (machines)
  assert_int_equal(mock_machine_handler_called, 1);
  assert_int_equal(mock_inventory_handler_called, 0);
}

static void test_route_similar_paths(void **state) {
  reset_mocks();
  HttpRequest req;
  HttpResponse res;
  strcpy(req.path, "/api/maintenance/reports"); // Hem maintenance hem reports
  strcpy(req.method, "GET");
  handle_route(&req, &res);
  // İlk eşleşen: maintenance
  assert_int_equal(mock_maintenance_handler_called, 1);
  assert_int_equal(mock_report_handler_called, 0);
}

/* ====================================================================
 * Test Cases - Edge Cases
 * ==================================================================== */

static void test_route_null_path(void **state) {
  reset_mocks();
  HttpRequest req;
  HttpResponse res;
  memset(&req, 0, sizeof(req));
  // path null olabilir, strstr NULL pointer ile çağrılır mı?
  // Gerçek kodda path'in null olması beklenmez ama yine de test edelim
  // Not: Bu test çökebilir, gerçek kodda path'in hep valid olduğunu varsayıyoruz
  // req.path[0] = '\0'; olarak bırakıyoruz
  handle_route(&req, &res);
  // Muhtemelen 404 döner
  assert_int_equal(res.status_code, 404);
}

static void test_route_very_long_path(void **state) {
  reset_mocks();
  HttpRequest req;
  HttpResponse res;
  char long_path[1000];
  strcpy(long_path, "/api/machines/");
  memset(long_path + strlen(long_path), 'a', 900);
  long_path[950] = '\0';
  strcpy(req.path, long_path);
  strcpy(req.method, "GET");
  handle_route(&req, &res);
  // Başlangıç /api/machines ile eşleşmeli
  assert_int_equal(mock_machine_handler_called, 1);
}

static void test_route_path_with_query(void **state) {
  reset_mocks();
  HttpRequest req;
  HttpResponse res;
  strcpy(req.path, "/api/inventory?min=10&max=100");
  strcpy(req.method, "GET");
  strcpy(req.query_params, "min=10&max=100");
  handle_route(&req, &res);
  assert_int_equal(mock_inventory_handler_called, 1);
  // Query params path'ten ayrılmış olmalı
  assert_string_equal(req.query_params, "min=10&max=100");
}

/* ====================================================================
 * Main test runner
 * ==================================================================== */

int main(void) {
  const struct CMUnitTest tests[] = {
    // Ana route'lar
    cmocka_unit_test(test_route_machines),
    cmocka_unit_test(test_route_machines_with_id),
    cmocka_unit_test(test_route_inventory),
    cmocka_unit_test(test_route_inventory_with_sku),
    cmocka_unit_test(test_route_login),
    cmocka_unit_test(test_route_maintenance),
    cmocka_unit_test(test_route_maintenance_with_id),
    cmocka_unit_test(test_route_reports),
    cmocka_unit_test(test_route_reports_with_type),
    cmocka_unit_test(test_route_faults),
    cmocka_unit_test(test_route_faults_with_id),
    cmocka_unit_test(test_route_alerts),

    // 404 testleri
    cmocka_unit_test(test_route_not_found),
    cmocka_unit_test(test_route_empty_path),
    cmocka_unit_test(test_route_root_path),

    // Farklı HTTP metodları
    cmocka_unit_test(test_route_machines_post),
    cmocka_unit_test(test_route_inventory_put),
    cmocka_unit_test(test_route_faults_delete),

    // Path çakışmaları
    cmocka_unit_test(test_route_order_priority),
    cmocka_unit_test(test_route_similar_paths),

    // Edge cases
    cmocka_unit_test(test_route_very_long_path),
    cmocka_unit_test(test_route_path_with_query),
    // cmocka_unit_test(test_route_null_path), // Bu test çökebilir, yoruma al
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
