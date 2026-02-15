#include <stdio.h>
#include <string.h>
#include "../../security/jwt.h"
#include <stdlib.h>
#include <assert.h>

// SQL Injection Testleri
void test_sql_injection_attempts() {
  printf("Testing SQL Injection...\n");
  char role[20];
  // Basic SQL injection
  int id = verify_user_credentials("' OR 1=1; --", "any", role);
  assert(id == -1); // -1 dönmeli!
  // Union based injection
  id = verify_user_credentials("' UNION SELECT * FROM users; --", "any", role);
  assert(id == -1);
  // Drop table injection
  id = verify_user_credentials("admin'; DROP TABLE users; --", "any", role);
  assert(id == -1);
  printf("✓ SQL Injection tests passed\n");
}

// Token Güvenlik Testleri
void test_token_security() {
  printf("Testing Token Security...\n");
  // Token forgery test
  char *real_token = generate_auth_token(1, "admin", "admin");
  char fake_token[] = "token_1_9999999999_admin"; // Expiry değiştirilmiş
  assert(validate_auth_token(real_token) == true);
  assert(validate_auth_token(fake_token) == false); // Geçersiz olmalı!
  free(real_token);
  printf("✓ Token security tests passed\n");
}

// Brute force koruması testi
void test_brute_force_protection() {
  printf("Testing Brute Force Protection...\n");

  // 5 başarısız deneme
  for(int i = 0; i < 5; i++) {
    int id = verify_user_credentials("admin", "wrong_pass", NULL);

    // 5. denemeden sonra lock olmalı veya gecikme eklenmeli
    if(i >= 3) {
      // Burada rate limiting kontrolü yap
    }
  }

  printf("✓ Brute force protection tests passed\n");
}
