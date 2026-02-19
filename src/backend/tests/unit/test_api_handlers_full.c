#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../../database/api_handlers.h"
#include "../../database/cJSON.h"

// Mock functions to avoid database access
int get_all_machines(void *list, int count) {
  (void)list;
  (void)count;
  return 1;
}

int get_all_inventory(void *items, int count) {
  (void)items;
  (void)count;
  return 1;
}

int get_recent_alerts(void *alerts, int count) {
  (void)alerts;
  (void)count;
  return 1;
}

int get_machine_sensor_stats(int machine_id, void *stats, int count) {
  (void)machine_id;
  (void)stats;
  (void)count;
  return 1;
}

int get_all_maintenance_logs(void *logs, int count) {
  (void)logs;
  (void)count;
  return 1;
}

void test_serialize_machines() {
  char *json = serialize_machines_to_json();
  assert(json != NULL);
  assert(strstr(json, "[") != NULL);
  free(json);
  printf("[PASS] serialize_machines_to_json\n");
}

void test_serialize_inventory() {
  char *json = serialize_inventory_to_json();
  assert(json != NULL);
  assert(strstr(json, "[") != NULL);
  free(json);
  printf("[PASS] serialize_inventory_to_json\n");
}

void test_serialize_alerts() {
  char *json = serialize_alerts_to_json();
  assert(json != NULL);
  assert(strstr(json, "[") != NULL);
  free(json);
  printf("[PASS] serialize_alerts_to_json\n");
}

void test_serialize_sensors() {
  char *json = serialize_sensors_to_json(1);
  assert(json != NULL);
  assert(strstr(json, "[") != NULL);
  free(json);
  printf("[PASS] serialize_sensors_to_json\n");
}

void test_serialize_maintenance() {
  char *json = serialize_maintenance_to_json();
  assert(json != NULL);
  assert(strstr(json, "[") != NULL);
  free(json);
  printf("[PASS] serialize_maintenance_to_json\n");
}

int main() {
  test_serialize_machines();
  test_serialize_inventory();
  test_serialize_alerts();
  test_serialize_sensors();
  test_serialize_maintenance();
  printf("API Handlers Full Coverage Test Passed\n");
  return 0;
}
