#include "../../security/jwt.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

// Note: This test links against the REAL jwt.c and db_connection.c
// We focus on the JSON/Token logic which doesn't require a live DB for the most part.

void test_jwt_lifecycle() {
  printf("Testing JWT Generate/Validate/Extract lifecycle...\n");
  int user_id = 42;
  const char *user = "testadmin";
  const char *role = "superadmin";
  // 1. Generate
  char *token = generate_auth_token(user_id, user, role);
  assert(token != NULL);
  assert(strncmp(token, "simple_jwt:", 11) == 0);
  printf("Generated Token: %s\n", token);
  // 2. Validate
  assert(validate_auth_token(token) == true);
  // 3. Extract User ID
  int extracted_id = get_user_id_from_token(token);
  assert(extracted_id == user_id);
  // 4. Extract Role
  char *extracted_role = get_role_from_token(token);
  assert(extracted_role != NULL);
  assert(strcmp(extracted_role, role) == 0);
  free(extracted_role);
  // 5. Cleanup
  free(token);
  printf("[PASS] test_jwt_lifecycle\n");
}

void test_jwt_errors() {
  printf("Testing JWT error cases...\n");
  // Null inputs
  assert(generate_auth_token(1, NULL, "admin") == NULL);
  assert(generate_auth_token(1, "admin", NULL) == NULL);
  // Invalid validation
  assert(validate_auth_token(NULL) == false);
  assert(validate_auth_token("") == false);
  assert(validate_auth_token("not_a_jwt") == false);
  assert(validate_auth_token("simple_jwt:invalid_json") == false);
  assert(validate_auth_token("simple_jwt:{\"no_exp\":1}") == false);
  // Invalid extraction
  assert(get_user_id_from_token(NULL) == -1);
  assert(get_user_id_from_token("invalid") == -1);
  assert(get_role_from_token(NULL) == NULL);
  assert(get_role_from_token("invalid") == NULL);
  printf("[PASS] test_jwt_errors\n");
}

int main() {
  printf("=== Real JWT Unit Tests ===\n");
  test_jwt_lifecycle();
  test_jwt_errors();
  printf("\n✅ All JWT tests passed!\n");
  return 0;
}
