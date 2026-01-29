#ifndef ALERT_SERVICE_H
#define ALERT_SERVICE_H

#include <stdbool.h>

typedef enum {
  SEVERITY_INFO,
  SEVERITY_WARNING,
  SEVERITY_CRITICAL
} AlertSeverity;

/**
 * Creates a new alert in the database.
 * @param sensor_id  The ID of the sensor that triggered the alert.
 * @param severity   The severity level of the alert.
 * @param message    A descriptive message about the alert.
 * @return true on success, false otherwise.
 */
bool create_alert(int sensor_id, AlertSeverity severity, const char *message);

/**
 * A simple rule engine that checks a value against thresholds.
 * If a threshold is exceeded, it automatically calls create_alert.
 *
 * @param sensor_id  The ID of the sensor.
 * @param sensor_type The type of sensor (e.g., "Temperature").
 * @param value       The value to check.
 */
void check_and_trigger_alerts(int sensor_id, const char *sensor_type, double value);

#endif
