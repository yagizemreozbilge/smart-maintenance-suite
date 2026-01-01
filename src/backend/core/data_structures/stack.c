/* This file is a template for stack.c. Content will be filled by yagiz on 2025-12-29. */
#include <stdio.h>
#include "stack.h"

void initStack(Stack *s) {
  s->top = 0;   // Start index
  s->count = 0; // No operations to undo yet
}

// ADD NEW OPERATION (Circular Overwrite Logic)
void push(Stack *s, Operation op) {
  // 1. Write new element to "top"
  s->history[s->top] = op;
  printf("Operation Added (ID: %d) -> Stack Index: %d\n", op.operation_id, s->top);
  // 2. Advance top (Circular: 0, 1, 2, 3, 4 -> 0)
  s->top = (s->top + 1) % MAX_UNDO_LEVEL;

  // 3. Increment count if not full.
  // If full (count == 5), do not increment.
  // Because we overwrote the oldest data, total count remains 5.
  if (s->count < MAX_UNDO_LEVEL) {
    s->count++;
  }
}

// UNDO OPERATION
bool pop(Stack *s, Operation *outOp) {
  if (s->count == 0) {
    printf("ERROR: No operation to undo! (Stack Empty)\n");
    return false;
  }

  // 1. Move Top BACK
  // WARNING: Negative modulo is tricky in C.
  // Formula: (top - 1 + MAX) % MAX
  s->top = (s->top - 1 + MAX_UNDO_LEVEL) % MAX_UNDO_LEVEL;
  // 2. Retrieve data
  *outOp = s->history[s->top];
  // 3. Decrement count
  s->count--;
  printf("Operation Undone: ID %d\n", outOp->operation_id);
  return true;
}

int getUndoCount(Stack *s) {
  return s->count;
}
