#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// PostgreSQL'i tamamen devre dışı bırak
#define LIBPQ_FE_H
#define PQconninfoOption void
#define PGconn void
#define PGresult void
#define ConnStatusType int
#define ExecStatusType int
#define CONNECTION_OK 0
#define CONNECTION_BAD 1
#define PGRES_COMMAND_OK 1
#define PGRES_TUPLES_OK 2
#define PGRES_FATAL_ERROR 3

#include "db_connection.h"

// Logger mock'ları
void LOG_INFO(const char *format, ...) {}

void LOG_ERROR(const char *format, ...) {}

/* ====================================================================
 * MOCK FONKSİYONLAR
 * ==================================================================== */

static int mock_connection_status = CONNECTION_OK;
static char mock_error_message[256] = "";
static int mock_connect_fail = 0;
static void *mock_conn_ptr = (void *)0x12345678;

static int mock_PQconnectdb_called = 0;
static int mock_PQfinish_called = 0;
static int mock_PQstatus_called = 0;

void *PQconnectdb(const char *conninfo) {
  mock_PQconnectdb_called++;

  if (mock_connect_fail) {
    return NULL;
  }

  return mock_conn_ptr;
}

void PQfinish(void *conn) {
  mock_PQfinish_called++;
}

int PQstatus(const void *conn) {
  mock_PQstatus_called++;
  return mock_connection_status;
}

char *PQerrorMessage(const void *conn) {
  return mock_error_message;
}

/* ====================================================================
 * TEST SETUP
 * ==================================================================== */

static void reset_mocks(void) {
  mock_connection_status = CONNECTION_OK;
  mock_error_message[0] = '\0';
  mock_connect_fail = 0;
  mock_PQconnectdb_called = 0;
  mock_PQfinish_called = 0;
  mock_PQstatus_called = 0;
}

static void setup_default_config(DatabaseConfig *cfg) {
  memset(cfg, 0, sizeof(DatabaseConfig));
  strcpy(cfg->host, "localhost");
  cfg->port = 5432;
  strcpy(cfg->dbname, "testdb");
  strcpy(cfg->user, "testuser");
  strcpy(cfg->password, "testpass");
  strcpy(cfg->sslmode, "disable");
  cfg->connect_timeout = 5;
  cfg->pool_min = 2;
  cfg->pool_max = 5;
}

/* ====================================================================
 * TEST CASES
 * ==================================================================== */

static void test_pool_init_success(void **state) {
  reset_mocks();
  DatabaseConfig cfg;
  setup_default_config(&cfg);
  ConnectionPool *pool = db_pool_init(&cfg, 0, 0);
  assert_non_null(pool);
  db_pool_destroy();
}

static void test_acquire_success(void **state) {
  reset_mocks();
  DatabaseConfig cfg;
  setup_default_config(&cfg);
  ConnectionPool *pool = db_pool_init(&cfg, 0, 0);
  DBConnection *conn = db_pool_acquire();
  assert_non_null(conn);
  db_pool_release(conn);
  db_pool_destroy();
}

static void test_acquire_all_and_exhaust(void **state) {
  reset_mocks();
  DatabaseConfig cfg;
  setup_default_config(&cfg);
  cfg.pool_max = 3;
  ConnectionPool *pool = db_pool_init(&cfg, 0, 0);
  DBConnection *conns[5];

  for (int i = 0; i < cfg.pool_max; i++) {
    conns[i] = db_pool_acquire();
    assert_non_null(conns[i]);
  }

  DBConnection *exhausted = db_pool_acquire();
  assert_null(exhausted);

  for (int i = 0; i < cfg.pool_max; i++) {
    db_pool_release(conns[i]);
  }

  db_pool_destroy();
}

static void test_metrics(void **state) {
  reset_mocks();
  DatabaseConfig cfg;
  setup_default_config(&cfg);
  ConnectionPool *pool = db_pool_init(&cfg, 0, 0);
  PoolMetrics metrics = db_pool_get_metrics();
  DBConnection *conn = db_pool_acquire();
  assert_non_null(conn);
  metrics = db_pool_get_metrics();
  printf("\nDEBUG: acquire_cnt=%zu, used_cnt=%d\n", metrics.acquire_cnt, metrics.used_cnt);
  db_pool_release(conn);
  metrics = db_pool_get_metrics();
  printf("DEBUG: release_cnt=%zu, used_cnt=%d\n", metrics.release_cnt, metrics.used_cnt);
  db_pool_destroy();
}

static void test_release_null(void **state) {
  reset_mocks();
  DatabaseConfig cfg;
  setup_default_config(&cfg);
  ConnectionPool *pool = db_pool_init(&cfg, 0, 0);
  db_pool_release(NULL);
  db_pool_destroy();
}

/* ====================================================================
 * MAIN
 * ==================================================================== */

int main(void) {
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_pool_init_success),
    cmocka_unit_test(test_acquire_success),
    cmocka_unit_test(test_acquire_all_and_exhaust),
    cmocka_unit_test(test_metrics),
    cmocka_unit_test(test_release_null),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
