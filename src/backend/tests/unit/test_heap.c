#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "../../core/data_structures/heap.h"

void test_heap_init() {
  Heap h;
  initHeap(&h);
  assert(h.count == 0);
  assert(getHeapSize(&h) == 0);
  printf("[PASS] test_heap_init\n");
}

void test_heap_insert_extract() {
  Heap h;
  initHeap(&h);
  Task t1 = {1, 10, 1000};
  Task t2 = {2, 50, 1001}; // Highest priority
  Task t3 = {3, 30, 1002};
  assert(insertHeap(&h, t1) == true);
  assert(insertHeap(&h, t2) == true);
  assert(insertHeap(&h, t3) == true);
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
  // Empty extract
  assert(extractMax(&h, &out) == false);
  printf("[PASS] test_heap_insert_extract\n");
}

void test_heap_priority_updates() {
  Heap h;
  initHeap(&h);
  Task t1 = {1, 10, 1000};
  Task t2 = {2, 20, 1001};
  Task t3 = {3, 30, 1002};
  insertHeap(&h, t1);
  insertHeap(&h, t2);
  insertHeap(&h, t3);
  // Root is t3 (30). Update t1 (10) -> 40.
  assert(updatePriority(&h, 1, 40) == true);
  Task out;
  extractMax(&h, &out);
  assert(out.id == 1); // t1 moved to top
  // Now root is t3 (30). Update t3 (30) -> 5.
  assert(updatePriority(&h, 3, 5) == true);
  extractMax(&h, &out);
  assert(out.id == 2); // t2 (20) moved to top
  // Update non-existent
  assert(updatePriority(&h, 999, 100) == false);
  printf("[PASS] test_heap_priority_updates\n");
}

void test_heap_tie_breaker() {
  Heap h;
  initHeap(&h);
  // Same priority, t1 is older (smaller timestamp)
  Task t1 = {1, 50, 1000};
  Task t2 = {2, 50, 2000};
  insertHeap(&h, t2);
  insertHeap(&h, t1); // Order shouldn't matter for insert
  Task out;
  extractMax(&h, &out);
  assert(out.id == 1); // Older wins tie-break (smaller timestamp)
  extractMax(&h, &out);
  assert(out.id == 2);
  printf("[PASS] test_heap_tie_breaker\n");
}

void test_heap_edge_cases() {
  Heap h;
  initHeap(&h);

  // Fill heap
  for (int i = 0; i < MAX_HEAP_SIZE; i++) {
    Task t = {i, 10, 1000};
    assert(insertHeap(&h, t) == true);
  }

  // Try to overflow
  Task over = {999, 99, 999};
  assert(insertHeap(&h, over) == false);
  printf("[PASS] test_heap_edge_cases\n");
}

int main() {
  printf("=== Heap Logic Unit Tests ===\n");
  test_heap_init();
  test_heap_insert_extract();
  test_heap_priority_updates();
  test_heap_tie_breaker();
  test_heap_edge_cases();
  printf("\n✅ All Heap tests passed!\n");
  return 0;
}
