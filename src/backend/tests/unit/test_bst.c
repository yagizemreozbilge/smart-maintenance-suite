#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../../core/data_structures/bst.h"

void test_bst_init() {
  BST tree;
  initBST(&tree);
  assert(tree.root == NULL);
  assert(tree.count == 0);
  printf("BST Init Test Passed\n");
}

void test_bst_insert_search() {
  BST tree;
  initBST(&tree);
  Machine m1 = {101, "M1", "L1", "2025-01-01", 30};
  Machine m2 = {100, "M2", "L2", "2025-01-02", 30};
  Machine m3 = {102, "M3", "L3", "2025-01-03", 30};
  assert(insertMachine(&tree, m1) == true);
  assert(tree.count == 1);
  assert(insertMachine(&tree, m2) == true);
  assert(tree.count == 2);
  assert(insertMachine(&tree, m3) == true);
  assert(tree.count == 3);
  // Duplicate ID should fail
  assert(insertMachine(&tree, m1) == false);
  assert(tree.count == 3);
  Machine *res = searchMachine(&tree, 102);
  assert(res != NULL);
  assert(strcmp(res->name, "M3") == 0);
  res = searchMachine(&tree, 999);
  assert(res == NULL);
  destroyBST(&tree);
  printf("BST Insert/Search Test Passed\n");
}

void run_bst_tests() {
  printf("Running BST Tests...\n");
  test_bst_init();
  test_bst_insert_search();
  printf("All BST Tests Passed!\n");
}

int main() {
  run_bst_tests();
  return 0;
}
