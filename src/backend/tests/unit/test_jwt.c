#include "../../security/jwt.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

void test_generate_auth_token() {
  char *token = generate_auth_token(1, "admin", "admin");
  assert(token != NULL);
  assert(strlen(token) > 0);
  assert(strstr(token, "token_") != NULL);
  printf("[PASS] generate_auth_token: token created\n");
  free(token);
}

void test_generate_auth_token_different_users() {
  char *token1 = generate_auth_token(1, "admin", "admin");
  char *token2 = generate_auth_token(2, "teknisyen", "teknisyen");
  assert(strcmp(token1, token2) != 0);
  printf("[PASS] generate_auth_token: different tokens for different users\n");
  free(token1);
  free(token2);
}

void test_validate_auth_token_valid() {
  char *token = generate_auth_token(1, "admin", "admin");
  assert(validate_auth_token(token) == true);
  printf("[PASS] validate_auth_token: valid token accepted\n");
  free(token);
}

void test_validate_auth_token_invalid() {
  assert(validate_auth_token("invalid_token") == false);
  printf("[PASS] validate_auth_token: invalid token rejected\n");
}

void test_validate_auth_token_null() {
  assert(validate_auth_token(NULL) == false);
  printf("[PASS] validate_auth_token: NULL token rejected\n");
}

void test_validate_auth_token_empty() {
  assert(validate_auth_token("") == false);
  printf("[PASS] validate_auth_token: empty token rejected\n");
}

void test_validate_auth_token_short() {
  assert(validate_auth_token("tok") == false);
  printf("[PASS] validate_auth_token: short token rejected\n");
}

void test_token_format() {
  char *token = generate_auth_token(42, "testuser", "operator");
  assert(strstr(token, "token_42_") != NULL);
  assert(strstr(token, "_operator") != NULL);
  printf("[PASS] generate_auth_token: correct format\n");
  free(token);
}

int main() {
  printf("=== JWT Unit Tests ===\n");
  printf("Note: Database-dependent tests (verify_user_credentials) require PostgreSQL.\n");
  printf("Running standalone tests only...\n\n");
  test_generate_auth_token();
  test_generate_auth_token_different_users();
  test_validate_auth_token_valid();
  test_validate_auth_token_invalid();
  test_validate_auth_token_null();
  test_validate_auth_token_empty();
  test_validate_auth_token_short();
  test_token_format();
  printf("\n✅ All JWT standalone tests passed!\n");
  printf("⚠️  Database integration tests require PostgreSQL connection.\n");
  return 0;
}
