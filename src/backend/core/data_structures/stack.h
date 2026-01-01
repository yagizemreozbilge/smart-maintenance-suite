/* This file is a template for stack.h. Content will be filled by yagiz on 2025-12-29. */
#ifndef STACK_H
#define STACK_H

#include <stdbool.h>
#include "../../common/types.h"

// Max Undo Limit (User decision: 5)
#define MAX_UNDO_LEVEL 5

typedef struct {
  // Circular Buffer (Circular Undo Buffer)
  Operation history[MAX_UNDO_LEVEL];

  int top;    // Index of the next free slot
  int count;  // Current number of undoable operations
} Stack;

// Functions
void initStack(Stack *s);
void push(Stack *s, Operation op); // Void because we overwrite old data, never "Full" error.
bool pop(Stack *s, Operation *outOp); // Undo operation
int getUndoCount(Stack *s);

#endif
