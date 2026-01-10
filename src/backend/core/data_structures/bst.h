/* This file is a template for bst.h. Content will be filled by yagiz on 2025-12-29. */
#ifndef BST_H
#define BST_H

#include <stdbool.h>
#include "../common/types.h"

// Binary Search Tree Node
typedef struct BSTNode {
  Machine data;
  struct BSTNode *left;
  struct BSTNode *right;
} BSTNode;

typedef struct {
  BSTNode *root;
  int count;
} BST;

// Functions
void initBST(BST *tree);
bool insertMachine(BST *tree, Machine machine); // Returns false if duplicate ID
Machine *searchMachine(BST *tree, int machine_id); // Returns NULL if not found
void printInventory(BST *tree); // In-Order Traversal (Sorted List)

// Helper: Free memory (Since we use malloc here)
void destroyBST(BST *tree);

#endif
