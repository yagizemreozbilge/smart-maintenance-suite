#ifndef MAINTENANCE_SERVICE_H
#define MAINTENANCE_SERVICE_H

#include <stdbool.h>

typedef struct {
  int id;
  int machine_id;
  char technician_name[100];
  char log_date[32];
  char description[512];
  double cost;
} MaintenanceLog;

/**
 * Adds a new maintenance log for a machine.
 * @return true on success.
 */
bool add_maintenance_log(int machine_id, const char *technician, const char *description, double cost);

/**
 * Fetches all maintenance logs for a specific machine.
 * @return Number of logs fetched.
 */
int get_maintenance_history(int machine_id, MaintenanceLog *out_logs, int max_logs);

#endif
