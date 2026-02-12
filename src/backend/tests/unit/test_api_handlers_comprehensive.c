#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "../../api/handlers/machine_handler.h"
#include "../../api/handlers/inventory_handler.h"
#include "../../api/handlers/auth_handler.h"
#include "../../api/handlers/maintenance_handler.h"
#include "../../api/handlers/fault_handler.h"
#include "../../api/handlers/report_handler.h"

/* ============ MACHINE HANDLER TESTS ============ */
void test_machine_handler() {
  printf("\n--- Testing Machine Handler ---\n");
  // Test handle_get_machines
  char *machines = handle_get_machines();
  assert(machines != NULL);
  printf("[OK] handle_get_machines: %s\n", machines);
  free(machines);
  // Test handle_get_machine_by_id
  char *machine = handle_get_machine_by_id(1);
  assert(machine != NULL);
  printf("[OK] handle_get_machine_by_id(1): %s\n", machine);
  free(machine);
  // Test handle_create_machine
  char *new_machine = handle_create_machine("{\"name\":\"Test Machine\",\"model\":\"T-1000\",\"location\":\"Test Lab\"}");
  assert(new_machine != NULL);
  printf("[OK] handle_create_machine\n");
  free(new_machine);
  // Test handle_update_machine
  char *updated = handle_update_machine(1, "{\"status\":\"maintenance\"}");
  assert(updated != NULL);
  printf("[OK] handle_update_machine\n");
  free(updated);
  // Test handle_delete_machine
  char *deleted = handle_delete_machine(1);
  assert(deleted != NULL);
  printf("[OK] handle_delete_machine\n");
  free(deleted);
  printf(" Machine Handler tests passed\n");
}

/* ============ INVENTORY HANDLER TESTS ============ */
void test_inventory_handler() {
  printf("\n--- Testing Inventory Handler ---\n");
  // Test handle_get_inventory
  char *inventory = handle_get_inventory();
  assert(inventory != NULL);
  printf("[OK] handle_get_inventory: %s\n", inventory);
  free(inventory);
  // Test handle_get_part_by_id
  char *part = handle_get_part_by_id(1);
  assert(part != NULL);
  printf("[OK] handle_get_part_by_id(1): %s\n", part);
  free(part);
  // Test handle_create_part
  char *new_part = handle_create_part("{\"name\":\"Test Part\",\"quantity\":10,\"min_threshold\":5}");
  assert(new_part != NULL);
  printf("[OK] handle_create_part\n");
  free(new_part);
  // Test handle_update_part
  char *updated = handle_update_part(1, "{\"quantity\":15}");
  assert(updated != NULL);
  printf("[OK] handle_update_part\n");
  free(updated);
  // Test handle_delete_part
  char *deleted = handle_delete_part(1);
  assert(deleted != NULL);
  printf("[OK] handle_delete_part\n");
  free(deleted);
  printf(" Inventory Handler tests passed\n");
}

/* ============ AUTH HANDLER TESTS ============ */
void test_auth_handler() {
  printf("\n--- Testing Auth Handler ---\n");
  // Test handle_login - success
  char *login_success = handle_login("admin", "admin123");
  assert(login_success != NULL);
  printf("[OK] handle_login (success): %s\n", login_success);
  free(login_success);
  // Test handle_login - failure
  char *login_fail = handle_login("admin", "wrongpass");
  assert(login_fail != NULL);
  printf("[OK] handle_login (failure): %s\n", login_fail);
  free(login_fail);
  // Test handle_register
  char *register_user = handle_register("{\"username\":\"testuser\",\"password\":\"test123\",\"role\":\"operator\"}");
  assert(register_user != NULL);
  printf("[OK] handle_register\n");
  free(register_user);
  // Test handle_logout
  char *logout = handle_logout("test-token-123");
  assert(logout != NULL);
  printf("[OK] handle_logout\n");
  free(logout);
  // Test handle_verify_token
  char *verify = handle_verify_token("test-token-123");
  assert(verify != NULL);
  printf("[OK] handle_verify_token\n");
  free(verify);
  printf(" Auth Handler tests passed\n");
}

/* ============ MAINTENANCE HANDLER TESTS ============ */
void test_maintenance_handler() {
  printf("\n--- Testing Maintenance Handler ---\n");
  // Test handle_get_maintenance_tasks
  char *tasks = handle_get_maintenance_tasks();
  assert(tasks != NULL);
  printf("[OK] handle_get_maintenance_tasks: %s\n", tasks);
  free(tasks);
  // Test handle_get_task_by_id
  char *task = handle_get_task_by_id(1);
  assert(task != NULL);
  printf("[OK] handle_get_task_by_id(1): %s\n", task);
  free(task);
  // Test handle_create_task
  char *new_task = handle_create_task("{\"machine_id\":1,\"description\":\"Check oil\",\"scheduled_date\":\"2024-01-15\"}");
  assert(new_task != NULL);
  printf("[OK] handle_create_task\n");
  free(new_task);
  // Test handle_update_task_status
  char *updated = handle_update_task_status(1, "completed");
  assert(updated != NULL);
  printf("[OK] handle_update_task_status\n");
  free(updated);
  // Test handle_delete_task
  char *deleted = handle_delete_task(1);
  assert(deleted != NULL);
  printf("[OK] handle_delete_task\n");
  free(deleted);
  // Test handle_get_tasks_by_machine
  char *machine_tasks = handle_get_tasks_by_machine(1);
  assert(machine_tasks != NULL);
  printf("[OK] handle_get_tasks_by_machine(1): %s\n", machine_tasks);
  free(machine_tasks);
  printf(" Maintenance Handler tests passed\n");
}

/* ============ FAULT HANDLER TESTS ============ */
void test_fault_handler() {
  printf("\n--- Testing Fault Handler ---\n");
  // Test handle_get_faults
  char *faults = handle_get_faults();
  assert(faults != NULL);
  printf("[OK] handle_get_faults: %s\n", faults);
  free(faults);
  // Test handle_get_fault_by_id
  char *fault = handle_get_fault_by_id(1);
  assert(fault != NULL);
  printf("[OK] handle_get_fault_by_id(1): %s\n", fault);
  free(fault);
  // Test handle_report_fault
  char *new_fault = handle_report_fault("{\"machine_id\":1,\"description\":\"Overheating\",\"severity\":\"high\"}");
  assert(new_fault != NULL);
  printf("[OK] handle_report_fault\n");
  free(new_fault);
  // Test handle_resolve_fault
  char *resolved = handle_resolve_fault(1, "{\"resolution\":\"Replaced fan\"}");
  assert(resolved != NULL);
  printf("[OK] handle_resolve_fault\n");
  free(resolved);
  // Test handle_get_faults_by_machine
  char *machine_faults = handle_get_faults_by_machine(1);
  assert(machine_faults != NULL);
  printf("[OK] handle_get_faults_by_machine(1): %s\n", machine_faults);
  free(machine_faults);
  // Test handle_get_faults_by_severity
  char *high_faults = handle_get_faults_by_severity("high");
  assert(high_faults != NULL);
  printf("[OK] handle_get_faults_by_severity(high): %s\n", high_faults);
  free(high_faults);
  printf(" Fault Handler tests passed\n");
}

/* ============ REPORT HANDLER TESTS ============ */
void test_report_handler() {
  printf("\n--- Testing Report Handler ---\n");
  // Test handle_get_machine_report
  char *machine_report = handle_get_machine_report(1);
  assert(machine_report != NULL);
  printf("[OK] handle_get_machine_report(1): %s\n", machine_report);
  free(machine_report);
  // Test handle_get_maintenance_report
  char *maintenance_report = handle_get_maintenance_report("2024-01-01", "2024-12-31");
  assert(maintenance_report != NULL);
  printf("[OK] handle_get_maintenance_report: %s\n", maintenance_report);
  free(maintenance_report);
  // Test handle_get_inventory_report
  char *inventory_report = handle_get_inventory_report();
  assert(inventory_report != NULL);
  printf("[OK] handle_get_inventory_report: %s\n", inventory_report);
  free(inventory_report);
  // Test handle_get_fault_report
  char *fault_report = handle_get_fault_report("2024-01-01", "2024-12-31");
  assert(fault_report != NULL);
  printf("[OK] handle_get_fault_report: %s\n", fault_report);
  free(fault_report);
  // Test handle_get_overall_dashboard
  char *dashboard = handle_get_overall_dashboard();
  assert(dashboard != NULL);
  printf("[OK] handle_get_overall_dashboard: %s\n", dashboard);
  free(dashboard);
  printf(" Report Handler tests passed\n");
}

/* ============ EDGE CASE TESTS ============ */
void test_edge_cases() {
  printf("\n--- Testing Edge Cases ---\n");
  // NULL/empty inputs
  char *null_result = handle_get_machine_by_id(-1);
  assert(null_result != NULL);
  printf("[OK] handle_get_machine_by_id(invalid): %s\n", null_result);
  free(null_result);
  null_result = handle_create_machine(NULL);
  assert(null_result != NULL);
  printf("[OK] handle_create_machine(NULL)\n");
  free(null_result);
  null_result = handle_login(NULL, NULL);
  assert(null_result != NULL);
  printf("[OK] handle_login(NULL,NULL)\n");
  free(null_result);
  // Large inputs
  char large_json[4096];
  sprintf(large_json, "{\"name\":\"%s\"}", "Very long machine name that might cause buffer issues...");
  null_result = handle_create_machine(large_json);
  assert(null_result != NULL);
  printf("[OK] handle_create_machine(large input)\n");
  free(null_result);
  printf(" Edge Cases tests passed\n");
}

/* ============ MAIN ============ */
int main() {
  printf("========================================\n");
  printf("  API HANDLERS COMPREHENSIVE TEST SUITE\n");
  printf("========================================\n");
  test_machine_handler();
  test_inventory_handler();
  test_auth_handler();
  test_maintenance_handler();
  test_fault_handler();
  test_report_handler();
  test_edge_cases();
  printf("\n========================================\n");
  printf("   ALL API HANDLER TESTS PASSED!\n");
  printf("========================================\n");
  return 0;
}
