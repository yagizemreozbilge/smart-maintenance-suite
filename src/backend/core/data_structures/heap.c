/* This file is a template for heap.c. Content will be filled by yagiz on 2025-12-29. */
#include <stdio.h>
#include "heap.h"

// Helper: Bubble Up (Moves a node UP to its correct position)
static void bubbleUp(Heap *h, int index) {
  while (index > 0) {
    int parentIndex = (index - 1) / 2;
    // Max-Heap Logic: Parent should be greater.
    // Tie-Breaker: If Priorities are EQUAL, Older Timestamp (smaller value) wins.
    bool parentIsSmaller = h->data[parentIndex].priority < h->data[index].priority;
    bool equalButNewer = (h->data[parentIndex].priority == h->data[index].priority) &&
                         (h->data[parentIndex].timestamp > h->data[index].timestamp);

    if (parentIsSmaller || equalButNewer) {
      // SWAP
      Task temp = h->data[parentIndex];
      h->data[parentIndex] = h->data[index];
      h->data[index] = temp;
      // Move up
      index = parentIndex;
    } else {
      break; // Order is correct
    }
  }
}

// Helper: Heapify / Bubble Down (Moves a node DOWN to its correct position)
static void heapify(Heap *h, int index) {
  int largest = index;
  int left = 2 * index + 1;
  int right = 2 * index + 2;

  // Check Left Child
  if (left < h->count) {
    bool leftIsBigger = h->data[left].priority > h->data[largest].priority;
    bool leftEqualButOlder = (h->data[left].priority == h->data[largest].priority) &&
                             (h->data[left].timestamp < h->data[largest].timestamp); // Smaller timestamp = Older

    if (leftIsBigger || leftEqualButOlder) {
      largest = left;
    }
  }

  // Check Right Child
  if (right < h->count) {
    bool rightIsBigger = h->data[right].priority > h->data[largest].priority;
    bool rightEqualButOlder = (h->data[right].priority == h->data[largest].priority) &&
                              (h->data[right].timestamp < h->data[largest].timestamp);

    if (rightIsBigger || rightEqualButOlder) {
      largest = right;
    }
  }

  // Swap and Recursive Call
  if (largest != index) {
    Task temp = h->data[index];
    h->data[index] = h->data[largest];
    h->data[largest] = temp;
    heapify(h, largest);
  }
}

void initHeap(Heap *h) {
  h->count = 0;
}

// INSERT
bool insertHeap(Heap *h, Task task) {
  if (h->count >= MAX_HEAP_SIZE) {
    printf("ERROR: Heap is full! Cannot add Task-%d\n", task.id);
    return false;
  }

  // 1. Add to end
  h->data[h->count] = task;
  // 2. Bubble Up
  bubbleUp(h, h->count);
  // 3. Increment Count
  h->count++;
  printf("Task Added: ID-%d [P:%d]\n", task.id, task.priority);
  return true;
}

// EXTRACT MAX
bool extractMax(Heap *h, Task *outTask) {
  if (h->count == 0) {
    printf("ERROR: Heap is empty!\n");
    return false;
  }

  // 1. Get Root (Max Priority)
  *outTask = h->data[0];
  // 2. Move last element to root
  h->data[0] = h->data[h->count - 1];
  h->count--;
  // 3. Heaterify (Bubble Down)
  heapify(h, 0);
  // printf("Task Extracted: ID-%d [P:%d]\n", outTask->id, outTask->priority);
  return true;
}

// UPDATE PRIORITY (Dynamic Update)
bool updatePriority(Heap *h, int task_id, int new_priority) {
  int index = -1;

  // 1. Find the task
  for(int i = 0; i < h->count; i++) {
    if(h->data[i].id == task_id) {
      index = i;
      break;
    }
  }

  if(index == -1) {
    printf("ERROR: Task-%d not found!\n", task_id);
    return false;
  }

  int old_priority = h->data[index].priority;
  h->data[index].priority = new_priority;
  printf("Task-%d Priority Updated: %d -> %d\n", task_id, old_priority, new_priority);

  // 2. Re-balance
  if (new_priority > old_priority) {
    bubbleUp(h, index);
  } else {
    heapify(h, index);
  }

  return true;
}

int getHeapSize(Heap *h) {
  return h->count;
}
