//
// Advent of Code 2021
//
// Day 18
//
// Author: Simone Franza
//
// Thought process:
// Save the numbers in a binary tree and do all the operations
// I didn't free all the elements cause it's not relevant right now and I don'the
// have time
//

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "input"

typedef struct Node_ {
  struct Node_ *parent;
  struct Node_ *left;
  struct Node_ *right;
  size_t value;
} Node;

Node **getTrees();
Node *readTree(char *str, size_t len);
void printTree(Node *root);
Node *checkExplode(Node *root, int depth);
Node *checkSplit(Node *root);
void explode(Node *node);
void split(Node *node);
void reduce(Node *root);

Node *addTrees(Node *a, Node *b);

size_t computeMagnitude(Node *node);

Node *findLeftSibling(Node *node);
Node *findRightSibling(Node *node);

Node *copyTree(Node *root);

int main() {
  // Part 1
  int num_trees = 0;
  Node **trees = getTrees(&num_trees);
  Node **trees_pt_1 = calloc(sizeof(Node*), num_trees);

  for(int i = 0; i < num_trees; i++)
    trees_pt_1[i] = copyTree(trees[i]);

  Node *curr = trees_pt_1[0];
  for (int i = 1; i < num_trees; i++) {
    curr = addTrees(curr, trees_pt_1[i]);
    reduce(curr);
  }
  printf("Magnitude of the sum of all snail numbers %zu\n",
         computeMagnitude(curr));

  Node **trees_pt_2 = calloc(sizeof(Node*), num_trees);
  for(int i = 0; i < num_trees; i++)
    trees_pt_2[i] = copyTree(trees[i]);

  size_t max_magnitude = 0;
  for (int i = 0; i < num_trees; i++) {
    for (int j = 0; j < num_trees; j++) {
      if (i == j)
        continue;
      Node *first = trees_pt_2[i];
      Node *second = trees_pt_2[j];
      Node *curr = addTrees(first, second);
      reduce(curr);
      size_t magnitude = computeMagnitude(curr);
      if(magnitude > max_magnitude)
        max_magnitude = magnitude;
      trees_pt_2[i] = copyTree(trees[i]);
      trees_pt_2[j] = copyTree(trees[j]);
    }
  }
  printf("The largest magnitude of the sum of any two snailfish numbers in the list is %zu.\n", max_magnitude);

  return 0;
}
Node **getTrees(int *num_trees) {
  FILE *fp = fopen(FILENAME, "r");
  size_t read_len;
  char *str;
  Node **trees = calloc(sizeof(Node*), 100);
  while ((str = fgetln(fp, &read_len))) {
    Node *root = readTree(str, read_len);
    reduce(root);
    trees[(*num_trees)++] = root;
  }
  fclose(fp);
  return trees;
}

Node *readTree(char *str, size_t len) {
  Node *root = calloc(sizeof(Node), 1);
  Node *last = root;
  for (int i = 1; i < len - 1; i++) {
    char ch = str[i];
    if (ch == '[') {
      Node *new_node = calloc(sizeof(Node), 1);
      new_node->parent = last;
      if (!last->left)
        last->left = new_node;
      else
        last->right = new_node;
      last = new_node;
    } else if (ch == ',') {

    } else if (ch == ']') {
      last = last->parent;
    } else if (!last->left) {
      Node *new_node = calloc(sizeof(Node), 1);
      new_node->value = ch - '0';
      new_node->parent = last;
      last->left = new_node;
    } else {
      Node *new_node = calloc(sizeof(Node), 1);
      new_node->value = ch - '0';
      new_node->parent = last;
      last->right = new_node;
    }
  }
  return root;
}

void printTree(Node *node) {
  if (!node->left && !node->right) {
    printf("%zu", node->value);
    return;
  }
  if (node->left) {
    putchar('[');
    printTree(node->left);
    putchar(',');
  }
  if (node->right) {
    printTree(node->right);
    putchar(']');
  }
  return;
}

Node *checkExplode(Node *node, int depth) {
  if (!node->left && !node->right)
    return NULL;
  if (depth > 3)
    return node;
  Node *left_node = checkExplode(node->left, depth + 1);
  if (left_node)
    return left_node;
  Node *right_node = checkExplode(node->right, depth + 1);
  if (right_node)
    return right_node;
  return NULL;
}

Node *checkSplit(Node *node) {
  if (!node->left && !node->right)
    return node->value >= 10 ? node : NULL;
  Node *left_node = checkSplit(node->left);
  if (left_node)
    return left_node;
  Node *right_node = checkSplit(node->right);
  if (right_node)
    return right_node;
  return NULL;
}

void explode(Node *node) {
  size_t left_val = node->left->value;
  size_t right_val = node->right->value;
  free(node->left);
  free(node->right);
  node->left = NULL;
  node->right = NULL;
  node->value = 0;
  Node *leftSibling = findLeftSibling(node);
  if (leftSibling)
    leftSibling->value += left_val;
  Node *rightSibling = findRightSibling(node);
  if (rightSibling)
    rightSibling->value += right_val;
}

void split(Node *node) {
  assert(node->value >= 10 &&
         "Value is not greater than 10 why are we splitting?");
  int left_val = floor(node->value / 2.0);
  int right_val = ceil(node->value / 2.0);
  node->value = 0;
  Node *left_node = calloc(sizeof(Node), 1);
  left_node->value = left_val;
  left_node->parent = node;
  node->left = left_node;

  Node *right_node = calloc(sizeof(Node), 1);
  right_node->value = right_val;
  right_node->parent = node;
  node->right = right_node;
}

void reduce(Node *root) {
  int changed = 0;
  do {
    changed = 0;
    Node *to_explode = checkExplode(root, 0);
    if (to_explode) {
      changed = 1;
      explode(to_explode);
      continue;
    }
    Node *to_split = checkSplit(root);
    if (to_split) {
      changed = 1;
      split(to_split);
    }
  } while (changed);
}

Node *addTrees(Node *a, Node *b) {
  Node *new_root = calloc(sizeof(Node), 1);
  new_root->left = a;
  a->parent = new_root;
  new_root->right = b;
  b->parent = new_root;
  return new_root;
}

size_t computeMagnitude(Node *node) {
  if (!node->left && !node->right)
    return node->value;
  return 3 * computeMagnitude(node->left) + 2 * computeMagnitude(node->right);
}

Node *findLeftSibling(Node *node) {
  assert(node->parent && "Find left sibling of root is impossible");
  Node *child = node;
  Node *parent = child->parent;
  while (parent->left == child) {
    child = parent;
    if (!child->parent)
      return NULL;
    parent = child->parent;
  }
  child = parent->left;
  if (!child->right)
    return child;
  do {
    child = child->right;
  } while (child->right);
  return child;
}

Node *findRightSibling(Node *node) {
  assert(node->parent && "Find right sibling of root is impossible");
  Node *child = node;
  Node *parent = child->parent;
  while (parent->right == child) {
    child = parent;
    if (!child->parent)
      return NULL;
    parent = child->parent;
  }
  child = parent->right;
  if (!child->left)
    return child;
  do {
    child = child->left;
  } while (child->left);
  return child;
}

Node *copyTree(Node *node) {
  if(!node->left && !node->right) {
    Node *new_leaf = calloc(sizeof(Node), 1);
    new_leaf->value = node->value;
    return new_leaf;
  }
  Node *new_node = calloc(sizeof(Node), 1);
  new_node->left = copyTree(node->left);
  new_node->left->parent = new_node;
  new_node->right = copyTree(node->right);
  new_node->right->parent = new_node;
  return new_node;
}
