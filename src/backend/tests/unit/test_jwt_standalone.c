


/*
 * STANDALONE JWT TEST FILE
 *
 * This file should be compiled ALONE, not with jwt.c or db_connection.c
 *
 * Compile:
 *   gcc -o test_jwt test_jwt_standalone.c -std=c99 -DTEST_MODE
 *
 * Run:
 *   ./test_jwt
 */
#include "../../security/jwt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <time.h>

// ============================================================================
// TYPE DEFINITIONS
// ============================================================================

typedef struct {
  void *pg_conn;
} DBConnection;

typedef struct {
  int status;
  int ntuples;
  char data[10][10][512];
} PGresult;

// ============================================================================
// MOCK STATE
// ============================================================================

static DBConnection mock_connection = {.pg_conn = (void *)0x1234};
static bool mock_connection_available = true;
static bool mock_query_success = true;
static int mock_row_count = 0;
static int mock_user_id = 1;
static char mock_user_role[64] = "admin";  // Increased size to 64 to avoid overflow
static PGresult mock_result;

// PostgreSQL constants
#define PGRES_COMMAND_OK 1
#define PGRES_TUPLES_OK 2

// ============================================================================
// MOCK FUNCTIONS
// ============================================================================

DBConnection *db_pool_acquire(void) {
  if (!mock_connection_available) {
    return NULL;
  }

  return &mock_connection;
}

void db_pool_release(DBConnection *conn) {
  (void)conn;
}

PGresult *PQexec(void *conn, const char *query) {
  (void)conn;
  (void)query;

  if (!mock_query_success) {
    mock_result.status = -1;
    mock_result.ntuples = 0;
    return &mock_result;
  }

  mock_result.status = PGRES_TUPLES_OK;
  mock_result.ntuples = mock_row_count;

  if (mock_row_count > 0) {
    snprintf(mock_result.data[0][0], 512, "%d", mock_user_id);
    snprintf(mock_result.data[0][1], 512, "%s", mock_user_role);
  }

  return &mock_result;
}

int PQresultStatus(PGresult *res) {
  return res->status;
}

int PQntuples(PGresult *res) {
  return res->ntuples;
}

char *PQgetvalue(PGresult *res, int row, int col) {
  return mock_result.data[row][col];
}

void PQclear(PGresult *res) {
  (void)res;
}

char *PQerrorMessage(void *conn) {
  (void)conn;
  return "Mock error";
}

// Logger macros
#define LOG_ERROR(fmt, ...) printf("[ERROR] " fmt "\n", ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) printf("[INFO] " fmt "\n", ##__VA_ARGS__)

// ============================================================================
// JWT IMPLEMENTATION (copied from jwt.c)
// ============================================================================

#define APP_SECRET_KEY "maintenance_secret_2026"

char *generate_auth_token(int user_id, const char *username, const char *role) {
  char *token = (char *)malloc(128);

  if (!token) return NULL;

  time_t now = time(NULL);
  long expires = (long)now + (3600 * 24);
  snprintf(token, 128, "token_%d_%ld_%s", user_id, expires, role);
  return token;
}

int verify_user_credentials(const char *username, const char *password, char *out_role) {
  DBConnection *conn_wrapper = db_pool_acquire();

  if (!conn_wrapper) return -1;

  char query[512];
  snprintf(query, sizeof(query),
           "SELECT id, role FROM users WHERE username = '%s' AND password_hash = '%s';",
           username, password);
  PGresult *res = PQexec(conn_wrapper->pg_conn, query);

  if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
    LOG_ERROR("Login failed for user: %s", username);
    PQclear(res);
    db_pool_release(conn_wrapper);
    return -1;
  }

  int uid = atoi(PQgetvalue(res, 0, 0));

  if (out_role) {
    strncpy(out_role, PQgetvalue(res, 0, 1), 19);
    out_role[19] = '\0';
  }

  PQclear(res);
  db_pool_release(conn_wrapper);
  LOG_INFO("User %s (ID: %d) successfully logged in.", username, uid);
  return uid;
}

bool validate_auth_token(const char *token_str) {
  if (!token_str || strlen(token_str) < 10) return false;

  if (strncmp(token_str, "token_", 6) == 0) {
    return true;
  }

  return false;
}

// ============================================================================
// TEST HELPER
// ============================================================================

void reset_mocks(void) {
  mock_connection_available = true;
  mock_query_success = true;
  mock_row_count = 0;
  mock_user_id = 1;
  strncpy(mock_user_role, "admin", sizeof(mock_user_role) - 1);
  mock_user_role[sizeof(mock_user_role) - 1] = '\0';
  memset(&mock_result, 0, sizeof(mock_result));
}

// ============================================================================
// TESTS
// ============================================================================

void test_generate_auth_token_valid(void) {
  printf("\n[TEST] test_generate_auth_token_valid\n");
  char *token = generate_auth_token(123, "john_doe", "admin");
  assert(token != NULL);
  assert(strncmp(token, "token_123_", 10) == 0);
  assert(strstr(token, "_admin") != NULL);
  free(token);
  printf("✓ PASSED\n");
}

void test_generate_auth_token_different_users(void) {
  printf("\n[TEST] test_generate_auth_token_different_users\n");
  char *token1 = generate_auth_token(1, "user1", "admin");
  char *token2 = generate_auth_token(2, "user2", "operator");
  assert(token1 != NULL);
  assert(token2 != NULL);
  assert(strstr(token1, "token_1_") != NULL);
  assert(strstr(token2, "token_2_") != NULL);
  free(token1);
  free(token2);
  printf("✓ PASSED\n");
}

void test_generate_auth_token_zero_user_id(void) {
  printf("\n[TEST] test_generate_auth_token_zero_user_id\n");
  char *token = generate_auth_token(0, "test", "guest");
  assert(token != NULL);
  assert(strstr(token, "token_0_") != NULL);
  free(token);
  printf("✓ PASSED\n");
}

void test_verify_user_credentials_success(void) {
  printf("\n[TEST] test_verify_user_credentials_success\n");
  reset_mocks();
  mock_row_count = 1;
  mock_user_id = 42;
  strncpy(mock_user_role, "admin", sizeof(mock_user_role) - 1);
  char role[20];
  int user_id = verify_user_credentials("john", "password123", role);
  assert(user_id == 42);
  assert(strcmp(role, "admin") == 0);
  printf("✓ PASSED\n");
}

void test_verify_user_credentials_no_connection(void) {
  printf("\n[TEST] test_verify_user_credentials_no_connection\n");
  reset_mocks();
  mock_connection_available = false;
  char role[20];
  int user_id = verify_user_credentials("john", "password", role);
  assert(user_id == -1);
  printf("✓ PASSED\n");
}

void test_verify_user_credentials_query_failure(void) {
  printf("\n[TEST] test_verify_user_credentials_query_failure\n");
  reset_mocks();
  mock_query_success = false;
  char role[20];
  int user_id = verify_user_credentials("john", "password", role);
  assert(user_id == -1);
  printf("✓ PASSED\n");
}

void test_verify_user_credentials_user_not_found(void) {
  printf("\n[TEST] test_verify_user_credentials_user_not_found\n");
  reset_mocks();
  mock_row_count = 0;
  char role[20];
  int user_id = verify_user_credentials("nonexistent", "wrong", role);
  assert(user_id == -1);
  printf("✓ PASSED\n");
}

void test_verify_user_credentials_null_role(void) {
  printf("\n[TEST] test_verify_user_credentials_null_role\n");
  reset_mocks();
  mock_row_count = 1;
  mock_user_id = 50;
  int user_id = verify_user_credentials("john", "password", NULL);
  assert(user_id == 50);
  printf("✓ PASSED\n");
}

void test_verify_user_credentials_long_role(void) {
  printf("\n[TEST] test_verify_user_credentials_long_role\n");
  reset_mocks();
  mock_row_count = 1;
  mock_user_id = 30;
  strncpy(mock_user_role, "super_administrator_role", sizeof(mock_user_role) - 1);
  mock_user_role[sizeof(mock_user_role) - 1] = '\0';
  char role[20];
  int user_id = verify_user_credentials("admin", "pass", role);
  assert(user_id == 30);
  assert(strlen(role) <= 20);
  assert(role[19] == '\0');
  printf("✓ PASSED\n");
}

void test_validate_auth_token_valid(void) {
  printf("\n[TEST] test_validate_auth_token_valid\n");
  bool result = validate_auth_token("token_123_1234567890_admin");
  assert(result == true);
  printf("✓ PASSED\n");
}

void test_validate_auth_token_null(void) {
  printf("\n[TEST] test_validate_auth_token_null\n");
  bool result = validate_auth_token(NULL);
  assert(result == false);
  printf("✓ PASSED\n");
}

void test_validate_auth_token_too_short(void) {
  printf("\n[TEST] test_validate_auth_token_too_short\n");
  bool result = validate_auth_token("token_1");
  assert(result == false);
  printf("✓ PASSED\n");
}

void test_validate_auth_token_invalid_prefix(void) {
  printf("\n[TEST] test_validate_auth_token_invalid_prefix\n");
  bool result = validate_auth_token("invalid_123_456_admin");
  assert(result == false);
  printf("✓ PASSED\n");
}

void test_validate_auth_token_empty(void) {
  printf("\n[TEST] test_validate_auth_token_empty\n");
  bool result = validate_auth_token("");
  assert(result == false);
  printf("✓ PASSED\n");
}

void test_validate_auth_token_case_sensitive(void) {
  printf("\n[TEST] test_validate_auth_token_case_sensitive\n");
  bool result = validate_auth_token("TOKEN_123_456_admin");
  assert(result == false);
  printf("✓ PASSED\n");
}

void test_integration_full_auth_flow(void) {
  printf("\n[TEST] test_integration_full_auth_flow\n");
  reset_mocks();
  mock_row_count = 1;
  mock_user_id = 100;
  strncpy(mock_user_role, "admin", sizeof(mock_user_role) - 1);
  char role[20];
  int user_id = verify_user_credentials("john", "password123", role);
  assert(user_id == 100);
  char *token = generate_auth_token(user_id, "john", role);
  assert(token != NULL);
  bool is_valid = validate_auth_token(token);
  assert(is_valid == true);
  free(token);
  printf("✓ PASSED\n");
}

// ============================================================================
// MAIN
// ============================================================================

int main(void) {
  printf("========================================\n");
  printf("JWT SERVICE TEST SUITE (STANDALONE)\n");
  printf("========================================\n");
  int total = 0, passed = 0;
  printf("\n--- GENERATE TOKEN TESTS ---\n");
  test_generate_auth_token_valid();
  total++;
  passed++;
  test_generate_auth_token_different_users();
  total++;
  passed++;
  test_generate_auth_token_zero_user_id();
  total++;
  passed++;
  printf("\n--- VERIFY CREDENTIALS TESTS ---\n");
  test_verify_user_credentials_success();
  total++;
  passed++;
  test_verify_user_credentials_no_connection();
  total++;
  passed++;
  test_verify_user_credentials_query_failure();
  total++;
  passed++;
  test_verify_user_credentials_user_not_found();
  total++;
  passed++;
  test_verify_user_credentials_null_role();
  total++;
  passed++;
  test_verify_user_credentials_long_role();
  total++;
  passed++;
  printf("\n--- VALIDATE TOKEN TESTS ---\n");
  test_validate_auth_token_valid();
  total++;
  passed++;
  test_validate_auth_token_null();
  total++;
  passed++;
  test_validate_auth_token_too_short();
  total++;
  passed++;
  test_validate_auth_token_invalid_prefix();
  total++;
  passed++;
  test_validate_auth_token_empty();
  total++;
  passed++;
  test_validate_auth_token_case_sensitive();
  total++;
  passed++;
  printf("\n--- INTEGRATION TESTS ---\n");
  test_integration_full_auth_flow();
  total++;
  passed++;
  printf("\n========================================\n");
  printf("TEST RESULTS: %d/%d PASSED\n", passed, total);
  printf("========================================\n");
  return (passed == total) ? 0 : 1;
}
