/* This file is a template for heap.h. Content will be filled by yagiz on 2025-12-29. */
#ifndef HEAP_H
#define HEAP_H

#include <stdbool.h>
#include "../common/types.h"

#define MAX_HEAP_SIZE 100

typedef struct {
  Task data[MAX_HEAP_SIZE];
  int count; // Current number of tasks
} Heap;

// Functions
void initHeap(Heap *h);
bool insertHeap(Heap *h, Task task);
bool extractMax(Heap *h, Task *outTask);
bool updatePriority(Heap *h, int task_id, int new_priority);
int getHeapSize(Heap *h);

#endif
