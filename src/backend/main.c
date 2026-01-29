#include "database/db_connection.h"
#include "database/machine_service.h"
#include "database/sensor_service.h"
#include "database/alert_service.h"
#include "database/maintenance_service.h"
#include "database/logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>

int main() {
  // 1. Database Configuration
  DatabaseConfig cfg = {
    .host = "localhost",
    .port = 5432,
    .dbname = "smart_maintenance",
    .user = "postgres",
    .password = "grup7_123",
    .sslmode = "disable",
    .connect_timeout = 5,
    .pool_min = 2,
    .pool_max = 5
  };
  // 2. Start the Connection Pool
  LOG_INFO("--- Smart Maintenance Suite System Booting ---");

  if (!db_pool_init(&cfg, BLOCK_WITH_TIMEOUT, 2000)) {
    LOG_ERROR("System failure: Pool initialization failed. Check DB credentials.");
    return EXIT_FAILURE;
  }

  // 3. Fetch Machines from the Factory Floor
  Machine machine_list[10];
  int count = get_all_machines(machine_list, 10);

  if (count > 0) {
    printf("\n======================================================\n");
    printf("               FACTORY FLOOR MONITOR                 \n");
    printf("======================================================\n");
    printf("%-3s | %-22s | %-12s | %-15s\n", "ID", "Name", "Status", "Location");
    printf("----+------------------------+--------------+-----------\n");

    for (int i = 0; i < count; i++) {
      printf("%02d  | %-22s | %-12s | %-15s\n",
             machine_list[i].id,
             machine_list[i].name,
             machine_list[i].status,
             machine_list[i].location);
    }

    printf("======================================================\n");
    // 4. Detailed Health Check for the first machine
    printf("\n>>> Detailed Health Check: [ID: %02d] %s\n", machine_list[0].id, machine_list[0].name);
    SensorStatus sensors[5];
    int s_count = get_machine_health(machine_list[0].id, sensors, 5);

    if (s_count > 0) {
      printf("------------------------------------------------------\n");
      printf("%-15s | %-10s | %-8s | %-19s\n", "Sensor Type", "Value", "Unit", "Last Updated");
      printf("----------------+------------+----------+---------------------\n");

      for (int i = 0; i < s_count; i++) {
        printf("%-15s | %-10.2f | %-8s | %-19s\n",
               sensors[i].sensor_type,
               sensors[i].last_value,
               sensors[i].unit,
               sensors[i].recorded_at);
      }

      printf("------------------------------------------------------\n");
    }

    // 5. Data Ingestion & Alerting Simulation
    if (s_count > 0) {
      printf("\n>>> Simulating CRITICAL Data Ingestion... (Sensor: %s, Value: 95.5)\n", sensors[0].sensor_type);

      if (add_sensor_reading(sensors[0].sensor_id, sensors[0].sensor_type, 95.5)) {
        LOG_INFO("New sensor reading saved. Check logs for triggered alerts!");
      }
    }

    // 6. Maintenance Logs Simulation
    printf("\n>>> Adding Maintenance Log for %s\n", machine_list[0].name);
    add_maintenance_log(machine_list[0].id, "Yagiz Emre", "Fixed high temperature issue by replacing the cooling pump.", 250.0);
    MaintenanceLog logs[5];
    int l_count = get_maintenance_history(machine_list[0].id, logs, 5);

    if (l_count > 0) {
      printf("\n--- MAINTENANCE HISTORY ---\n");

      for (int i = 0; i < l_count; i++) {
        printf("[%s] Tech: %-12s | Cost: $%-7.2f | Desc: %s\n",
               logs[i].log_date, logs[i].technician_name, logs[i].cost, logs[i].description);
      }

      printf("---------------------------\n");
    }
  } else {
    LOG_INFO("No machines found or database is empty.");
  }

  // 7. Secure System Shutdown
  db_pool_destroy();
  LOG_INFO("System shutdown successfully.");
  return EXIT_SUCCESS;
}
