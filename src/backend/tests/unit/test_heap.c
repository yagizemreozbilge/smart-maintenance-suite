#include <stdio.h>
#include <assert.h>
#include "../../core/data_structures/heap.h"

void test_heap_init() {
  Heap h;
  initHeap(&h);
  assert(h.count == 0);
  printf("Heap Init Test Passed\n");
}

void test_heap_insert_extract() {
  Heap h;
  initHeap(&h);
  Task t1 = {1, 10, 1000};
  Task t2 = {2, 50, 1001}; // Higher priority
  Task t3 = {3, 30, 1002};
  insertHeap(&h, t1);
  insertHeap(&h, t2);
  insertHeap(&h, t3);
  assert(getHeapSize(&h) == 3);
  Task out;
  // Should get t2 (priority 50)
  assert(extractMax(&h, &out) == true);
  assert(out.id == 2);
  // Should get t3 (priority 30)
  assert(extractMax(&h, &out) == true);
  assert(out.id == 3);
  // Should get t1 (priority 10)
  assert(extractMax(&h, &out) == true);
  assert(out.id == 1);
  assert(extractMax(&h, &out) == false);
  printf("Heap Insert/Extract Test Passed\n");
}

void run_heap_tests() {
  printf("Running Heap Tests...\n");
  test_heap_init();
  test_heap_insert_extract();
  printf("All Heap Tests Passed!\n");
}
