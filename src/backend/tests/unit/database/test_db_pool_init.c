#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../../database/db_connection.h"

void test_pool_init_values() {
  DatabaseConfig cfg;
  memset(&cfg, 0, sizeof(DatabaseConfig));
  strcpy(cfg.host, "localhost");
  cfg.port = 5432;
  strcpy(cfg.dbname, "testdb");
  cfg.pool_min = 3;
  cfg.pool_max = 7;
  ConnectionPool *pool = db_pool_init(&cfg, FAIL_FAST, 0);
  assert(pool != NULL);
  PoolMetrics m = db_pool_get_metrics();
  assert(m.used_cnt == 0);
  assert(m.acquire_cnt == 0);
  assert(m.release_cnt == 0);
  db_pool_destroy();
  printf("Pool Init Values Test Passed\n");
}

void run_db_pool_init_tests() {
  printf("Running DB Pool Init Tests...\n");
  test_pool_init_values();
  printf("DB Pool Init Tests Passed!\n");
}
