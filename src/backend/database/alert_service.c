#include "alert_service.h"
#include "db_connection.h"
#include "logger.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

bool create_alert(int sensor_id, AlertSeverity severity, const char *message) {
  DBConnection *conn_wrapper = db_pool_acquire();

  if (!conn_wrapper) {
    LOG_ERROR("Could not acquire connection to create alert.");
    return false;
  }

  char query[1024];
  // We use PQescapeStringConn or simple snprintf if we trust the message.
  // For this learning project, we'll use snprintf but be mindful of SQL injection in real apps.
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

void check_and_trigger_alerts(int sensor_id, const char *sensor_type, double value) {
  // Simple Rule Engine Logic
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
