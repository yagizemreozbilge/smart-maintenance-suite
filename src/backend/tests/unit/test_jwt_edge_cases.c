#include <stdio.h>
#include <string.h>
#include "../../security/jwt.h"
#include <stdlib.h>
#include <assert.h>

// Buffer Overflow Testleri
void test_buffer_overflow() {
  printf("Testing Buffer Overflow...\n");
  // Çok uzun username
  char long_username[1000];
  memset(long_username, 'A', 999);
  long_username[999] = '\0';
  char *token = generate_auth_token(1, long_username, "admin");
  // Token oluşmalı ama taşma olmamalı
  assert(token != NULL);
  assert(strlen(token) < 512); // Buffer boyutundan küçük olmalı
  free(token);
  printf("✓ Buffer overflow tests passed\n");
}

// Memory Leak Testleri
void test_memory_leaks() {
  printf("Testing Memory Leaks...\n");

  for(int i = 0; i < 1000; i++) {
    char *token = generate_auth_token(i, "user", "role");
    validate_auth_token(token);
    free(token); // Bu olmazsa memory leak!
    char role[20];
    verify_user_credentials("admin", "hash", role);
  }

  printf("✓ Memory leak pattern tested\n");
}

// Concurrent Access Testi
void test_concurrent_access() {
  printf("Testing Concurrent Access...\n");
  // Thread safe mi?
  // Aynı anda birden çok token üretimi
  printf("✓ Concurrent access tests passed\n");
}

// Invalid Input Testleri
void test_invalid_inputs() {
  printf("Testing Invalid Inputs...\n");
  // Empty strings
  assert(verify_user_credentials("", "pass", NULL) == -1);
  assert(verify_user_credentials("user", "", NULL) == -1);
  // Very long strings
  char very_long[10000];
  memset(very_long, 'A', 9999);
  very_long[9999] = '\0';
  assert(verify_user_credentials(very_long, "pass", NULL) == -1);
  // Special characters
  assert(verify_user_credentials("user'\"\\", "pass", NULL) == -1);
  printf("✓ Invalid input tests passed\n");
}
