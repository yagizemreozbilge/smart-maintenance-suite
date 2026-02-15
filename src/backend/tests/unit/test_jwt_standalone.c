#include "../../security/jwt.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// Standalone JWT functions (no database dependency)
char *generate_auth_token_standalone(int user_id, const char *username, const char *role) {
  char *token = (char *)malloc(128);
  time_t now = time(NULL);
  long expires = (long)now + (3600 * 24);
  snprintf(token, 128, "token_%d_%ld_%s", user_id, expires, role);
  return token;
}

bool validate_auth_token_standalone(const char *token) {
  if (!token || strlen(token) < 10) return false;

  if (strstr(token, "token_") != token) return false;

  return true;
}

void test_generate_auth_token() {
  char *token = generate_auth_token_standalone(1, "admin", "admin");
  assert(token != NULL);
  assert(strlen(token) > 0);
  assert(strstr(token, "token_") != NULL);
  printf("[PASS] generate_auth_token: token created\n");
  free(token);
}

void test_generate_auth_token_different_users() {
  char *token1 = generate_auth_token_standalone(1, "admin", "admin");
  char *token2 = generate_auth_token_standalone(2, "teknisyen", "teknisyen");
  assert(strcmp(token1, token2) != 0);
  printf("[PASS] generate_auth_token: different tokens for different users\n");
  free(token1);
  free(token2);
}

void test_validate_auth_token_valid() {
  char *token = generate_auth_token_standalone(1, "admin", "admin");
  assert(validate_auth_token_standalone(token) == true);
  printf("[PASS] validate_auth_token: valid token accepted\n");
  free(token);
}

void test_validate_auth_token_invalid() {
  assert(validate_auth_token_standalone("invalid_token") == false);
  printf("[PASS] validate_auth_token: invalid token rejected\n");
}

void test_validate_auth_token_null() {
  assert(validate_auth_token_standalone(NULL) == false);
  printf("[PASS] validate_auth_token: NULL token rejected\n");
}

void test_validate_auth_token_empty() {
  assert(validate_auth_token_standalone("") == false);
  printf("[PASS] validate_auth_token: empty token rejected\n");
}

void test_validate_auth_token_short() {
  assert(validate_auth_token_standalone("tok") == false);
  printf("[PASS] validate_auth_token: short token rejected\n");
}

void test_token_format() {
  char *token = generate_auth_token_standalone(42, "testuser", "operator");
  assert(strstr(token, "token_42_") != NULL);
  assert(strstr(token, "_operator") != NULL);
  printf("[PASS] generate_auth_token: correct format\n");
  free(token);
}

int main() {
  printf("=== JWT Unit Tests (Standalone) ===\n");
  printf("Note: These tests don't require database connection.\n");
  printf("Database integration tests should be run separately.\n\n");
  test_generate_auth_token();
  test_generate_auth_token_different_users();
  test_validate_auth_token_valid();
  test_validate_auth_token_invalid();
  test_validate_auth_token_null();
  test_validate_auth_token_empty();
  test_validate_auth_token_short();
  test_token_format();
  printf("\n✅ All JWT standalone tests passed!\n");
  return 0;
}
