#ifndef SENSOR_SERVICE_H
#define SENSOR_SERVICE_H

#include <stdbool.h>

/** Sensor data representation for health monitoring */
typedef struct {
  int sensor_id;
  char sensor_type[32];      // e.g., "Temperature", "Vibration", "Pressure"
  double last_value;  // Most recent reading from the database
  char unit[10];      // e.g., "C", "Hz", "Bar"
  char recorded_at[32];
} SensorStatus;

/**
 * Fetches the latest health metrics for all sensors on a specific machine.
 */
int get_machine_sensor_stats(int machine_id, SensorStatus *out_stats, int max_sensors);

/**
 * Adds a new sensor reading and triggers the alert engine.
 * @param sensor_id  The ID of the sensor.
 * @param type       The type of sensor (for rule engine).
 * @param value      The measured value.
 * @return true on success, false otherwise.
 */
bool add_sensor_reading(int sensor_id, const char *type, double value);

#endif
