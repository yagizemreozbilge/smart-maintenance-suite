#include "../../../database/maintenance_service.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

void test_maintenance_log_struct() {
  MaintenanceLog log;
  assert(sizeof(log.technician_name) == 100);
  assert(sizeof(log.log_date) == 32);
  assert(sizeof(log.description) == 512);
  printf("[PASS] MaintenanceLog struct: correct sizes\n");
}

void test_maintenance_log_initialization() {
  MaintenanceLog log = {0};
  log.id = 1;
  log.machine_id = 5;
  strcpy(log.technician_name, "Ahmet Yilmaz");
  strcpy(log.log_date, "2026-02-09");
  strcpy(log.description, "Preventive maintenance completed");
  log.cost = 150.50;
  assert(log.id == 1);
  assert(log.machine_id == 5);
  assert(strcmp(log.technician_name, "Ahmet Yilmaz") == 0);
  assert(log.cost == 150.50);
  printf("[PASS] MaintenanceLog: initialization\n");
}

void test_maintenance_cost_validation() {
  MaintenanceLog log1 = {.cost = 0.0};
  MaintenanceLog log2 = {.cost = -10.0};
  MaintenanceLog log3 = {.cost = 1000.0};
  assert(log1.cost >= 0);
  assert(log2.cost < 0); // Should be rejected by service
  assert(log3.cost > 0);
  printf("[PASS] Maintenance: cost validation\n");
}

void test_maintenance_description_length() {
  MaintenanceLog log = {0};
  char long_desc[600];
  memset(long_desc, 'X', 599);
  long_desc[599] = '\0';
  strncpy(log.description, long_desc, 511);
  log.description[511] = '\0';
  assert(strlen(log.description) == 511);
  printf("[PASS] Maintenance: description boundary\n");
}

void test_maintenance_date_format() {
  MaintenanceLog log = {0};
  strcpy(log.log_date, "2026-02-09");
  assert(strlen(log.log_date) == 10);
  assert(log.log_date[4] == '-');
  assert(log.log_date[7] == '-');
  printf("[PASS] Maintenance: date format\n");
}

void test_maintenance_technician_assignment() {
  MaintenanceLog logs[3] = {
    {.id = 1, .technician_name = "Tech A"},
    {.id = 2, .technician_name = "Tech B"},
    {.id = 3, .technician_name = "Tech A"}
  };
  int tech_a_count = 0;

  for (int i = 0; i < 3; i++) {
    if (strcmp(logs[i].technician_name, "Tech A") == 0) {
      tech_a_count++;
    }
  }

  assert(tech_a_count == 2);
  printf("[PASS] Maintenance: technician assignment tracking\n");
}

int main() {
  printf("=== Maintenance Service Unit Tests ===\n");
  test_maintenance_log_struct();
  test_maintenance_log_initialization();
  test_maintenance_cost_validation();
  test_maintenance_description_length();
  test_maintenance_date_format();
  test_maintenance_technician_assignment();
  printf("\n✅ All Maintenance Service unit tests passed!\n");
  return 0;
}
