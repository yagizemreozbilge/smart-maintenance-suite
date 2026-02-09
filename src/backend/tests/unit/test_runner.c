#include <stdio.h>
#include <stdlib.h>

// External test function declarations
extern int main_test_queue();
extern int main_test_stack();
extern int main_test_heap();
extern int main_test_bst();
extern int main_test_graph();
extern int main_test_rbac();
extern int main_test_jwt();
extern int main_test_machine_service();
extern int main_test_inventory_service();
extern int main_test_maintenance_service();
extern int main_test_api_handlers();

typedef struct {
  const char *name;
  int (*test_func)();
} TestSuite;

int main() {
  printf("╔════════════════════════════════════════════════════════╗\n");
  printf("║  SMART MAINTENANCE SUITE - COMPREHENSIVE TEST RUNNER  ║\n");
  printf("╚════════════════════════════════════════════════════════╝\n\n");
  TestSuite suites[] = {
    {"Data Structures - Queue", main_test_queue},
    {"Data Structures - Stack", main_test_stack},
    {"Data Structures - Heap", main_test_heap},
    {"Data Structures - BST", main_test_bst},
    {"Data Structures - Graph", main_test_graph},
    {"Security - RBAC", main_test_rbac},
    {"Security - JWT", main_test_jwt},
    {"Database - Machine Service", main_test_machine_service},
    {"Database - Inventory Service", main_test_inventory_service},
    {"Database - Maintenance Service", main_test_maintenance_service},
    {"API - Handlers", main_test_api_handlers}
  };
  int total_suites = sizeof(suites) / sizeof(TestSuite);
  int passed = 0;
  int failed = 0;

  for (int i = 0; i < total_suites; i++) {
    printf("\n┌─────────────────────────────────────────────────────────┐\n");
    printf("│ Running: %-47s │\n", suites[i].name);
    printf("└─────────────────────────────────────────────────────────┘\n");
    int result = suites[i].test_func();

    if (result == 0) {
      passed++;
      printf("✅ %s: PASSED\n", suites[i].name);
    } else {
      failed++;
      printf("❌ %s: FAILED\n", suites[i].name);
    }
  }

  printf("\n╔════════════════════════════════════════════════════════╗\n");
  printf("║                    TEST SUMMARY                        ║\n");
  printf("╠════════════════════════════════════════════════════════╣\n");
  printf("║  Total Test Suites: %-2d                                ║\n", total_suites);
  printf("║  Passed: %-2d                                           ║\n", passed);
  printf("║  Failed: %-2d                                           ║\n", failed);
  printf("║  Success Rate: %.1f%%                                  ║\n",
         (passed * 100.0) / total_suites);
  printf("╚════════════════════════════════════════════════════════╝\n");
  return (failed == 0) ? 0 : 1;
}

// Wrapper functions to match expected signature
int main_test_queue() {
  extern int main();
  return main();
}

int main_test_stack() {
  extern int main();
  return main();
}

int main_test_heap() {
  extern int main();
  return main();
}

int main_test_bst() {
  extern int main();
  return main();
}

int main_test_graph() {
  extern int main();
  return main();
}

int main_test_rbac() {
  extern int main();
  return main();
}

int main_test_jwt() {
  extern int main();
  return main();
}

int main_test_machine_service() {
  extern int main();
  return main();
}

int main_test_inventory_service() {
  extern int main();
  return main();
}

int main_test_maintenance_service() {
  extern int main();
  return main();
}

int main_test_api_handlers() {
  extern int main();
  return main();
}
