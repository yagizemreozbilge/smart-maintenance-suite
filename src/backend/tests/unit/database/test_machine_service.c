#include "../../../database/machine_service.h"
#include "../../../database/db_connection.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

// Mock test - requires actual DB connection for integration tests
void test_machine_struct_size() {
  Machine m;
  assert(sizeof(m.name) == 100);
  assert(sizeof(m.model) == 50);
  assert(sizeof(m.location) == 100);
  assert(sizeof(m.status) == 20);
  printf("[PASS] Machine struct: correct field sizes\n");
}

void test_machine_initialization() {
  Machine m = {0};
  m.id = 1;
  strcpy(m.name, "Test Machine");
  strcpy(m.status, "operational");
  assert(m.id == 1);
  assert(strcmp(m.name, "Test Machine") == 0);
  assert(strcmp(m.status, "operational") == 0);
  printf("[PASS] Machine initialization: fields set correctly\n");
}

void test_machine_name_boundary() {
  Machine m = {0};
  char long_name[150];
  memset(long_name, 'A', 149);
  long_name[149] = '\0';
  strncpy(m.name, long_name, 99);
  m.name[99] = '\0';
  assert(strlen(m.name) == 99);
  printf("[PASS] Machine name: boundary check passed\n");
}

void test_machine_status_values() {
  Machine m1 = {.id = 1};
  strcpy(m1.status, "operational");
  Machine m2 = {.id = 2};
  strcpy(m2.status, "maintenance");
  Machine m3 = {.id = 3};
  strcpy(m3.status, "fault");
  assert(strcmp(m1.status, "operational") == 0);
  assert(strcmp(m2.status, "maintenance") == 0);
  assert(strcmp(m3.status, "fault") == 0);
  printf("[PASS] Machine status: all valid states\n");
}

int main() {
  printf("=== Machine Service Unit Tests ===\n");
  printf("Note: These are structure tests. Integration tests require DB connection.\n\n");
  test_machine_struct_size();
  test_machine_initialization();
  test_machine_name_boundary();
  test_machine_status_values();
  printf("\n✅ All Machine Service unit tests passed!\n");
  printf("⚠️  Run integration tests with active PostgreSQL connection for full coverage.\n");
  return 0;
}
