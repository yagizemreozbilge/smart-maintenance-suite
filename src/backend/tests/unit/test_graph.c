#include <stdio.h>
#include <assert.h>
#include "../../core/data_structures/graph.h"

void test_graph_init() {
  Graph g;
  initGraph(&g);
  assert(g.count == 0);
  printf("Graph Init Test Passed\n");
}

void test_graph_ops() {
  Graph g;
  initGraph(&g);
  // Add machines (Nodes)
  assert(addMachineNode(&g, 100) == true);
  assert(addMachineNode(&g, 200) == true);
  assert(addMachineNode(&g, 300) == true);
  // Dependencies: 100 -> 200 (100 depends on 200)
  assert(addDependency(&g, 100, 200) == true);
  assert(hasDependency(&g, 100, 200) == true);
  assert(hasDependency(&g, 200, 100) == false);
  // Remove dependency
  assert(removeDependency(&g, 100, 200) == true);
  assert(hasDependency(&g, 100, 200) == false);
  printf("Graph Ops Test Passed\n");
}

void run_graph_tests() {
  printf("Running Graph Tests...\n");
  test_graph_init();
  test_graph_ops();
  printf("All Graph Tests Passed!\n");
}
