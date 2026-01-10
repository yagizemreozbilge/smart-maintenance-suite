/* This file is a template for bst.c. Content will be filled by yagiz on 2025-12-29. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bst.h"

void initBST(BST *tree) {
  tree->root = NULL;
  tree->count = 0;
}

// Helper: Create a new node
static BSTNode *createNode(Machine machine) {
  BSTNode *newNode = (BSTNode *)malloc(sizeof(BSTNode));

  if (newNode == NULL) {
    printf("CRITICAL ERROR: Memory allocation failed!\n");
    return NULL;
  }

  newNode->data = machine;
  newNode->left = NULL;
  newNode->right = NULL;
  return newNode;
}

// Helper: Recursive Insert
static bool insertRecursive(BSTNode **node, Machine machine) {
  // 1. Base Case: Found empty spot
  if (*node == NULL) {
    *node = createNode(machine);
    return true;
  }

  // 2. Recursive Step
  if (machine.machine_id < (*node)->data.machine_id) {
    return insertRecursive(&(*node)->left, machine);
  } else if (machine.machine_id > (*node)->data.machine_id) {
    return insertRecursive(&(*node)->right, machine);
  } else {
    // 3. Duplicate Case
    printf("ERROR: Machine ID %d already exists! duplicate rejected.\n", machine.machine_id);
    return false;
  }
}

bool insertMachine(BST *tree, Machine machine) {
  bool success = insertRecursive(&tree->root, machine);

  if (success) {
    tree->count++;
    printf("Inventory Added: %s (ID: %d)\n", machine.name, machine.machine_id);
  }

  return success;
}

// Helper: Recursive Search
static Machine *searchRecursive(BSTNode *node, int id) {
  // Base Case: Empty or Found
  if (node == NULL || node->data.machine_id == id) {
    return (node == NULL) ? NULL : &node->data;
  }

  // Navigation
  if (id < node->data.machine_id) {
    return searchRecursive(node->left, id);
  } else {
    return searchRecursive(node->right, id);
  }
}

Machine *searchMachine(BST *tree, int machine_id) {
  Machine *result = searchRecursive(tree->root, machine_id);

  if (result) {
    printf("FOUND: %s in %s\n", result->name, result->location);
  } else {
    printf("NOT FOUND: Machine ID %d\n", machine_id);
  }

  return result;
}

// Helper: In-Order Traversal (L-N-R)
static void printRecursive(BSTNode *node) {
  if (node != NULL) {
    printRecursive(node->left);
    printf("[%d] %-20s | Location: %s\n",
           node->data.machine_id, node->data.name, node->data.location);
    printRecursive(node->right);
  }
}

void printInventory(BST *tree) {
  printf("\n--- FACTORY INVENTORY (Sorted by ID) ---\n");
  printRecursive(tree->root);
  printf("----------------------------------------\n");
}

static void destroyRecursive(BSTNode *node) {
  if (node != NULL) {
    destroyRecursive(node->left);
    destroyRecursive(node->right);
    free(node);
  }
}

void destroyBST(BST *tree) {
  destroyRecursive(tree->root);
  tree->root = NULL;
  tree->count = 0;
}
