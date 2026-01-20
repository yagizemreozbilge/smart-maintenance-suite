#include <stdio.h>

// External test functions
extern void run_queue_tests();
extern void run_stack_tests();
extern void run_heap_tests();
extern void run_bst_tests();
extern void run_graph_tests();
extern void run_db_pool_tests();

int main() {
  printf("=================================\n");
  printf("    STARTING UNIT TESTS          \n");
  printf("=================================\n");
  run_queue_tests();
  run_stack_tests();
  run_heap_tests();
  run_bst_tests();
  run_graph_tests();
  run_db_pool_tests();
  printf("=================================\n");
  printf("    ALL UNIT TESTS PASSED        \n");
  printf("=================================\n");
  return 0;
}
