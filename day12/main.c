//
// Advent of Code 2021
//
// Day 12
//
// Author: Simone Franza
//
// Thought process:
// Create graph, then add node connected to start to queue and save which small
// nodes where already visited. Then add all possibilities to queue and
// go to next element
//

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "input"
#define MAX_NODES_REF 50
#define MAX_PATHS 15000
#define START "ST"
#define END "EN"

typedef struct Node_ {
  char name[3];
  int is_big;
  int num_edges;
  struct Node_ **siblings;
} Node;

typedef struct PathStep_ {
  Node *node;
  int num_visited;
  Node *avoid_list[15];
} PathStep;

typedef struct PathStep2_ {
  Node *node;
  int num_visited;
  int visited_one_twice;
  Node *avoid_list[40];
} PathStep2;

void createGraph(Node **nodes, int *num_nodes);
Node *findNodeRef(Node **nodes, int num_nodes, char name[3]);
Node *createNode(char name[3]);
void addConnection(Node *node_a, Node *node_b);
int alreadyVisitedNode(Node **list, int list_len, Node *node);
void shiftPaths(PathStep *paths, int shift_idx);
void shiftPaths2(PathStep2 *paths, int shift_idx);

int main() {
  // Part 1
  Node **nodes = calloc(sizeof(Node *), MAX_NODES_REF);
  int num_nodes = 0;
  createGraph(nodes, &num_nodes);

  Node *start = findNodeRef(nodes, num_nodes, START);

  PathStep *paths = calloc(sizeof(PathStep), MAX_PATHS);
  int paths_size = MAX_PATHS;
  int num_paths = 0;
  for (int i = 0; i < start->num_edges; i++) {
    paths[num_paths].node = start->siblings[i];
    paths[num_paths].avoid_list[paths[num_paths].num_visited++] = start;
    num_paths++;
  }
  int existing_paths = 0;
  for (int i = 0; i < num_paths; i++) {
    Node *curr = paths[i].node;
    
    for (int j = 0; j < curr->num_edges; j++) {
      Node *new_node = curr->siblings[j];
      if (!strcmp(new_node->name, END)) {
        existing_paths++;
        continue;
      }
      if(alreadyVisitedNode(paths[i].avoid_list, paths[i].num_visited, new_node))
        continue;

      paths[num_paths].node = new_node;
      memcpy(paths[num_paths].avoid_list, paths[i].avoid_list, sizeof(Node*)*paths[i].num_visited);
      paths[num_paths].num_visited = paths[i].num_visited;
      if(!isupper(curr->name[0]))
        paths[num_paths].avoid_list[paths[num_paths].num_visited++] = curr;
      num_paths++;
      if(num_paths == paths_size) {
        paths = realloc(paths, sizeof(PathStep)*(num_paths + 100));
        paths_size += 100;
      }
    }
  }
  printf("There are a total of %d paths\n", existing_paths);

  //Part 2
  PathStep2 *paths2 = calloc(sizeof(PathStep2), MAX_PATHS);
  int paths2_size = MAX_PATHS;
  num_paths = 0;
  for (int i = 0; i < start->num_edges; i++) {
    paths2[num_paths].node = start->siblings[i];
    paths2[num_paths].num_visited = 0;
    paths2[num_paths].avoid_list[paths2[num_paths].num_visited++] = start;
    paths2[num_paths].visited_one_twice = 0;
    num_paths++;
  }
  existing_paths = 0;
  for (int i = 0; i < num_paths; i++) {
    Node *curr = paths2[i].node;
    
    for (int j = 0; j < curr->num_edges; j++) {
      Node *new_node = curr->siblings[j];
      if (!strcmp(new_node->name, START))
        continue;
      if (!strcmp(new_node->name, END)) {
        existing_paths++;
        continue;
      }
      int visited = alreadyVisitedNode(paths2[i].avoid_list, paths2[i].num_visited, new_node);
      if(visited && paths2[i].visited_one_twice)
        continue;

      paths2[num_paths].node = new_node;
      memcpy(paths2[num_paths].avoid_list, paths2[i].avoid_list, sizeof(Node*)*paths2[i].num_visited);
      paths2[num_paths].num_visited = paths2[i].num_visited;
      paths2[num_paths].visited_one_twice = paths2[i].visited_one_twice;

      if(visited)
        paths2[num_paths].visited_one_twice = 1;
      if(!isupper(curr->name[0]))
        paths2[num_paths].avoid_list[paths2[num_paths].num_visited++] = curr;
      num_paths++;
      if(num_paths == paths2_size) {
        paths2 = realloc(paths2, sizeof(PathStep2)*(num_paths + 100));
        paths2_size += 100;
      }
    }
  }
  printf("There are a total of %d paths\n", existing_paths);

  // Cleanup
  free(paths);
  free(paths2);

  return 0;
}

int alreadyVisitedNode(Node **list, int list_len, Node *node) {
  for (int i = 0; i < list_len; i++)
    if(list[i] == node)
      return 1;
  return 0;
}

Node *findNodeRef(Node **nodes, int num_nodes, char name[3]) {
  for (int i = 0; i < MAX_NODES_REF; i++) {
    if (nodes[i] && !strcmp(nodes[i]->name, name))
      return nodes[i];
  }
  return NULL;
}

Node *createNode(char name[3]) {
  Node *node = calloc(sizeof(Node), 1);
  memcpy(node->name, name, 3);
  node->is_big = isupper(name[0]);
  node->num_edges = 0;
  node->siblings = calloc(sizeof(Node *), 20);
  return node;
}

void addConnection(Node *node_a, Node *node_b) {
  node_a->siblings[node_a->num_edges++] = node_b;
  node_b->siblings[node_b->num_edges++] = node_a;
}

void createGraph(Node **nodes, int *num_nodes) {
  FILE *fp = fopen(FILENAME, "r");
  size_t read_len;
  char *str;
  while ((str = fgetln(fp, &read_len))) {
    char *copy = calloc(sizeof(char), read_len);
    memcpy(copy, str, read_len - 1);
    char *first_name = strtok(copy, "-");
    if (!strcmp(first_name, "start"))
      memcpy(first_name, START, 3);
    else if (!strcmp(first_name, "end"))
      memcpy(first_name, END, 3);
    char *second_name = strtok(NULL, "");
    if (!strcmp(second_name, "start"))
      memcpy(second_name, START, 3);
    else if (!strcmp(second_name, "end"))
      memcpy(second_name, END, 3);
    Node *first_node = findNodeRef(nodes, *num_nodes, first_name);
    if (!first_node) {
      first_node = createNode(first_name);
      nodes[(*num_nodes)++] = first_node;
    }
    Node *second_node = findNodeRef(nodes, *num_nodes, second_name);
    if (!second_node) {
      second_node = createNode(second_name);
      nodes[(*num_nodes)++] = second_node;
    }
    addConnection(first_node, second_node);
  }
  fclose(fp);
}
