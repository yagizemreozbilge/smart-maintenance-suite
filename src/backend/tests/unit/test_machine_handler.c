/* test_machine_handler_suite.c */
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include "api/handlers/machine_handler.h"

// Mock logs to avoid linking errors
void LOG_INFO(const char *f, ...) {
  (void)f;
}

void LOG_ERROR(const char *f, ...) {
  (void)f;
}

static void test_get_machines_success(void **state) {
  (void)state;
  HttpRequest req = { .method = "GET" };
  HttpResponse res = { 0 };
  handle_machine_request(&req, &res);
  assert_int_equal(res.status_code, 200);
  assert_non_null(strstr(res.body, "CNC Machine"));
}

static void test_post_machines_success(void **state) {
  (void)state;
  HttpRequest req = { .method = "POST" };
  HttpResponse res = { 0 };
  handle_machine_request(&req, &res);
  assert_int_equal(res.status_code, 201);
}

static void test_unsupported_method_branch(void **state) {
  (void)state;
  HttpRequest req = { .method = "DELETE" }; // Triggers the final implicit 'else'
  HttpResponse res = { .status_code = -1 };
  handle_machine_request(&req, &res);
  // Since code has no 'else' for other methods, status remains unchanged
  assert_int_equal(res.status_code, -1);
}

int main(void) {
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_get_machines_success),
    cmocka_unit_test(test_post_machines_success),
    cmocka_unit_test(test_unsupported_method_branch),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
