#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "maintenance_service.h"

void test_get_all_maintenance_logs_success() {
  MaintenanceLog logs[5];
  int count = get_all_maintenance_logs(logs, 5);
  assert(count == 1);
  assert(logs[0].id == 1);
  assert(logs[0].machine_id == 1);
  assert(strcmp(logs[0].technician_name, "Ali") == 0);
  assert(strcmp(logs[0].log_date, "2025-01-01") == 0);
  assert(strcmp(logs[0].description, "Routine oil change") == 0);
  assert(logs[0].cost == 150.5);
  printf(" test_get_all_maintenance_logs_success passed\n");
}

void test_get_all_maintenance_logs_null_pointer() {
  int count = get_all_maintenance_logs(NULL, 5);
  assert(count == 0);
  printf(" test_get_all_maintenance_logs_null_pointer passed\n");
}

void test_get_all_maintenance_logs_zero_max() {
  MaintenanceLog logs[5];
  int count = get_all_maintenance_logs(logs, 0);
  assert(count == 0);
  printf("✔ test_get_all_maintenance_logs_zero_max passed\n");
}

int main() {
  test_get_all_maintenance_logs_success();
  test_get_all_maintenance_logs_null_pointer();
  test_get_all_maintenance_logs_zero_max();
  printf("\n All maintenance service tests passed!\n");
  return 0;
}
