#include "database/db_connection.h"
#include "database/machine_service.h"
#include "database/sensor_service.h"
#include "database/alert_service.h"
#include "database/maintenance_service.h"
#include "database/inventory_service.h"
#include "database/api_handlers.h"
#include "api/http_server.h"
#include "database/logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database/cJSON.h"
#include <libpq-fe.h>

/* ANSI Color Codes - High Intensity for that "Electric" Look */
#define RESET     "\033[0m"
#define BOLD_BLUE "\033[1;34m"
#define B_CYAN    "\033[1;36m"
#define B_RED     "\033[1;31m"
#define B_YELLOW  "\033[1;33m"
#define B_GREEN   "\033[1;32m"
#define B_MAGENTA "\033[1;35m"

void clear_screen() {
#ifdef _WIN32
  system("cls");
#else
  system("clear");
#endif
}

void show_menu() {
  printf("\n%s======================================================%s\n", BOLD_BLUE, RESET);
  printf("%s          SMART MAINTENANCE SUITE - DASHBOARD         %s\n", BOLD_BLUE, RESET);
  printf("%s======================================================%s\n", BOLD_BLUE, RESET);
  printf(" 1. %sList All Machines (Factory Floor)%s\n", B_CYAN, RESET);
  printf(" 2. %sDetailed Health Check (Sensors)%s\n", B_CYAN, RESET);
  printf(" 3. %sView Maintenance History%s\n", B_CYAN, RESET);
  printf(" 4. %sAdd New Maintenance Log%s\n", B_CYAN, RESET);
  printf(" 5. %sView Recent Alerts (%sSecurity Dashboard%s%s)\n", B_CYAN, B_RED, B_CYAN, RESET);
  printf(" 6. %sSimulate Sensor Data (Test Engine)%s\n", B_YELLOW, RESET);
  printf(" 7. %sInventory Management%s\n", B_CYAN, RESET);
  printf(" 8. %sJSON Export Test%s (" B_GREEN "API Simulation" RESET ")\n", B_CYAN, RESET);
  printf(" 0. %sExit System%s\n", B_RED, RESET);
  printf("%s======================================================%s\n", BOLD_BLUE, RESET);
  printf("Choice: ");
}

int main() {
  DatabaseConfig cfg = {
    .host = getenv("DB_HOST") ? getenv("DB_HOST") : "localhost",
    .port = getenv("DB_PORT") ? atoi(getenv("DB_PORT")) : 5432,
    .dbname = getenv("DB_NAME") ? getenv("DB_NAME") : "smart_maintenance",
    .user = getenv("DB_USER") ? getenv("DB_USER") : "postgres",
    .password = getenv("DB_PASSWORD") ? getenv("DB_PASSWORD") : "grup7_123",
    .sslmode = getenv("DB_SSLMODE") ? getenv("DB_SSLMODE") : "disable",
    .connect_timeout = getenv("DB_CONNECT_TIMEOUT") ? atoi(getenv("DB_CONNECT_TIMEOUT")) : 5,
    .pool_min = getenv("POOL_MIN") ? atoi(getenv("POOL_MIN")) : 2,
    .pool_max = getenv("POOL_MAX") ? atoi(getenv("POOL_MAX")) : 5
  };

  if (!db_pool_init(&cfg, BLOCK_WITH_TIMEOUT, 2000)) {
    LOG_ERROR("System failure: Pool initialization failed.");
    return EXIT_FAILURE;
  }

  // Start the Web API Server
  if (start_http_server(8080)) {
    printf("%s[WEB] API is live at http://localhost:8080/api/machines%s\n", B_GREEN, RESET);
  }

  int choice = -1;

  while (choice != 0) {
    clear_screen();
    show_menu();

    if (scanf("%d", &choice) != 1) {
      while (getchar() != '\n');

      continue;
    }

    switch (choice) {
      case 1: {
        Machine machines[10];
        int count = get_all_machines(machines, 10);
        printf("\n%s%-3s | %-22s | %-12s | %-15s%s\n", B_YELLOW, "ID", "Name", "Status", "Location", RESET);
        printf("----+------------------------+--------------+-----------\n");

        for (int i = 0; i < count; i++) {
          const char *status_color = (strcmp(machines[i].status, "operational") == 0) ? B_GREEN : B_RED;
          printf("%02d  | %-22s | %s%-12s%s | %-15s\n",
                 machines[i].id, machines[i].name, status_color, machines[i].status, RESET, machines[i].location);
        }

        printf("\nPress Enter to continue...");
        getchar();
        getchar();
        break;
      }

      case 2: {
        int mid;
        printf("Enter Machine ID: ");
        scanf("%d", &mid);
        SensorStatus sensors[10];
        int s_count = get_machine_sensor_stats(mid, sensors, 10);

        if (s_count > 0) {
          printf("\n%s--- HEALTH REPORT (Machine %d) ---%s\n", B_MAGENTA, mid, RESET);
          printf("%-15s | %-10s | %-8s\n", "Type", "Value", "Unit");
          printf("----------------+------------+----------\n");

          for (int i = 0; i < s_count; i++) {
            printf("%-15s | %s%-10.2f%s | %-8s\n", sensors[i].sensor_type, B_YELLOW, sensors[i].last_value, RESET, sensors[i].unit);
          }
        } else {
          printf("%sNo sensor data for this machine.%s\n", B_RED, RESET);
        }

        printf("\nPress Enter to continue...");
        getchar();
        getchar();
        break;
      }

      case 3: {
        int mid;
        printf("Enter Machine ID: ");
        scanf("%d", &mid);
        MaintenanceLog logs[10];
        int l_count = get_maintenance_history(mid, logs, 10);

        if (l_count > 0) {
          printf("\n%s--- MAINTENANCE LOGS ---%s\n", B_GREEN, RESET);

          for (int i = 0; i < l_count; i++) {
            printf("[%s] %s%-12s%s: %s ($%.2f)\n", logs[i].log_date, B_CYAN, logs[i].technician_name, RESET, logs[i].description, logs[i].cost);
          }
        } else {
          printf("%sNo logs found.%s\n", B_YELLOW, RESET);
        }

        printf("\nPress Enter to continue...");
        getchar();
        getchar();
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
          printf("%sLog saved successfully.%s\n", B_GREEN, RESET);
        }

        printf("\nPress Enter to continue...");
        getchar();
        getchar();
        break;
      }

      case 5: {
        AlertInfo alerts[10];
        int a_count = get_recent_alerts(alerts, 10);

        if (a_count > 0) {
          printf("\n%s--- RECENT SYSTEM ALERTS ---%s\n", B_RED, RESET);
          printf("%-20s | %-10s | %s\n", "Time", "Severity", "Message");
          printf("---------------------+------------+----------------------\n");

          for (int i = 0; i < a_count; i++) {
            const char *sev_col = (strcmp(alerts[i].severity, "CRITICAL") == 0) ? B_RED : B_YELLOW;
            printf("%-20s | %s%-10s%s | %s\n", alerts[i].created_at, sev_col, alerts[i].severity, RESET, alerts[i].message);
          }
        } else {
          printf("%sNo alerts found. System is healthy.%s\n", B_GREEN, RESET);
        }

        printf("\nPress Enter to continue...");
        getchar();
        getchar();
        break;
      }

      case 6: {
        int sid;
        char type[32];
        double val;
        printf("Sensor ID: ");
        scanf("%d", &sid);
        printf("Sensor Type: ");
        scanf("%s", type);
        printf("Recorded Value: ");
        scanf("%lf", &val);

        if (add_sensor_reading(sid, type, val)) {
          printf("%sData saved. Engine checks triggered.%s\n", B_GREEN, RESET);
        }

        printf("\nPress Enter to continue...");
        getchar();
        getchar();
        break;
      }

      case 7: {
        InventoryItem items[50];
        int count = get_all_inventory(items, 50);
        printf("\n%s--- INVENTORY STATUS ---%s\n", B_CYAN, RESET);
        printf("%-20s | %-12s | %-8s | %-8s\n", "Part Name", "SKU", "Stock", "Min Level");
        printf("---------------------+--------------+----------+----------\n");

        for (int i = 0; i < count; i++) {
          const char *col = (items[i].quantity < items[i].min_stock_level) ? B_RED : B_GREEN;
          printf("%-20s | %-12s | %s%-8d%s | %-8d\n",
                 items[i].part_name, items[i].sku, col, items[i].quantity, RESET, items[i].min_stock_level);
        }

        printf("\nPress Enter to continue...");
        getchar();
        getchar();
        break;
      }

      case 8: {
        printf("\n%s--- JSON SERIALIZATION TEST ---%s\n", B_MAGENTA, RESET);
        printf("\n%s[Machines API Output]:%s\n", B_YELLOW, RESET);
        char *machine_json = serialize_machines_to_json();
        printf("%s\n", machine_json);
        free(machine_json);
        printf("\n%s[Inventory API Output]:%s\n", B_YELLOW, RESET);
        char *inv_json = serialize_inventory_to_json();
        printf("%s\n", inv_json);
        free(inv_json);
        printf("\nPress Enter to continue...");
        getchar();
        getchar();
        break;
      }

      case 0:
        LOG_INFO("Shutting down system...");
        break;

      default:
        printf("%sInvalid choice!%s\n", B_RED, RESET);
        printf("\nPress Enter to continue...");
        getchar();
        getchar();
    }
  }

  stop_http_server();
  db_pool_destroy();
  return EXIT_SUCCESS;
}
