#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "db_connection.c" // White-box testing

void *waiting_worker(void *arg) {
  DBConnection *c = db_pool_acquire();
  *(DBConnection **)arg = c;
  return NULL;
}

void test_fifo_logic() {
  DatabaseConfig cfg;
  memset(&cfg, 0, sizeof(DatabaseConfig));
  cfg.pool_min = 0; // Don't try to connect at init
  cfg.pool_max = 1;
  db_pool_init(&cfg, QUEUE_REQUESTS, 0);
  // Manually inject a dummy connection to simulate success
  // We need a non-null pointer for pg_conn
  g_pool.connections[0].pg_conn = (PGconn *)0xDEADBEEF;
  g_pool.connections[0].in_use = false;
  stack_push(g_pool.free_stack, &g_pool.free_top, 0);
  // Thread A acquires the only connection
  DBConnection *c1 = db_pool_acquire();
  assert(c1 != NULL);
  assert(g_pool.used_cnt == 1);
  // Thread B tries to acquire and should block
  pthread_t t1, t2;
  DBConnection *res1 = NULL, *res2 = NULL;
  pthread_create(&t1, NULL, waiting_worker, &res1);
  usleep(100000); // Wait for T1 to block
  // Thread C tries to acquire and should block behind B
  pthread_create(&t2, NULL, waiting_worker, &res2);
  usleep(100000); // Wait for T2 to block
  // Release connection - T1 (Thread B) should wake up and get it
  db_pool_release(c1);
  pthread_join(t1, NULL);
  assert(res1 != NULL);
  assert(res1 == c1); // Got the same slot
  // Release again - T2 (Thread C) should wake up
  db_pool_release(res1);
  pthread_join(t2, NULL);
  assert(res2 != NULL);
  assert(res2 == c1);
  db_pool_destroy();
  printf("FIFO Logic Test Passed\n");
}

void run_db_pool_fifo_tests() {
  printf("Running DB Pool FIFO Tests...\n");
  test_fifo_logic();
  printf("DB Pool FIFO Tests Passed!\n");
}
