#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "db_connection.h"

void test_pool_exhaustion_fail_fast() {
  DatabaseConfig cfg;
  memset(&cfg, 0, sizeof(DatabaseConfig));
  cfg.pool_min = 1;
  cfg.pool_max = 1;
  // Initialize pool. Connections will likely fail in local test environment.
  db_pool_init(&cfg, FAIL_FAST, 0);
  // Attempting to acquire when all (failed) connections are NULL
  // should trigger lazy creation, which also fails, and then FAIL_FAST.
  DBConnection *c = db_pool_acquire();
  assert(c == NULL);
  PoolMetrics m = db_pool_get_metrics();
  assert(m.acquire_cnt == 0); // No successful acquires
  db_pool_destroy();
  printf("Pool Exhaustion Fail Fast Test Passed\n");
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
