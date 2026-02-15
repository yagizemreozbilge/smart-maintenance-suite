#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "../../src/backend/security/jwt.h"
#include "../../src/backend/database/db_connection.h"

// ======================================================
// MOCK FUNCTIONS - db_connection.h'daki fonksiyonları mock'luyoruz
// ======================================================

// Mock global değişkenler
static int mock_db_acquire_fail = 0;
static int mock_db_query_fail = 0;
static int mock_db_no_results = 0;
static char mock_query_buffer[1024];

// db_pool_acquire mock'u
DBConnection *__real_db_pool_acquire(void);
DBConnection *__wrap_db_pool_acquire(void) {
  if (mock_db_acquire_fail) {
    return NULL;
  }

  // Gerçek fonksiyon çağrılacakmış gibi dummy bir connection döndür
  static DBConnection dummy_conn;
  dummy_conn.pg_conn = (PGconn *)0x1234; // Dummy pointer
  return &dummy_conn;
}

// db_pool_release mock'u
void __real_db_pool_release(DBConnection *conn);
void __wrap_db_pool_release(DBConnection *conn) {
  // Do nothing in mock
  return;
}

// PQexec mock'u - PostgreSQL'in C kütüphanesi fonksiyonu
PGresult *__real_PQexec(PGconn *conn, const char *query);
PGresult *__wrap_PQexec(PGconn *conn, const char *query) {
  strncpy(mock_query_buffer, query, sizeof(mock_query_buffer) - 1);

  if (mock_db_query_fail) {
    return (PGresult *)0x5678; // Dummy result with error
  }

  return (PGresult *)0x5678; // Dummy result with success
}

// PQresultStatus mock'u
ExecStatusType __real_PQresultStatus(const PGresult *res);
ExecStatusType __wrap_PQresultStatus(const PGresult *res) {
  if (mock_db_query_fail) {
    return PGRES_FATAL_ERROR;
  }

  if (mock_db_no_results) {
    return PGRES_TUPLES_OK; // Ama 0 satır dönecek
  }

  return PGRES_TUPLES_OK;
}

// PQntuples mock'u
int __real_PQntuples(const PGresult *res);
int __wrap_PQntuples(const PGresult *res) {
  if (mock_db_no_results) {
    return 0;
  }

  return 1; // 1 satır döndü varsayıyoruz
}

// PQgetvalue mock'u
char *__real_PQgetvalue(const PGresult *res, int row_number, int column_number);
char *__wrap_PQgetvalue(const PGresult *res, int row_number, int column_number) {
  static char id_str[16];
  static char role_str[20];

  if (column_number == 0) {
    snprintf(id_str, sizeof(id_str), "%d", mock_user_id);
    return id_str;
  } else {
    strcpy(role_str, mock_user_role);
    return role_str;
  }
}

// PQclear mock'u
void __real_PQclear(PGresult *res);
void __wrap_PQclear(PGresult *res) {
  // Do nothing in mock
  return;
}

// Mock verileri
static int mock_user_id = 42;
static char mock_user_role[20] = "admin";
static char expected_username[64];
static char expected_password[64];

// ======================================================
// TEST SETUP & TEARDOWN
// ======================================================

static int setup(void **state) {
  // Her test öncesi mock'ları varsayılan duruma getir
  mock_db_acquire_fail = 0;
  mock_db_query_fail = 0;
  mock_db_no_results = 0;
  mock_user_id = 42;
  strcpy(mock_user_role, "admin");
  memset(mock_query_buffer, 0, sizeof(mock_query_buffer));
  return 0;
}

static int teardown(void **state) {
  // Her test sonrası temizlik
  return 0;
}

// ======================================================
// TEST CASES - generate_auth_token()
// ======================================================

static void test_generate_token_success(void **state) {
  // Normal çalışma
  char *token = generate_auth_token(1, "testuser", "admin");
  assert_non_null(token);
  // Token formatı kontrolü: "token_userid_expires_role"
  char expected_prefix[32];
  snprintf(expected_prefix, sizeof(expected_prefix), "token_1_");
  assert_true(strncmp(token, expected_prefix, strlen(expected_prefix)) == 0);
  assert_true(strstr(token, "admin") != NULL);
  // Token uzunluğu kontrolü (en az 20 karakter olmalı)
  assert_true(strlen(token) > 20);
  free(token);
}

static void test_generate_token_null_username(void **state) {
  // NULL kullanıcı adı - Şu an patlar, düzeltilmeli!
  // Bu test başarısız olacak, ama hatayı gösteriyor
  char *token = generate_auth_token(1, NULL, "admin");
  // Olması gereken: NULL kontrolü yapıp varsayılan bir şey dönmeli veya NULL dönmeli
  // Şu an segmentation fault olur, o yüzden bu testi geçici olarak atlıyoruz
  assert_null(token);

  if (token) free(token);
}

static void test_generate_token_null_role(void **state) {
  // NULL rol
  char *token = generate_auth_token(1, "testuser", NULL);
  // Olması gereken: NULL kontrolü
  assert_null(token);

  if (token) free(token);
}

static void test_generate_token_long_username(void **state) {
  // Buffer overflow testi - Çok uzun kullanıcı adı
  char long_username[200];
  memset(long_username, 'A', 199);
  long_username[199] = '\0';
  char *token = generate_auth_token(1, long_username, "admin");
  // Token oluşmalı ama kesilmiş olmalı (snprintf güvenli)
  assert_non_null(token);
  assert_true(strlen(token) < 128); // 128 byte buffer var
  free(token);
}

static void test_generate_token_memory_allocation(void **state) {
  // Bellek allocation testi
  char *token = generate_auth_token(1, "test", "test");
  assert_non_null(token);
  // Token'ı değiştirebiliyor muyuz? (yazılabilir bellek)
  strcpy(token, "modified");
  assert_string_equal(token, "modified");
  free(token);
}

// ======================================================
// TEST CASES - validate_auth_token()
// ======================================================

static void test_validate_token_success(void **state) {
  // Geçerli token
  bool result = validate_auth_token("token_123_456789_admin");
  assert_true(result);
}

static void test_validate_token_invalid_prefix(void **state) {
  // Yanlış prefix
  bool result = validate_auth_token("invalid_123_admin");
  assert_false(result);
}

static void test_validate_token_null(void **state) {
  // NULL token
  bool result = validate_auth_token(NULL);
  assert_false(result);
}

static void test_validate_token_empty(void **state) {
  // Boş string
  bool result = validate_auth_token("");
  assert_false(result);
}

static void test_validate_token_too_short(void **state) {
  // Çok kısa token (min 10 karakter olmalı)
  bool result = validate_auth_token("token_123");
  assert_false(result);
}

static void test_validate_token_hacker_attempt(void **state) {
  // HACKER TESTİ: "token_" ile başlayan her şey geçerli mi?
  // Şu an bu test BAŞARISIZ olmalı (çünkü kod zayıf)
  bool result = validate_auth_token("token_ben_hackerim_12345");
  // OLMASI GEREKEN: false dönmeli çünkü format uygun değil
  // AMA ŞU AN true dönüyor! Bu güvenlik açığı!
  assert_false(result); // Bu başarısız olacak, güvenlik açığını gösteriyor
}

static void test_validate_token_with_spaces(void **state) {
  // Boşluklu token
  bool result = validate_auth_token("token_123 admin");
  assert_false(result);
}

static void test_validate_token_special_chars(void **state) {
  // Özel karakterli token
  bool result = validate_auth_token("token_123_!@#$%^&*()");
  assert_false(result); // Özel karakterler olmamalı
}

// ======================================================
// TEST CASES - verify_user_credentials()
// ======================================================

static void test_verify_credentials_success(void **state) {
  // Başarılı giriş
  char role[20];
  int user_id = verify_user_credentials("admin", "correct_hash", role);
  assert_true(user_id > 0);
  assert_int_equal(user_id, 42);
  assert_string_equal(role, "admin");
  // SQL query kontrolü
  assert_true(strstr(mock_query_buffer, "admin") != NULL);
  assert_true(strstr(mock_query_buffer, "correct_hash") != NULL);
}

static void test_verify_credentials_null_username(void **state) {
  // NULL kullanıcı adı - Şu an patlar!
  char role[20];
  int user_id = verify_user_credentials(NULL, "pass", role);
  // Olması gereken: -1 dönmeli
  assert_int_equal(user_id, -1);
}

static void test_verify_credentials_null_password(void **state) {
  // NULL şifre
  char role[20];
  int user_id = verify_user_credentials("admin", NULL, role);
  assert_int_equal(user_id, -1);
}

static void test_verify_credentials_null_role_buffer(void **state) {
  // NULL role buffer (opsiyonel)
  int user_id = verify_user_credentials("admin", "correct_hash", NULL);
  assert_true(user_id > 0);
  assert_int_equal(user_id, 42);
}

static void test_verify_credentials_wrong_password(void **state) {
  // Yanlış şifre
  mock_db_no_results = 1; // Hiç satır dönme
  char role[20];
  int user_id = verify_user_credentials("admin", "wrong_hash", role);
  assert_int_equal(user_id, -1);
}

static void test_verify_credentials_nonexistent_user(void **state) {
  // Olmayan kullanıcı
  mock_db_no_results = 1;
  char role[20];
  int user_id = verify_user_credentials("ghost", "hash", role);
  assert_int_equal(user_id, -1);
}

static void test_verify_credentials_sql_injection_attempt(void **state) {
  // SQL INJECTION TESTİ - ÇOK KRİTİK!
  char role[20];
  // ' OR 1=1; --
  int user_id = verify_user_credentials("' OR 1=1; --", "anything", role);
  // Güvenlik testi: Bu -1 dönmeli, yoksa tüm kullanıcılar giriş yapabilir!
  assert_int_equal(user_id, -1);
  // Query'de tırnak işaretleri escape edilmiş mi kontrol et
  // (Bu biraz karmaşık, manuel kontrol gerekebilir)
}

static void test_verify_credentials_db_connection_fail(void **state) {
  // Veritabanı bağlantı hatası
  mock_db_acquire_fail = 1;
  char role[20];
  int user_id = verify_user_credentials("admin", "hash", role);
  assert_int_equal(user_id, -1);
}

static void test_verify_credentials_db_query_fail(void **state) {
  // Sorgu hatası
  mock_db_query_fail = 1;
  char role[20];
  int user_id = verify_user_credentials("admin", "hash", role);
  assert_int_equal(user_id, -1);
}

static void test_verify_credentials_multiple_users(void **state) {
  // Birden fazla kullanıcı dönmesi (olmayan senaryo)
  // Bu test için PQntuples mock'unu değiştirmek gerek
  // Şimdilik atlıyoruz
}

static void test_verify_credentials_long_username(void **state) {
  // Çok uzun kullanıcı adı - buffer overflow testi
  char long_username[1000];
  memset(long_username, 'A', 999);
  long_username[999] = '\0';
  char role[20];
  int user_id = verify_user_credentials(long_username, "hash", role);
  // Patlamamalı, ya -1 dönmeli ya da çalışmalı
  // Ama kesinlikle segmentation fault olmamalı
  assert_true(user_id == -1 || user_id > 0);
}

// ======================================================
// GÜVENLİK TESTLERİ
// ======================================================

static void test_security_token_expiry_check(void **state) {
  // Token expiry kontrolü testi
  // Şu an validate_auth_token expiry KONTROL ETMİYOR!
  // 1 günlük token üret
  char *token = generate_auth_token(1, "user", "admin");
  // Token içindeki expiry timestamp'i parse et
  // Format: "token_userid_expires_role"
  // Şu an validate_auth_token expiry'yi kontrol etmediği için
  // Bu test başarısız olacak - GÜVENLİK AÇIĞI!
  bool valid = validate_auth_token(token);
  assert_true(valid); // Token geçerli
  // NOT: Expiry testi için zamanı manipüle etmek gerek
  // Bu daha karmaşık, şimdilik atlıyoruz
  free(token);
}

static void test_security_token_signature_verification(void **state) {
  // İmza doğrulama testi
  // Şu an jwt.c'de İMZA YOK!
  // Başka bir "secret" ile üretilmiş token'ı doğrulamaya çalış
  char *fake_token = "token_123_456789_admin";
  bool valid = validate_auth_token(fake_token);
  // OLMASI GEREKEN: false (çünkü imza yok/yetersiz)
  // AMA ŞU AN true dönüyor!
  assert_false(valid); // Bu başarısız olacak
}

// ======================================================
// TEST SUITE
// ======================================================

int main(void) {
  const struct CMUnitTest tests[] = {
    // generate_auth_token testleri
    cmocka_unit_test_setup_teardown(test_generate_token_success, setup, teardown),
    cmocka_unit_test_setup_teardown(test_generate_token_null_username, setup, teardown),
    cmocka_unit_test_setup_teardown(test_generate_token_null_role, setup, teardown),
    cmocka_unit_test_setup_teardown(test_generate_token_long_username, setup, teardown),
    cmocka_unit_test_setup_teardown(test_generate_token_memory_allocation, setup, teardown),

    // validate_auth_token testleri
    cmocka_unit_test_setup_teardown(test_validate_token_success, setup, teardown),
    cmocka_unit_test_setup_teardown(test_validate_token_invalid_prefix, setup, teardown),
    cmocka_unit_test_setup_teardown(test_validate_token_null, setup, teardown),
    cmocka_unit_test_setup_teardown(test_validate_token_empty, setup, teardown),
    cmocka_unit_test_setup_teardown(test_validate_token_too_short, setup, teardown),
    cmocka_unit_test_setup_teardown(test_validate_token_hacker_attempt, setup, teardown),
    cmocka_unit_test_setup_teardown(test_validate_token_with_spaces, setup, teardown),
    cmocka_unit_test_setup_teardown(test_validate_token_special_chars, setup, teardown),

    // verify_user_credentials testleri
    cmocka_unit_test_setup_teardown(test_verify_credentials_success, setup, teardown),
    cmocka_unit_test_setup_teardown(test_verify_credentials_null_username, setup, teardown),
    cmocka_unit_test_setup_teardown(test_verify_credentials_null_password, setup, teardown),
    cmocka_unit_test_setup_teardown(test_verify_credentials_null_role_buffer, setup, teardown),
    cmocka_unit_test_setup_teardown(test_verify_credentials_wrong_password, setup, teardown),
    cmocka_unit_test_setup_teardown(test_verify_credentials_nonexistent_user, setup, teardown),
    cmocka_unit_test_setup_teardown(test_verify_credentials_sql_injection_attempt, setup, teardown),
    cmocka_unit_test_setup_teardown(test_verify_credentials_db_connection_fail, setup, teardown),
    cmocka_unit_test_setup_teardown(test_verify_credentials_db_query_fail, setup, teardown),
    cmocka_unit_test_setup_teardown(test_verify_credentials_long_username, setup, teardown),

    // Güvenlik testleri
    cmocka_unit_test_setup_teardown(test_security_token_expiry_check, setup, teardown),
    cmocka_unit_test_setup_teardown(test_security_token_signature_verification, setup, teardown),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
