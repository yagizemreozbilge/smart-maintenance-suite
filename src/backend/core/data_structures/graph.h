/* This file is a template for graph.h. Content will be filled by yagiz on 2025-12-29. */
#ifndef GRAPH_H
#define GRAPH_H

#include <stdbool.h>
#include "../common/types.h"

// Fixed max machines for Matrix implementation
#define MAX_MACHINES 100

typedef struct {
  // Adjacency Matrix [Row][Col]
  // 1: Row depends on Col (Row -> Col)
  // 0: No dependency
  int adjMatrix[MAX_MACHINES][MAX_MACHINES];

  // Mapping from Matrix Index (0-99) to Machine ID (real ID)
  int machineIds[MAX_MACHINES];
  int count; // Number of machines in the graph
} Graph;

// Functions
void initGraph(Graph *g);
bool addMachineNode(Graph *g, int machine_id); // Adds a machine to the graph
bool addDependency(Graph *g, int from_id, int to_id); // 'from' depends on 'to'
bool removeDependency(Graph *g, int from_id, int to_id);
bool hasDependency(Graph *g, int from_id, int to_id);

// Traversal / Analysis
void printDependencies(Graph *g, int machine_id); // Who depends on this machine?

#endif
