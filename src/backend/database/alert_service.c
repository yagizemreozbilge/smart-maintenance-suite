#include "alert_service.h"
#include "db_connection.h"
#include "logger.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* ------------------------------------------------------------------
 * PRIVATE HELPERS
 * ------------------------------------------------------------------ */

const char *severity_to_str(AlertSeverity severity) {
  switch (severity) {
    case SEVERITY_INFO:
      return "INFO";

    case SEVERITY_WARNING:
      return "WARNING";

    case SEVERITY_CRITICAL:
      return "CRITICAL";

    default:
      return "UNKNOWN";
  }
}

/* ------------------------------------------------------------------
 * PUBLIC API
 * ------------------------------------------------------------------ */
#ifndef TEST_MODE

bool create_alert(int sensor_id, AlertSeverity severity, const char *message) {
  DBConnection *conn_wrapper = db_pool_acquire();

  if (!conn_wrapper) {
    LOG_ERROR("Could not acquire connection to create alert.");
    return false;
  }

  char query[1024];
  snprintf(query, sizeof(query),
           "INSERT INTO alerts (sensor_id, severity, message) VALUES (%d, '%s', '%s');",
           sensor_id, severity_to_str(severity), message);
  PGresult *res = PQexec(conn_wrapper->pg_conn, query);

  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    LOG_ERROR("Failed to insert alert: %s", PQerrorMessage(conn_wrapper->pg_conn));
    PQclear(res);
    db_pool_release(conn_wrapper);
    return false;
  }

  PQclear(res);
  db_pool_release(conn_wrapper);
  LOG_INFO("[ALERT CREATED] Sensor %d: %s (%s)", sensor_id, message, severity_to_str(severity));
  return true;
}

/* ------------------------------------------------------------------ */

void check_and_trigger_alerts(int sensor_id, const char *sensor_type, double value) {
  if (strcmp(sensor_type, "Temperature") == 0) {
    if (value > 90.0) {
      char msg[128];
      snprintf(msg, sizeof(msg), "Critical Temperature detected: %.2f C", value);
      create_alert(sensor_id, SEVERITY_CRITICAL, msg);
    } else if (value > 75.0) {
      char msg[128];
      snprintf(msg, sizeof(msg), "High Temperature warning: %.2f C", value);
      create_alert(sensor_id, SEVERITY_WARNING, msg);
    }
  } else if (strcmp(sensor_type, "Vibration") == 0) {
    if (value > 5.0) {
      char msg[128];
      snprintf(msg, sizeof(msg), "Excessive Vibration detected: %.2f Hz", value);
      create_alert(sensor_id, SEVERITY_CRITICAL, msg);
    }
  } else if (strcmp(sensor_type, "Pressure") == 0) {
    if (value > 12.0) {
      char msg[128];
      snprintf(msg, sizeof(msg), "High Pressure detected: %.2f Bar", value);
      create_alert(sensor_id, SEVERITY_WARNING, msg);
    }
  }
}

/* ------------------------------------------------------------------ */

int get_recent_alerts(AlertInfo *out_alerts, int max_alerts) {
  DBConnection *conn_wrapper = db_pool_acquire();

  if (!conn_wrapper) return 0;

  char query[256];
  snprintf(query, sizeof(query),
           "SELECT id, sensor_id, severity, message, to_char(alert_time, 'YYYY-MM-DD HH24:MI:SS') "
           "FROM alerts ORDER BY alert_time DESC LIMIT %d;",
           max_alerts);
  PGresult *res = PQexec(conn_wrapper->pg_conn, query);

  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    LOG_ERROR("Failed to fetch alerts: %s", PQerrorMessage(conn_wrapper->pg_conn));
    PQclear(res);
    db_pool_release(conn_wrapper);
    return 0;
  }

  int rows = PQntuples(res);
  int count = (rows < max_alerts) ? rows : max_alerts;

  for (int i = 0; i < count; i++) {
    out_alerts[i].id = atoi(PQgetvalue(res, i, 0));
    out_alerts[i].sensor_id = atoi(PQgetvalue(res, i, 1));
    strncpy(out_alerts[i].severity, PQgetvalue(res, i, 2), 15);
    out_alerts[i].severity[15] = '\0';
    strncpy(out_alerts[i].message, PQgetvalue(res, i, 3), 255);
    out_alerts[i].message[255] = '\0';
    strncpy(out_alerts[i].created_at, PQgetvalue(res, i, 4), 31);
    out_alerts[i].created_at[31] = '\0';
  }

  PQclear(res);
  db_pool_release(conn_wrapper);
  return count;
}

#endif // TEST_MODE

/* ------------------------------------------------------------------
 * JSON SERIALIZATION (API ENDPOINTS)
 * ------------------------------------------------------------------ */

#ifndef TEST_MODE

char *alert_service_serialize_alerts(void) {
  AlertInfo alerts[50];
  int count = get_recent_alerts(alerts, 50);
  char *json = (char *)malloc(4096);

  if (!json) return NULL;

  char *ptr = json;
  ptr += sprintf(ptr, "{\"alerts\":[");

  for (int i = 0; i < count; i++) {
    if (i > 0) {
      ptr += sprintf(ptr, ",");
    }

    ptr += sprintf(ptr,
                   "{"
                   "\"id\":%d,"
                   "\"sensor_id\":%d,"
                   "\"severity\":\"%s\","
                   "\"message\":\"%s\","
                   "\"created_at\":\"%s\""
                   "}",
                   alerts[i].id,
                   alerts[i].sensor_id,
                   alerts[i].severity,
                   alerts[i].message,
                   alerts[i].created_at
                  );
  }

  ptr += sprintf(ptr, "]}");
  return json;
}

#endif



/* ------------------------------------------------------------------
 * TEST/MOCK VERSION (compile with -DTEST_MODE)
 * ------------------------------------------------------------------ */

#ifdef TEST_MODE

char *alert_service_serialize_alerts(void) {
  char *json = (char *)malloc(256);

  if (json) {
    strcpy(json, "{\"alerts\":[]}");
  }

  return json;
}

bool create_alert(int sensor_id, AlertSeverity severity, const char *message) {
  (void)sensor_id;
  (void)severity;
  (void)message;
  return true;
}

void check_and_trigger_alerts(int sensor_id, const char *sensor_type, double value) {
  (void)sensor_id;
  (void)sensor_type;
  (void)value;
}

int get_recent_alerts(AlertInfo *out_alerts, int max_alerts) {
  if (out_alerts && max_alerts > 0) {
    out_alerts[0].id = 1;
    out_alerts[0].sensor_id = 1;
    strcpy(out_alerts[0].severity, "CRITICAL");
    strcpy(out_alerts[0].message, "Mock critical alert");
    strcpy(out_alerts[0].created_at, "2025-01-01 10:00:00");
    return 1;
  }

  return 0;
}

#endif
