/* This file is a template for types.h. Content will be filled by yagiz on 2025-12-29. */
#ifndef TYPES_H
#define TYPES_H

#include <time.h> // for timestamp

// Sensor data structure
typedef struct {
  int sensor_id;
  double value;   // Temperature etc.
  long timestamp; // Timestamp
} SensorData;

// Operation Type Enum (Factory Scenario)
typedef enum {
  OP_FAULT_ENTRY,      // Fault Entry
  OP_PART_REPLACEMENT, // Part Replacement
  OP_MAINTENANCE_LOG,  // Maintenance Log
  OP_SYSTEM_RESET      // System Reset
} OperationType;

// Detailed operation data for Undo Stack
typedef struct {
  int operation_id;
  int machine_id;
  OperationType type;
  float old_value;     // Old value to restore on undo
  char timestamp[30];  // Operation time
} Operation;

#endif
