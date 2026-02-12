#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "auth_handler.h"

/* ---------------- MOCK STRUCTS ---------------- */

typedef struct {
  int dummy;
} HttpRequest;

typedef struct {
  int status_code;
  char content_type[128];
  char body[8192];
} HttpResponse;

/* ---------------- MOCK generate_auth_token ---------------- */

char *generate_auth_token(int user_id, const char *username, const char *role) {
  assert_int_equal(user_id, 1);
  assert_string_equal(username, "admin");
  assert_string_equal(role, "admin");
  char *mock_token = malloc(32);
  strcpy(mock_token, "mocked_jwt_token");
  return mock_token;
}

/* ---------------- TEST ---------------- */

static void test_handle_auth_request_success(void **state) {
  (void) state;
  HttpRequest req;
  HttpResponse res;
  memset(&res, 0, sizeof(HttpResponse));
  handle_auth_request(&req, &res);
  /* Status code kontrol */
  assert_int_equal(res.status_code, 200);
  /* Content-Type kontrol */
  assert_string_equal(res.content_type, "application/json");
  /* Body içeriği kontrol */
  assert_non_null(strstr(res.body, "\"status\":\"authenticated\""));
  assert_non_null(strstr(res.body, "\"token\":\"mocked_jwt_token\""));
  assert_non_null(strstr(res.body, "\"role\":\"admin\""));
  assert_non_null(strstr(res.body, "\"username\":\"admin\""));
}

/* ---------------- MAIN ---------------- */

int main(void) {
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_handle_auth_request_success),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
