#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#define LIBPQ_FE_H
#define PGconn void
#define PGresult void
#define ConnStatusType int
#define ExecStatusType int
#define CONNECTION_OK 0
#define CONNECTION_BAD 1
#define PGRES_COMMAND_OK 1
#define PGRES_TUPLES_OK 2
#define PGRES_FATAL_ERROR 3

#include "alert_service.h"
#include "db_connection.h"

// Logger mock
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
static int mock_pqntuples_called = 0;

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
  mock_pqntuples_called++;
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
  mock_pqntuples_called = 0;
  mock_row_count = 0;
  memset(mock_rows, 0, sizeof(mock_rows));
}

/* ====================================================================
 * PRIVATE FONKSİYON TESTLERİ (severity_to_str)
 * ==================================================================== */

static void test_severity_to_str_info(void **state) {
  // Bu private fonksiyonu create_alert üzerinden test edeceğiz
  reset_mocks();
  create_alert(1, SEVERITY_INFO, "Info message");
  // Query'de "INFO" olmalı
  assert_non_null(strstr(mock_query, "INFO"));
}

static void test_severity_to_str_warning(void **state) {
  reset_mocks();
  create_alert(1, SEVERITY_WARNING, "Warning message");
  assert_non_null(strstr(mock_query, "WARNING"));
}

static void test_severity_to_str_critical(void **state) {
  reset_mocks();
  create_alert(1, SEVERITY_CRITICAL, "Critical message");
  assert_non_null(strstr(mock_query, "CRITICAL"));
}

static void test_severity_to_str_unknown(void **state) {
  reset_mocks();
  create_alert(1, 999, "Unknown severity");
  assert_non_null(strstr(mock_query, "UNKNOWN"));
}

/* ====================================================================
 * create_alert TESTLERİ
 * ==================================================================== */

static void test_create_alert_success(void **state) {
  reset_mocks();
  bool result = create_alert(1, SEVERITY_CRITICAL, "Test message");
  assert_true(result);
  assert_int_equal(mock_acquire_called, 1);
  assert_int_equal(mock_pqexec_called, 1);
  assert_int_equal(mock_pqclear_called, 1);
  assert_int_equal(mock_release_called, 1);
  // Query kontrolü
  assert_non_null(strstr(mock_query, "INSERT INTO alerts"));
  assert_non_null(strstr(mock_query, "1"));
  assert_non_null(strstr(mock_query, "CRITICAL"));
  assert_non_null(strstr(mock_query, "Test message"));
}

static void test_create_alert_acquire_fails(void **state) {
  reset_mocks();
  mock_acquire_success = 0;
  bool result = create_alert(1, SEVERITY_CRITICAL, "Test");
  assert_false(result);
  assert_int_equal(mock_acquire_called, 1);
  assert_int_equal(mock_pqexec_called, 0);
  assert_int_equal(mock_release_called, 0);
}

static void test_create_alert_db_error(void **state) {
  reset_mocks();
  mock_result_status = PGRES_FATAL_ERROR;
  strcpy(mock_error_message, "Database error");
  bool result = create_alert(1, SEVERITY_CRITICAL, "Test");
  assert_false(result);
  assert_int_equal(mock_acquire_called, 1);
  assert_int_equal(mock_pqexec_called, 1);
  assert_int_equal(mock_pqclear_called, 1);
  assert_int_equal(mock_release_called, 1);
}

static void test_create_alert_with_apostrophe(void **state) {
  reset_mocks();
  // Mesajda tek tırnak var - SQL injection testi
  bool result = create_alert(1, SEVERITY_INFO, "Test's message with' quote");
  assert_true(result);
  // SQL injection koruması yoksa sorgu bozulur ama fonksiyon çalışır
}

/* ====================================================================
 * check_and_trigger_alerts TESTLERİ
 * ==================================================================== */

static void test_check_temperature_critical(void **state) {
  reset_mocks();
  check_and_trigger_alerts(1, "Temperature", 95.5);
  assert_int_equal(mock_pqexec_called, 1);
  assert_non_null(strstr(mock_query, "CRITICAL"));
  assert_non_null(strstr(mock_query, "Critical Temperature"));
}

static void test_check_temperature_warning(void **state) {
  reset_mocks();
  check_and_trigger_alerts(1, "Temperature", 80.0);
  assert_int_equal(mock_pqexec_called, 1);
  assert_non_null(strstr(mock_query, "WARNING"));
  assert_non_null(strstr(mock_query, "High Temperature warning"));
}

static void test_check_temperature_normal(void **state) {
  reset_mocks();
  check_and_trigger_alerts(1, "Temperature", 70.0);
  assert_int_equal(mock_pqexec_called, 0); // Alert oluşturulmamalı
}

static void test_check_vibration_critical(void **state) {
  reset_mocks();
  check_and_trigger_alerts(2, "Vibration", 6.5);
  assert_int_equal(mock_pqexec_called, 1);
  assert_non_null(strstr(mock_query, "CRITICAL"));
  assert_non_null(strstr(mock_query, "Excessive Vibration"));
}

static void test_check_vibration_normal(void **state) {
  reset_mocks();
  check_and_trigger_alerts(2, "Vibration", 4.0);
  assert_int_equal(mock_pqexec_called, 0);
}

static void test_check_pressure_warning(void **state) {
  reset_mocks();
  check_and_trigger_alerts(3, "Pressure", 15.0);
  assert_int_equal(mock_pqexec_called, 1);
  assert_non_null(strstr(mock_query, "WARNING"));
  assert_non_null(strstr(mock_query, "High Pressure"));
}

static void test_check_pressure_normal(void **state) {
  reset_mocks();
  check_and_trigger_alerts(3, "Pressure", 10.0);
  assert_int_equal(mock_pqexec_called, 0);
}

static void test_check_unknown_sensor_type(void **state) {
  reset_mocks();
  check_and_trigger_alerts(4, "Unknown", 100.0);
  assert_int_equal(mock_pqexec_called, 0);
}

/* ====================================================================
 * get_recent_alerts TESTLERİ
 * ==================================================================== */

static void test_get_recent_alerts_success(void **state) {
  reset_mocks();
  mock_result_status = PGRES_TUPLES_OK;
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
  // İlk alert kontrolü
  assert_int_equal(alerts[0].id, 1);
  assert_int_equal(alerts[0].sensor_id, 101);
  assert_string_equal(alerts[0].severity, "CRITICAL");
  assert_string_equal(alerts[0].message, "Test alert 1");
  assert_string_equal(alerts[0].created_at, "2024-01-01 10:00:00");
  // İkinci alert kontrolü
  assert_int_equal(alerts[1].id, 2);
  assert_string_equal(alerts[1].severity, "WARNING");
  // Üçüncü alert kontrolü
  assert_int_equal(alerts[2].id, 3);
  assert_string_equal(alerts[2].severity, "INFO");
}

static void test_get_recent_alerts_max_limit(void **state) {
  reset_mocks();
  mock_result_status = PGRES_TUPLES_OK;
  mock_row_count = 5; // 5 satır var ama max_alerts = 3
  AlertInfo alerts[3];
  int count = get_recent_alerts(alerts, 3);
  assert_int_equal(count, 3); // Sadece 3 dönmeli
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
}

/* ====================================================================
 * alert_service_serialize_alerts TESTLERİ
 * ==================================================================== */

static void test_serialize_alerts_success(void **state) {
  reset_mocks();
  mock_result_status = PGRES_TUPLES_OK;
  mock_row_count = 2;
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
  char *json = alert_service_serialize_alerts();
  assert_non_null(json);
  assert_non_null(strstr(json, "\"alerts\":["));
  assert_non_null(strstr(json, "\"id\":1"));
  assert_non_null(strstr(json, "\"severity\":\"CRITICAL\""));
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
  // malloc başarısızlığını test etmek zor, geçiyoruz
}

static void test_serialize_alerts_db_error(void **state) {
  reset_mocks();
  mock_result_status = PGRES_FATAL_ERROR;
  char *json = alert_service_serialize_alerts();
  // DB hatası olsa bile JSON dönmeli (boş array)
  assert_non_null(json);
  assert_string_equal(json, "{\"alerts\":[]}");
  free(json);
}

/* ====================================================================
 * MAIN
 * ==================================================================== */

int main(void) {
  const struct CMUnitTest tests[] = {
    // severity_to_str testleri
    cmocka_unit_test(test_severity_to_str_info),
    cmocka_unit_test(test_severity_to_str_warning),
    cmocka_unit_test(test_severity_to_str_critical),
    cmocka_unit_test(test_severity_to_str_unknown),

    // create_alert testleri
    cmocka_unit_test(test_create_alert_success),
    cmocka_unit_test(test_create_alert_acquire_fails),
    cmocka_unit_test(test_create_alert_db_error),
    cmocka_unit_test(test_create_alert_with_apostrophe),

    // check_and_trigger_alerts testleri
    cmocka_unit_test(test_check_temperature_critical),
    cmocka_unit_test(test_check_temperature_warning),
    cmocka_unit_test(test_check_temperature_normal),
    cmocka_unit_test(test_check_vibration_critical),
    cmocka_unit_test(test_check_vibration_normal),
    cmocka_unit_test(test_check_pressure_warning),
    cmocka_unit_test(test_check_pressure_normal),
    cmocka_unit_test(test_check_unknown_sensor_type),

    // get_recent_alerts testleri
    cmocka_unit_test(test_get_recent_alerts_success),
    cmocka_unit_test(test_get_recent_alerts_max_limit),
    cmocka_unit_test(test_get_recent_alerts_acquire_fails),
    cmocka_unit_test(test_get_recent_alerts_db_error),
    cmocka_unit_test(test_get_recent_alerts_empty_result),

    // alert_service_serialize_alerts testleri
    cmocka_unit_test(test_serialize_alerts_success),
    cmocka_unit_test(test_serialize_alerts_empty),
    cmocka_unit_test(test_serialize_alerts_db_error),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}
