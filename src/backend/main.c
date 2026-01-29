#include "database/db_connection.h"
#include "database/machine_service.h"
#include "database/sensor_service.h"
#include "database/alert_service.h"
#include "database/maintenance_service.h"
#include "database/logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>

void show_menu() {
  printf("\n======================================================\n");
  printf("          SMART MAINTENANCE SUITE - ADMIN             \n");
  printf("======================================================\n");
  printf(" 1. List All Machines (Factory Floor)\n");
  printf(" 2. Detailed Health Check (Sensors)\n");
  printf(" 3. View Maintenance History\n");
  printf(" 4. Add New Maintenance Log\n");
  printf(" 5. Simulate Sensor Data (Manual Input)\n");
  printf(" 0. Exit System\n");
  printf("======================================================\n");
  printf("Choice: ");
}

int main() {
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
  LOG_INFO("--- Smart Maintenance Suite System Booting ---");

  if (!db_pool_init(&cfg, BLOCK_WITH_TIMEOUT, 2000)) {
    LOG_ERROR("System failure: Pool initialization failed.");
    return EXIT_FAILURE;
  }

  int choice = -1;

  while (choice != 0) {
    show_menu();

    if (scanf("%d", &choice) != 1) {
      while (getchar() != '\n'); // Clear buffer

      continue;
    }

    switch (choice) {
      case 1: {
        Machine machines[10];
        int count = get_all_machines(machines, 10);
        printf("\n%-3s | %-22s | %-12s | %-15s\n", "ID", "Name", "Status", "Location");
        printf("----+------------------------+--------------+-----------\n");

        for (int i = 0; i < count; i++) {
          printf("%02d  | %-22s | %-12s | %-15s\n",
                 machines[i].id, machines[i].name, machines[i].status, machines[i].location);
        }

        break;
      }

      case 2: {
        int mid;
        printf("Enter Machine ID: ");
        scanf("%d", &mid);
        SensorStatus sensors[10];
        int s_count = get_machine_health(mid, sensors, 10);

        if (s_count > 0) {
          printf("\n--- HEALTH REPORT ---\n");
          printf("%-15s | %-10s | %-8s\n", "Type", "Value", "Unit");

          for (int i = 0; i < s_count; i++) {
            printf("%-15s | %-10.2f | %-8s\n", sensors[i].sensor_type, sensors[i].last_value, sensors[i].unit);
          }
        } else {
          printf("No sensor data for this machine.\n");
        }

        break;
      }

      case 3: {
        int mid;
        printf("Enter Machine ID: ");
        scanf("%d", &mid);
        MaintenanceLog logs[10];
        int l_count = get_maintenance_history(mid, logs, 10);

        if (l_count > 0) {
          printf("\n--- MAINTENANCE LOGS ---\n");

          for (int i = 0; i < l_count; i++) {
            printf("[%s] %-12s: %s ($%.2f)\n", logs[i].log_date, logs[i].technician_name, logs[i].description, logs[i].cost);
          }
        } else {
          printf("No logs found.\n");
        }

        break;
      }

      case 4: {
        int mid;
        char tech[100], desc[512];
        double cost;
        printf("Machine ID: ");
        scanf("%d", &mid);
        printf("Technician: ");
        scanf(" %99[^\n]", tech);
        printf("Description: ");
        scanf(" %511[^\n]", desc);
        printf("Cost: ");
        scanf("%lf", &cost);

        if (add_maintenance_log(mid, tech, desc, cost)) {
          printf("Log saved successfully.\n");
        }

        break;
      }

      case 5: {
        int sid;
        char type[32];
        double val;
        printf("Sensor ID: ");
        scanf("%d", &sid);
        printf("Sensor Type (e.g. Temperature): ");
        scanf("%s", type);
        printf("Recorded Value: ");
        scanf("%lf", &val);

        if (add_sensor_reading(sid, type, val)) {
          printf("Data saved. Rule engine triggered.\n");
        }

        break;
      }

      case 0:
        LOG_INFO("Shutting down system...");
        break;

      default:
        printf("Invalid choice!\n");
    }
  }

  db_pool_destroy();
  return EXIT_SUCCESS;
}
