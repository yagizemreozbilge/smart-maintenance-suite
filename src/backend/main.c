#include "database/db_connection.h"
#include "database/machine_service.h"
#include "database/logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>
int main() {
  // 1. Database Configuration
  // NOTE: Update 'password' with your actual PostgreSQL password.
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
    printf("%-3s | %-20s | %-12s | %-15s\n", "ID", "Name", "Status", "Location");
    printf("----+----------------------+--------------+-----------\n");

    for (int i = 0; i < count; i++) {
      printf("%02d  | %-20s | %-12s | %-15s\n",
             machine_list[i].id,
             machine_list[i].name,
             machine_list[i].status,
             machine_list[i].location);
    }

    printf("======================================================\n\n");
  } else {
    LOG_INFO("No machines found or database is empty.");
  }

  // 4. Secure System Shutdown
  db_pool_destroy();
  LOG_INFO("System shutdown successfully.");
  return EXIT_SUCCESS;
}
