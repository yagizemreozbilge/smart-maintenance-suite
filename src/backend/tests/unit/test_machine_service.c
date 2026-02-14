#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "machine_service.h"

void test_get_all_machines_success() {
  Machine machines[5];
  int count = get_all_machines(machines, 5);
  assert(count == 1);
  assert(machines[0].id == 1);
  assert(strcmp(machines[0].name, "CNC-01") == 0);
  assert(strcmp(machines[0].model, "X100") == 0);
  assert(strcmp(machines[0].location, "Line A") == 0);
  assert(strcmp(machines[0].status, "active") == 0);
  assert(machines[0].health_score == 88.5);
  printf(" test_get_all_machines_success passed\n");
}

void test_get_all_machines_null_pointer() {
  int count = get_all_machines(NULL, 5);
  assert(count == 0);
  printf(" test_get_all_machines_null_pointer passed\n");
}

void test_get_all_machines_zero_max() {
  Machine machines[5];
  int count = get_all_machines(machines, 0);
  assert(count == 0);
  printf(" test_get_all_machines_zero_max passed\n");
}

void test_get_machine_by_id_success() {
  Machine machine;
  int result = get_machine_by_id(10, &machine);
  assert(result == 1);
  assert(machine.id == 10);
  assert(strcmp(machine.name, "Mock Machine") == 0);
  assert(strcmp(machine.model, "Mock Model") == 0);
  assert(strcmp(machine.location, "Test Lab") == 0);
  assert(strcmp(machine.status, "active") == 0);
  assert(machine.health_score == 90.0);
  printf(" test_get_machine_by_id_success passed\n");
}

void test_get_machine_by_id_null() {
  int result = get_machine_by_id(10, NULL);
  assert(result == 0);
  printf(" test_get_machine_by_id_null passed\n");
}

void test_get_machine_health_score() {
  double score = get_machine_health_score(1);
  assert(score == 88.5);
  printf(" test_get_machine_health_score passed\n");
}

int main() {
  test_get_all_machines_success();
  test_get_all_machines_null_pointer();
  test_get_all_machines_zero_max();
  test_get_machine_by_id_success();
  test_get_machine_by_id_null();
  test_get_machine_health_score();
  printf("\n All machine service tests passed!\n");
  return 0;
}
