#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "../../database/db_connection.c"
#include "../../database/logger.h"

/* ------------------------------------------------------------ */
/* PART 1: Initialization & Config Tests                        */
/* ------------------------------------------------------------ */
void test_db_pool_init_logic() {
  printf("  [Part 1] Testing Init & Config...\n");
  DatabaseConfig cfg;
  memset(&cfg, 0, sizeof(DatabaseConfig));
  strcpy(cfg.host, "localhost");
  cfg.pool_min = 2;
  cfg.pool_max = 5;
  ConnectionPool *pool = db_pool_init(&cfg, FAIL_FAST, 100);
  assert(pool != NULL);
  assert(g_pool.max_size == 5);
  assert(g_pool.min_size == 2);
  assert(g_pool.policy == FAIL_FAST);
  PoolMetrics m = db_pool_get_metrics();
  assert(m.acquire_cnt == 0);
  assert(m.used_cnt == 0);
  db_pool_destroy();
  printf("  [Part 1] Passed!\n");
}

/* ------------------------------------------------------------ */
/* PART 2: Basic Acquire/Release & Metrics                      */
/* ------------------------------------------------------------ */
void test_db_pool_metrics_logic() {
  printf("  [Part 2] Testing Acquire/Release Metrics...\n");
  DatabaseConfig cfg;
  memset(&cfg, 0, sizeof(DatabaseConfig));
  cfg.pool_min = 0;
  cfg.pool_max = 2;
  db_pool_init(&cfg, FAIL_FAST, 0);
  // Mock a connection manually (white-box)
  g_pool.connections[0].pg_conn = (PGconn *)0x1;
  stack_push(g_pool.free_stack, &g_pool.free_top, 0);
  DBConnection *c = db_pool_acquire();
  assert(c != NULL);
  assert(g_pool.used_cnt == 1);
  assert(g_pool.acquire_cnt == 1);
  db_pool_release(c);
  assert(g_pool.used_cnt == 0);
  assert(g_pool.release_cnt == 1);
  db_pool_destroy();
  printf("  [Part 2] Passed!\n");
}

/* ------------------------------------------------------------ */
/* PART 3: FIFO Queue Logic (Multithreaded)                    */
/* ------------------------------------------------------------ */
void *fifo_worker(void *arg) {
  DBConnection *c = db_pool_acquire();
  *(DBConnection **)arg = c;
  return NULL;
}

void test_db_pool_fifo_logic() {
  printf("  [Part 3] Testing FIFO Queue Logic...\n");
  DatabaseConfig cfg;
  memset(&cfg, 0, sizeof(DatabaseConfig));
  cfg.pool_min = 0;
  cfg.pool_max = 1;
  db_pool_init(&cfg, QUEUE_REQUESTS, 0);
  // Mock 1 connection
  g_pool.connections[0].pg_conn = (PGconn *)0x1;
  stack_push(g_pool.free_stack, &g_pool.free_top, 0);
  // 1. Thread A takes the connection
  DBConnection *c1 = db_pool_acquire();
  assert(c1 != NULL);
  // 2. Thread B and C wait
  pthread_t tB, tC;
  DBConnection *resB = NULL, *resC = NULL;
  pthread_create(&tB, NULL, fifo_worker, &resB);
  usleep(50000);
  pthread_create(&tC, NULL, fifo_worker, &resC);
  usleep(50000);
  // 3. Release c1 -> B should wake up first
  db_pool_release(c1);
  pthread_join(tB, NULL);
  assert(resB == c1);
  // 4. Release resB -> C should wake up
  db_pool_release(resB);
  pthread_join(tC, NULL);
  assert(resC == c1);
  db_pool_destroy();
  printf("  [Part 3] Passed!\n");
}

/* ------------------------------------------------------------ */
/* MAIN RUNNER FOR DB POOL                                      */
/* ------------------------------------------------------------ */
void run_db_pool_tests() {
  printf("--- Running DB Connection Pool Tests ---\n");
  test_db_pool_init_logic();
  test_db_pool_metrics_logic();
  test_db_pool_fifo_logic();
  printf("--- DB Connection Pool Tests Passed! ---\n\n");
}
