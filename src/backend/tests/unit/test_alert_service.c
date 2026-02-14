#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Mock edilecek fonksiyonlar için宏lar
#define UNIT_TESTING 1

// Header dosyalarını include et
#include "alert_service.h"
#include "db_connection.h"
#include "logger.h"

/* ====================================================================
 * MOCK FUNCTIONS
 * ==================================================================== */

// Mock global variables
static DBConnection mock_conn;
static PGresult mock_result;
static int mock_acquire_success = 1;
static int mock_execute_success = 1;
static int mock_result_status = PGRES_COMMAND_OK;
static int mock_rows_returned = 0;
static char mock_error_message[256] = "";
static char mock_query[1024] = "";
static int mock_release_called = 0;
static int mock_acquire_called = 0;
static int mock_pqexec_called = 0;
static int mock_pqclear_called = 0;

// Mock row data for get_recent_alerts
typedef struct {
  char id[10];
  char sensor_id[10];
  char severity[20];
  char message[256];
  char created_at[32];
} MockRow;

static MockRow mock_rows[10];
static int mock_row_count = 0;

// Mock functions
DBConnection *db_pool_acquire(void) {
  mock_acquire_called++;

  if (mock_acquire_success) {
    mock_conn.pg_conn = (PGconn *)0x1234; // Non-null mock pointer
    return &mock_conn;
  }

  return NULL;
}

void db_pool_release(DBConnection *conn) {
  mock_release_called++;
  assert_true(conn == &mock_conn);
}

PGresult *PQexec(PGconn *conn, const char *query) {
  mock_pqexec_called++;
  strncpy(mock_query, query, sizeof(mock_query) - 1);
  return &mock_result;
}

ExecStatusType PQresultStatus(const PGresult *res) {
  return mock_result_status;
}

const char *PQerrorMessage(const PGconn *conn) {
  return mock_error_message;
}

void PQclear(PGresult *res) {
  mock_pqclear_called++;
  assert_true(res == &mock_result);
}

int PQntuples(const PGresult *res) {
  return mock_row_count;
}

char *PQgetvalue(const PGresult *res, int row, int field) {
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

// Logger mock
void LOG_ERROR(const char *format, ...) {
  // Just for coverage, we don't need to verify
}

void LOG_INFO(const char *format, ...) {
  // Just for coverage, we don't need to verify
}

/* ====================================================================
 * Test Setup/Teardown
 * ==================================================================== */

static void reset_mocks(void) {
  mock_acquire_success = 1;
  mock_execute_success = 1;
  mock_result_status = PGRES_COMMAND_OK;
  mock_rows_returned = 0;
  mock_error_message[0] = '\0';
  mock_query[0] = '\0';
  mock_release_called = 0;
  mock_acquire_called = 0;
  mock_pqexec_called = 0;
  mock_pqclear_called = 0;
  mock_row_count = 0;
  memset(mock_rows, 0, sizeof(mock_rows));
}

/* ====================================================================
 * Test Cases for create_alert
 * ==================================================================== */

static void test_create_alert_success(void **state) {
  reset_mocks();
  bool result = create_alert(1, SEVERITY_CRITICAL, "Test message");
  assert_true(result);
  assert_int_equal(mock_acquire_called, 1);
  assert_int_equal(mock_pqexec_called, 1);
  assert_int_equal(mock_pqclear_called, 1);
  assert_int_equal(mock_release_called, 1);
  // Verify query contains expected values
  assert_non_null(strstr(mock_query, "INSERT INTO alerts"));
  assert_non_null(strstr(mock_query, "1"));
  assert_non_null(strstr(mock_query, "CRITICAL"));
  assert_non_null(strstr(mock_query, "Test message"));
}

static void test_create_alert_info_success(void **state) {
  reset_mocks();
  bool result = create_alert(2, SEVERITY_INFO, "Info message");
  assert_true(result);
  assert_non_null(strstr(mock_query, "INFO"));
}

static void test_create_alert_warning_success(void **state) {
  reset_mocks();
  bool result = create_alert(3, SEVERITY_WARNING, "Warning message");
  assert_true(result);
  assert_non_null(strstr(mock_query, "WARNING"));
}

static void test_create_alert_invalid_severity(void **state) {
  reset_mocks();
  bool result = create_alert(4, 999, "Invalid severity");
  assert_true(result); // Should still succeed with UNKNOWN
  assert_non_null(strstr(mock_query, "UNKNOWN"));
}

static void test_create_alert_acquire_fails(void **state) {
  reset_mocks();
  mock_acquire_success = 0;
  bool result = create_alert(1, SEVERITY_CRITICAL, "Test message");
  assert_false(result);
  assert_int_equal(mock_acquire_called, 1);
  assert_int_equal(mock_pqexec_called, 0);
  assert_int_equal(mock_release_called, 0);
}

static void test_create_alert_db_error(void **state) {
  reset_mocks();
  mock_result_status = PGRES_FATAL_ERROR;
  strcpy(mock_error_message, "Database error");
  bool result = create_alert(1, SEVERITY_CRITICAL, "Test message");
  assert_false(result);
  assert_int_equal(mock_acquire_called, 1);
  assert_int_equal(mock_pqexec_called, 1);
  assert_int_equal(mock_pqclear_called, 1);
  assert_int_equal(mock_release_called, 1);
}

static void test_create_alert_sql_injection(void **state) {
  reset_mocks();
  // Message with single quote
  bool result = create_alert(1, SEVERITY_CRITICAL, "Test's message with' quote");
  assert_true(result);
  // Note: The current implementation doesn't escape quotes, so this test
  // just verifies the function doesn't crash
}

/* ====================================================================
 * Test Cases for check_and_trigger_alerts
 * ==================================================================== */

static void test_check_temperature_critical(void **state) {
  reset_mocks();
  mock_acquire_success = 1;
  mock_result_status = PGRES_COMMAND_OK;
  check_and_trigger_alerts(1, "Temperature", 95.5);
  // Should call create_alert with CRITICAL
  assert_int_equal(mock_acquire_called, 1);
  assert_int_equal(mock_pqexec_called, 1);
  assert_non_null(strstr(mock_query, "CRITICAL"));
  assert_non_null(strstr(mock_query, "Critical Temperature"));
}

static void test_check_temperature_warning(void **state) {
  reset_mocks();
  check_and_trigger_alerts(1, "Temperature", 80.0);
  assert_int_equal(mock_acquire_called, 1);
  assert_non_null(strstr(mock_query, "WARNING"));
  assert_non_null(strstr(mock_query, "High Temperature warning"));
}

static void test_check_temperature_normal(void **state) {
  reset_mocks();
  check_and_trigger_alerts(1, "Temperature", 70.0);
  assert_int_equal(mock_acquire_called, 0); // No alert should be created
}

static void test_check_vibration_critical(void **state) {
  reset_mocks();
  check_and_trigger_alerts(2, "Vibration", 6.5);
  assert_int_equal(mock_acquire_called, 1);
  assert_non_null(strstr(mock_query, "CRITICAL"));
  assert_non_null(strstr(mock_query, "Excessive Vibration"));
}

static void test_check_vibration_normal(void **state) {
  reset_mocks();
  check_and_trigger_alerts(2, "Vibration", 4.0);
  assert_int_equal(mock_acquire_called, 0);
}

static void test_check_pressure_warning(void **state) {
  reset_mocks();
  check_and_trigger_alerts(3, "Pressure", 15.0);
  assert_int_equal(mock_acquire_called, 1);
  assert_non_null(strstr(mock_query, "WARNING"));
  assert_non_null(strstr(mock_query, "High Pressure"));
}

static void test_check_pressure_normal(void **state) {
  reset_mocks();
  check_and_trigger_alerts(3, "Pressure", 10.0);
  assert_int_equal(mock_acquire_called, 0);
}

static void test_check_unknown_sensor_type(void **state) {
  reset_mocks();
  check_and_trigger_alerts(4, "Unknown", 100.0);
  assert_int_equal(mock_acquire_called, 0); // No alert for unknown type
}

/* ====================================================================
 * Test Cases for get_recent_alerts
 * ==================================================================== */

static void test_get_recent_alerts_success(void **state) {
  reset_mocks();
  mock_result_status = PGRES_TUPLES_OK;
  // Setup mock rows
  mock_row_count = 3;
  strcpy(mock_rows[0].id, "1");
  strcpy(mock_rows[0].sensor_id, "101");
  strcpy(mock_rows[0].severity, "CRITICAL");
  strcpy(mock_rows[0].message, "Test alert 1");
  strcpy(mock_rows[0].created_at, "2024-01-01 10:00:00");
  strcpy(mock_rows[1].id, "2");
  strcpy(mock_rows[1].sensor_id, "102");
  strcpy(mock_rows[1].severity, "WARNING");
  strcpy(mock_rows[1].message, "Test alert 2");
  strcpy(mock_rows[1].created_at, "2024-01-01 09:00:00");
  strcpy(mock_rows[2].id, "3");
  strcpy(mock_rows[2].sensor_id, "103");
  strcpy(mock_rows[2].severity, "INFO");
  strcpy(mock_rows[2].message, "Test alert 3");
  strcpy(mock_rows[2].created_at, "2024-01-01 08:00:00");
  AlertInfo alerts[5];
  int count = get_recent_alerts(alerts, 5);
  assert_int_equal(count, 3);
  assert_int_equal(mock_acquire_called, 1);
  assert_int_equal(mock_pqexec_called, 1);
  assert_int_equal(mock_pqclear_called, 1);
  assert_int_equal(mock_release_called, 1);
  // Verify first alert
  assert_int_equal(alerts[0].id, 1);
  assert_int_equal(alerts[0].sensor_id, 101);
  assert_string_equal(alerts[0].severity, "CRITICAL");
  assert_string_equal(alerts[0].message, "Test alert 1");
  assert_string_equal(alerts[0].created_at, "2024-01-01 10:00:00");
  // Verify third alert
  assert_int_equal(alerts[2].id, 3);
  assert_int_equal(alerts[2].sensor_id, 103);
  assert_string_equal(alerts[2].severity, "INFO");
  assert_string_equal(alerts[2].message, "Test alert 3");
}

static void test_get_recent_alerts_max_limit(void **state) {
  reset_mocks();
  mock_result_status = PGRES_TUPLES_OK;
  // Setup 5 mock rows
  mock_row_count = 5;

  for (int i = 0; i < 5; i++) {
    sprintf(mock_rows[i].id, "%d", i+1);
    sprintf(mock_rows[i].sensor_id, "%d", 100+i);
    strcpy(mock_rows[i].severity, "INFO");
    strcpy(mock_rows[i].message, "Test");
    strcpy(mock_rows[i].created_at, "2024-01-01");
  }

  AlertInfo alerts[3];
  int count = get_recent_alerts(alerts, 3);
  assert_int_equal(count, 3); // Should only return max_alerts
  assert_int_equal(mock_acquire_called, 1);
}

static void test_get_recent_alerts_acquire_fails(void **state) {
  reset_mocks();
  mock_acquire_success = 0;
  AlertInfo alerts[5];
  int count = get_recent_alerts(alerts, 5);
  assert_int_equal(count, 0);
  assert_int_equal(mock_pqexec_called, 0);
}

static void test_get_recent_alerts_db_error(void **state) {
  reset_mocks();
  mock_result_status = PGRES_FATAL_ERROR;
  strcpy(mock_error_message, "Query failed");
  AlertInfo alerts[5];
  int count = get_recent_alerts(alerts, 5);
  assert_int_equal(count, 0);
  assert_int_equal(mock_acquire_called, 1);
  assert_int_equal(mock_pqexec_called, 1);
  assert_int_equal(mock_pqclear_called, 1);
  assert_int_equal(mock_release_called, 1);
}

static void test_get_recent_alerts_empty_result(void **state) {
  reset_mocks();
  mock_result_status = PGRES_TUPLES_OK;
  mock_row_count = 0;
  AlertInfo alerts[5];
  int count = get_recent_alerts(alerts, 5);
  assert_int_equal(count, 0);
  assert_int_equal(mock_acquire_called, 1);
  assert_int_equal(mock_pqexec_called, 1);
}

/* ====================================================================
 * Test Cases for alert_service_serialize_alerts
 * ==================================================================== */

static void test_serialize_alerts_success(void **state) {
  reset_mocks();
  mock_result_status = PGRES_TUPLES_OK;
  // Setup mock rows
  mock_row_count = 2;
  strcpy(mock_rows[0].id, "1");
  strcpy(mock_rows[0].sensor_id, "101");
  strcpy(mock_rows[0].severity, "CRITICAL");
  strcpy(mock_rows[0].message, "Test alert 1");
  strcpy(mock_rows[0].created_at, "2024-01-01 10:00:00");
  strcpy(mocks_rows[1].id, "2");
  strcpy(mock_rows[1].sensor_id, "102");
  strcpy(mock_rows[1].severity, "WARNING");
  strcpy(mock_rows[1].message, "Test alert 2");
  strcpy(mock_rows[1].created_at, "2024-01-01 09:00:00");
  char *json = alert_service_serialize_alerts();
  assert_non_null(json);
  assert_non_null(strstr(json, "\"alerts\":["));
  assert_non_null(strstr(json, "\"id\":1"));
  assert_non_null(strstr(json, "\"sensor_id\":101"));
  assert_non_null(strstr(json, "\"severity\":\"CRITICAL\""));
  assert_non_null(strstr(json, "\"message\":\"Test alert 1\""));
  assert_non_null(strstr(json, "\"id\":2"));
  assert_non_null(strstr(json, "\"severity\":\"WARNING\""));
  assert_non_null(strstr(json, "]}"));
  free(json);
}

static void test_serialize_alerts_empty(void **state) {
  reset_mocks();
  mock_result_status = PGRES_TUPLES_OK;
  mock_row_count = 0;
  char *json = alert_service_serialize_alerts();
  assert_non_null(json);
  assert_string_equal(json, "{\"alerts\":[]}");
  free(json);
}

static void test_serialize_alerts_malloc_fail(void **state) {
  // Note: This is tricky to test without overriding malloc
  // We'll skip for now or use a custom malloc wrapper
}

static void test_serialize_alerts_db_error(void **state) {
  reset_mocks();
  mock_result_status = PGRES_FATAL_ERROR;
  char *json = alert_service_serialize_alerts();
  // Should return empty JSON even on DB error
  assert_non_null(json);
  assert_string_equal(json, "{\"alerts\":[]}");
  free(json);
}

/* ====================================================================
 * Test Cases for severity_to_str (indirectly tested through other tests)
 * ==================================================================== */

static void test_severity_to_str_all_values(void **state) {
  // This is a static function, but we can test it through create_alert
  reset_mocks();
  create_alert(1, SEVERITY_INFO, "test");
  assert_non_null(strstr(mock_query, "INFO"));
  create_alert(1, SEVERITY_WARNING, "test");
  assert_non_null(strstr(mock_query, "WARNING"));
  create_alert(1, SEVERITY_CRITICAL, "test");
  assert_non_null(strstr(mock_query, "CRITICAL"));
  create_alert(1, 999, "test");
  assert_non_null(strstr(mock_query, "UNKNOWN"));
}

/* ====================================================================
 * Test Cases for edge cases and buffer overflows
 * ==================================================================== */

static void test_create_alert_long_message(void **state) {
  reset_mocks();
  char long_msg[500];
  memset(long_msg, 'A', 499);
  long_msg[499] = '\0';
  bool result = create_alert(1, SEVERITY_INFO, long_msg);
  assert_true(result);
  // Should not crash, though message might be truncated by snprintf
}

static void test_get_recent_alerts_large_max(void **state) {
  reset_mocks();
  mock_result_status = PGRES_TUPLES_OK;
  mock_row_count = 100; // More than the AlertInfo array in serialize
  AlertInfo alerts[200];
  int count = get_recent_alerts(alerts, 200);
  assert_int_equal(count, 100); // Should return actual rows
}

/* ====================================================================
 * Main test runner
 * ==================================================================== */

int main(void) {
  const struct CMUnitTest tests[] = {
    // create_alert tests
    cmocka_unit_test(test_create_alert_success),
    cmocka_unit_test(test_create_alert_info_success),
    cmocka_unit_test(test_create_alert_warning_success),
    cmocka_unit_test(test_create_alert_invalid_severity),
    cmocka_unit_test(test_create_alert_acquire_fails),
    cmocka_unit_test(test_create_alert_db_error),
    cmocka_unit_test(test_create_alert_sql_injection),

    // check_and_trigger_alerts tests
    cmocka_unit_test(test_check_temperature_critical),
    cmocka_unit_test(test_check_temperature_warning),
    cmocka_unit_test(test_check_temperature_normal),
    cmocka_unit_test(test_check_vibration_critical),
    cmocka_unit_test(test_check_vibration_normal),
    cmocka_unit_test(test_check_pressure_warning),
    cmocka_unit_test(test_check_pressure_normal),
    cmocka_unit_test(test_check_unknown_sensor_type),

    // get_recent_alerts tests
    cmocka_unit_test(test_get_recent_alerts_success),
    cmocka_unit_test(test_get_recent_alerts_max_limit),
    cmocka_unit_test(test_get_recent_alerts_acquire_fails),
    cmocka_unit_test(test_get_recent_alerts_db_error),
    cmocka_unit_test(test_get_recent_alerts_empty_result),

    // serialize tests
    cmocka_unit_test(test_serialize_alerts_success),
    cmocka_unit_test(test_serialize_alerts_empty),
    cmocka_unit_test(test_serialize_alerts_db_error),

    // severity_to_str tests
    cmocka_unit_test(test_severity_to_str_all_values),

    // edge cases
    cmocka_unit_test(test_create_alert_long_message),
    cmocka_unit_test(test_get_recent_alerts_large_max),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
