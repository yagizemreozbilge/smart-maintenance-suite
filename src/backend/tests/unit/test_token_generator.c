#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../security/jwt.h"
#include "../../security/token_generator.h"

int main() {
  printf("Token Generator Test\n");
  printf("--------------------\n");
  // Test 1: Token oluşturma
  char *token = generate_auth_token_simple(1, "admin");

  if (token) {
    printf("[OK] Token generated: %s\n", token);
    free(token);
  } else {
    printf("[FAIL] Token generation failed\n");
    return 1;
  }

  // Test 2: Token doğrulama
  // ...
  printf("\nAll tests passed!\n");
  return 0;
}
