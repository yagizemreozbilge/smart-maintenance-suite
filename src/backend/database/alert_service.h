#ifndef ALERT_SERVICE_H
#define ALERT_SERVICE_H

#include <stdbool.h>

typedef enum {
  SEVERITY_INFO,
  SEVERITY_WARNING,
  SEVERITY_CRITICAL
} AlertSeverity;

/**
 * Converts severity enum to string representation.
 */
const char *severity_to_str(AlertSeverity severity);

/**
 * Creates a new alert in the database.
 */
bool create_alert(int sensor_id, AlertSeverity severity, const char *message);

/**
 * A simple rule engine that checks a value against thresholds.
 */
void check_and_trigger_alerts(int sensor_id, const char *sensor_type, double value);

typedef struct {
  int id;
  int sensor_id;
  char severity[16];
  char message[256];
  char created_at[32];
} AlertInfo;

/**
 * Fetches the most recent alerts from the database.
 */
int get_recent_alerts(AlertInfo *out_alerts, int max_alerts);

/**
 * Serializes alerts to JSON format for API responses.
 * Returns a JSON string that must be freed by the caller.
 */
char *serialize_alerts_to_json(void);

#endif /* ALERT_SERVICE_H */
