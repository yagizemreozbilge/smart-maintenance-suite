/* This file is a template for graph.c. Content will be filled by yagiz on 2025-12-29. */
#include <stdio.h>
#include "graph.h"

void initGraph(Graph *g) {
  g->count = 0;

  // Initialize Matrix with 0s
  for(int i = 0; i < MAX_MACHINES; i++) {
    g->machineIds[i] = 0;

    for(int j = 0; j < MAX_MACHINES; j++) {
      g->adjMatrix[i][j] = 0;
    }
  }
}

// Helper to find internal index from Machine ID
static int getIndex(Graph *g, int machine_id) {
  for(int i = 0; i < g->count; i++) {
    if(g->machineIds[i] == machine_id) {
      return i;
    }
  }

  return -1;
}

bool addMachineNode(Graph *g, int machine_id) {
  if(g->count >= MAX_MACHINES) {
    printf("ERROR: Graph is full! Cannot add Machine %d\n", machine_id);
    return false;
  }

  // Check duplicate
  if(getIndex(g, machine_id) != -1) {
    // Already exists
    return false;
  }

  g->machineIds[g->count] = machine_id;
  g->count++;
  return true;
}

bool addDependency(Graph *g, int from_id, int to_id) {
  int u = getIndex(g, from_id);
  int v = getIndex(g, to_id);

  if(u == -1 || v == -1) {
    printf("ERROR: Machine IDs not found in graph.\n");
    return false;
  }

  // Directed Edge: u depends on v
  g->adjMatrix[u][v] = 1;
  // If user wants bidirectional (mutual), they call this function twice reversed.
  // Or we could enable this logic here if explicitly requested.
  printf("Dependency Added: Machine %d -> Depends on -> Machine %d\n", from_id, to_id);
  return true;
}

bool hasDependency(Graph *g, int from_id, int to_id) {
  int u = getIndex(g, from_id);
  int v = getIndex(g, to_id);

  if(u == -1 || v == -1) return false;

  return (g->adjMatrix[u][v] == 1);
}

void printDependencies(Graph *g, int machine_id) {
  int u = getIndex(g, machine_id);

  if(u == -1) {
    printf("Machine %d not found.\n", machine_id);
    return;
  }

  printf("Machine %d depends on:\n", machine_id);
  bool found = false;

  for(int v = 0; v < g->count; v++) {
    if(g->adjMatrix[u][v] == 1) {
      printf(" - Machine %d\n", g->machineIds[v]);
      found = true;
    }
  }

  if(!found) {
    printf(" - (No Dependencies, Independent Machine)\n");
  }
}
