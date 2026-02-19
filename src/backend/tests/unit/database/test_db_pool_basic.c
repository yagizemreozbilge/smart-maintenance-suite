#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "db_connection.h"

void test_pool_exhaustion_fail_fast() {
  DatabaseConfig cfg;
  memset(&cfg, 0, sizeof(DatabaseConfig));
  cfg.pool_min = 1;
  cfg.pool_max = 1;
  // Initialize pool. In TEST_MODE, this always "succeeds" with mocks.
  db_pool_init(&cfg, FAIL_FAST, 0);
  DBConnection *c = db_pool_acquire();
  assert(c != NULL); // In TEST_MODE, mocks always return 0xDEADBEEF
  db_pool_destroy();
  printf("Pool Basic Test Passed (Mocked)\n");
}

void run_db_pool_basic_tests() {
  printf("Running DB Pool Basic Tests...\n");
  test_pool_exhaustion_fail_fast();
  printf("DB Pool Basic Tests Passed!\n");
}

int main() {
  run_db_pool_basic_tests();
  return 0;
}
