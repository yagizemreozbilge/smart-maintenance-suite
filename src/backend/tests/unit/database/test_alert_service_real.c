#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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

// Header dosyaları - sıra önemli!
#include "alert_service.h"
#include "db_connection.h"

// Logger mock'ları
void LOG_INFO(const char *format, ...) {}

void LOG_ERROR(const char *format, ...) {}

/* ====================================================================
 * MOCK FONKSİYONLAR
 * ==================================================================== */

static DBConnection mock_conn;
static void *mock_result;
static int mock_acquire_success = 1;
static int mock_result_status = PGRES_COMMAND_OK;
static char mock_error_message[256] = "";
static char mock_query[1024] = "";

static int mock_acquire_called = 0;
static int mock_release_called = 0;
static int mock_pqexec_called = 0;
static int mock_pqclear_called = 0;

typedef struct {
  char id[10];
  char sensor_id[10];
  char severity[20];
  char message[256];
  char created_at[32];
} MockRow;

static MockRow mock_rows[10];
static int mock_row_count = 0;

DBConnection *db_pool_acquire(void) {
  mock_acquire_called++;

  if (mock_acquire_success) {
    mock_conn.pg_conn = (void *)0x1234;
    return &mock_conn;
  }

  return NULL;
}

void db_pool_release(DBConnection *conn) {
  mock_release_called++;
}

void *PQexec(void *conn, const char *query) {
  mock_pqexec_called++;

  if (query) strncpy(mock_query, query, sizeof(mock_query) - 1);

  return mock_result;
}

int PQresultStatus(const void *res) {
  return mock_result_status;
}

char *PQerrorMessage(const void *conn) {
  return mock_error_message;
}

void PQclear(void *res) {
  mock_pqclear_called++;
}

int PQntuples(const void *res) {
  return mock_row_count;
}

char *PQgetvalue(const void *res, int row, int field) {
  if (row < mock_row_count) {
    switch (field) {
      case 0:
        return mock_rows[row].id;

      case 1:
        return mock_rows[row].sensor_id;

      case 2:
        return mock_rows[row].severity;

      case 3:
        return mock_rows[row].message;

      case 4:
        return mock_rows[row].created_at;
    }
  }

  return "";
}

/* ====================================================================
 * TEST SETUP
 * ==================================================================== */

static void reset_mocks(void) {
  mock_acquire_success = 1;
  mock_result_status = PGRES_COMMAND_OK;
  mock_error_message[0] = '\0';
  mock_query[0] = '\0';
  mock_acquire_called = 0;
  mock_release_called = 0;
  mock_pqexec_called = 0;
  mock_pqclear_called = 0;
  mock_row_count = 0;
  memset(mock_rows, 0, sizeof(mock_rows));
}

/* ====================================================================
 * TEST CASES
 * ==================================================================== */

static void test_create_alert_success(void **state) {
  reset_mocks();
  bool result = create_alert(1, 2, "Test message");
  assert_true(result);
  assert_int_equal(mock_acquire_called, 1);
  assert_int_equal(mock_pqexec_called, 1);
  assert_int_equal(mock_pqclear_called, 1);
  assert_int_equal(mock_release_called, 1);
}

static void test_create_alert_acquire_fails(void **state) {
  reset_mocks();
  mock_acquire_success = 0;
  bool result = create_alert(1, 2, "Test");
  assert_false(result);
}

static void test_create_alert_db_error(void **state) {
  reset_mocks();
  mock_result_status = PGRES_FATAL_ERROR;
  bool result = create_alert(1, 2, "Test");
  assert_false(result);
}

static void test_check_temperature_critical(void **state) {
  reset_mocks();
  check_and_trigger_alerts(1, "Temperature", 95.5);
  assert_int_equal(mock_pqexec_called, 1);
}

static void test_check_temperature_normal(void **state) {
  reset_mocks();
  check_and_trigger_alerts(1, "Temperature", 70.0);
  assert_int_equal(mock_pqexec_called, 0);
}

static void test_get_recent_alerts_success(void **state) {
  reset_mocks();
  mock_result_status = PGRES_TUPLES_OK;
  mock_row_count = 2;
  strcpy(mock_rows[0].id, "1");
  strcpy(mock_rows[0].sensor_id, "101");
  strcpy(mock_rows[0].severity, "CRITICAL");
  strcpy(mock_rows[0].message, "Test1");
  strcpy(mock_rows[0].created_at, "2024-01-01");
  strcpy(mock_rows[1].id, "2");
  strcpy(mock_rows[1].sensor_id, "102");
  strcpy(mock_rows[1].severity, "WARNING");
  strcpy(mock_rows[1].message, "Test2");
  strcpy(mock_rows[1].created_at, "2024-01-02");
  AlertInfo alerts[5];
  int count = get_recent_alerts(alerts, 5);
  assert_int_equal(count, 2);
  assert_int_equal(alerts[0].id, 1);
}

static void test_get_recent_alerts_empty(void **state) {
  reset_mocks();
  mock_result_status = PGRES_TUPLES_OK;
  mock_row_count = 0;
  AlertInfo alerts[5];
  int count = get_recent_alerts(alerts, 5);
  assert_int_equal(count, 0);
}

/* ====================================================================
 * MAIN
 * ==================================================================== */

int main(void) {
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_create_alert_success),
    cmocka_unit_test(test_create_alert_acquire_fails),
    cmocka_unit_test(test_create_alert_db_error),
    cmocka_unit_test(test_check_temperature_critical),
    cmocka_unit_test(test_check_temperature_normal),
    cmocka_unit_test(test_get_recent_alerts_success),
    cmocka_unit_test(test_get_recent_alerts_empty),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
