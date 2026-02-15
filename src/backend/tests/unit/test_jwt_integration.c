#include <stdio.h>
#include <stdlib.h>
#include "../../security/jwt.h"
#include "db_connection.h"
#include <string.h>
#include <assert.h>

// Gerçek veritabanı ile test
void test_real_db_authentication() {
  printf("Testing with Real Database...\n");
  // Önce test kullanıcısı oluştur
  setup_test_user("testuser", "hash123", "editor");
  // Gerçek giriş testi
  char role[20];
  int id = verify_user_credentials("testuser", "hash123", role);
  assert(id > 0);
  assert(strcmp(role, "editor") == 0);
  // Token üret ve doğrula
  char *token = generate_auth_token(id, "testuser", role);
  assert(validate_auth_token(token) == true);
  // Temizlik
  cleanup_test_user("testuser");
  free(token);
  printf("✓ Real database tests passed\n");
}
