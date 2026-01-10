#include <stdio.h>
#include <assert.h>
#include "../../core/data_structures/stack.h"

void test_stack_init() {
  Stack s;
  initStack(&s);
  assert(s.top == 0);
  assert(s.count == 0);
  printf("Stack Init Test Passed\n");
}

void test_stack_push_pop() {
  Stack s;
  initStack(&s);
  Operation op1 = {1, 101, OP_FAULT_ENTRY, 0.0, "2025-01-01"};
  Operation op2 = {2, 102, OP_PART_REPLACEMENT, 1.0, "2025-01-02"};
  push(&s, op1);
  assert(getUndoCount(&s) == 1);
  push(&s, op2);
  assert(getUndoCount(&s) == 2);
  Operation out;
  assert(pop(&s, &out) == true);
  assert(out.operation_id == 2);
  assert(getUndoCount(&s) == 1);
  assert(pop(&s, &out) == true);
  assert(out.operation_id == 1);
  assert(getUndoCount(&s) == 0);
  assert(pop(&s, &out) == false);
  printf("Stack Push/Pop Test Passed\n");
}

void run_stack_tests() {
  printf("Running Stack Tests...\n");
  test_stack_init();
  test_stack_push_pop();
  printf("All Stack Tests Passed!\n");
}
