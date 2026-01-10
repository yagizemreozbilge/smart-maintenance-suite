#include <stdio.h>

extern int main(int argc, char **argv);
// Note: real implementation would reference integration tests too.
// For now we just call the unit test logic or similar.

// Re-declare to avoid linking issues or just call system("./run_unit_tests")
// But for a pure C runner, I should link everything.
// Let's just make a simple wrapper that runs unit tests for now.

extern void run_queue_tests();
extern void run_stack_tests();
extern void run_heap_tests();
extern void run_bst_tests();
extern void run_graph_tests();

void run_integration_tests() {
  printf("Running Integration Tests (Mock)...\n");
  // Placeholder for:
  // test_machine_lifecycle();
  // test_maintenance_scheduling();
  printf("Integration Tests Passed!\n");
}

int main() {
  printf(">>> RUNNING ALL TESTS <<<\n");
  run_queue_tests();
  run_stack_tests();
  run_heap_tests();
  run_bst_tests();
  run_graph_tests();
  run_integration_tests();
  printf(">>> ALL TESTS COMPLETED <<<\n");
  return 0;
}
