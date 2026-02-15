#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "jwt.h"

/* ================================
   generate_auth_token TESTS
================================ */

void test_generate_auth_token() {
  char *token = generate_auth_token(5, "admin", "admin");
  assert(token != NULL);
  assert(strncmp(token, "token_5_", 8) == 0);
  free(token);
  printf(" test_generate_auth_token passed\n");
}

/* ================================
   validate_auth_token TESTS
================================ */

void test_validate_auth_token_valid() {
  assert(validate_auth_token("token_1_123_admin") == true);
  printf(" test_validate_auth_token_valid passed\n");
}

void test_validate_auth_token_null() {
  assert(validate_auth_token(NULL) == false);
  printf(" test_validate_auth_token_null passed\n");
}

void test_validate_auth_token_short() {
  assert(validate_auth_token("abc") == false);
  printf(" test_validate_auth_token_short passed\n");
}

void test_validate_auth_token_wrong_prefix() {
  assert(validate_auth_token("wrong_1_123_admin") == false);
  printf(" test_validate_auth_token_wrong_prefix passed\n");
}

void test_validate_auth_token_empty() {
  assert(validate_auth_token("") == false);
  printf(" test_validate_auth_token_empty passed\n");
}



void test_verify_user_credentials_no_connection() {
  int result = verify_user_credentials("admin", "1234", NULL);
  assert(result == -1);
  printf(" test_verify_user_credentials_no_connection passed\n");
}

int main() {
  test_generate_auth_token();
  test_validate_auth_token_valid();
  test_validate_auth_token_null();
  test_validate_auth_token_short();
  test_validate_auth_token_wrong_prefix();
  test_validate_auth_token_empty();
  test_verify_user_credentials_no_connection();
  printf("\n All JWT tests passed!\n");
  return 0;
}
