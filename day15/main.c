//
// Advent of Code 2021
//
// Day 15
//
// Author: Simone Franza
//
// Thought process:
// I think Dijkstra's algorithm is a good fit for this one, so I'm going for it
// Use second array to mark visited nodes, third for distances
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "heap.h"

#define FILENAME "input"
#define WIDTH 100
#define HEIGHT 100
#define WIDTH_PT_2 (WIDTH * 5)
#define HEIGHT_PT_2 (HEIGHT * 5)
#define ZERO_VAL 48
#define BIG_NUMBER 999999

int *readInput();
void performDijkstra(int *data, int width, int height, int enable_out);
int *scaleData(int *data);
void drawSteps(int *prev, int *data, int width, int height);
void updateDist(Heap *heap, associated_data **handles, int *dist, int *data, 
    int *prev, int to_set_idx, int curr_idx);

int main(int argv, char *argc[]) {
  int enable_out = 0;
  // Enable output
  if (argv == 2 && !strcmp(argc[1], "-v"))
    enable_out = 1;
  // Part 1
  int *data = readInput();
  performDijkstra(data, WIDTH, HEIGHT, enable_out);

  // Part 2
  data = scaleData(data);
  performDijkstra(data, WIDTH_PT_2, HEIGHT_PT_2, enable_out);

  return 0;
}

int *readInput() {
  int *data = calloc(sizeof(int), WIDTH * HEIGHT);
  FILE *fp = fopen(FILENAME, "r");
  size_t read_len;
  char *str;
  int line = 0;
  while ((str = fgetln(fp, &read_len))) {
    for (int i = 0; i < WIDTH; i++)
      data[line * WIDTH + i] = str[i] - ZERO_VAL;
    line++;
  }

  fclose(fp);
  return data;
}
void performDijkstra(int *data, int width, int height, int enable_out) {
  int *dist = calloc(sizeof(int), width * height);
  int *prev = calloc(sizeof(int), width * height);
  associated_data **handles = calloc(sizeof(associated_data*), width * height);
  associated_data *ad = calloc(sizeof(associated_data), width * height);

  for (int i = 0; i < width * height; i++) {
    dist[i] = !i ? 0 : BIG_NUMBER;
    ad[i].dist_ptr = dist + i;
    ad[i].idx = i;
    prev[i] = -1;
  }
  Heap *heap = createHeapData(ad, width*height, MIN_HEAP);
  for (int i = 0; i < width * height; i++) {
    handles[heap->data[i]->idx] = heap->data[i];
  }
  int idx = pop(heap)->idx;
  while (1) {
    int col = idx % width;
    int row = idx / width;
    // Right
    if ((idx % width) < width - 1)
      updateDist(heap, handles, dist, data, prev, idx + 1, idx);
    // Bottom
    if (idx / width < height - 1)
      updateDist(heap, handles, dist, data, prev, idx + width, idx);
    // Left
    if (idx % width)
      updateDist(heap, handles, dist, data, prev, idx - 1, idx);
    // Top
    if (idx / width)
      updateDist(heap, handles, dist, data, prev, idx - width, idx);

    if(isEmpty(heap))
      break;
    idx = pop(heap)->idx;
  }
  printf("Total to reach last tile %d\n", dist[width * height - 1]);
  if(enable_out)
    drawSteps(prev, data, width, height);
}

int *scaleData(int *data) {
  int *res = calloc(sizeof(int), (WIDTH_PT_2) * (HEIGHT_PT_2));
  for (int row = 0; row < HEIGHT_PT_2; row++) {
    int row_scale = row / HEIGHT;
    for (int col = 0; col < HEIGHT_PT_2; col++) {
      int col_scale = col / WIDTH;
      int new_val =
          data[(row % WIDTH) * WIDTH + (col % HEIGHT)] + row_scale + col_scale;
      if (new_val > 9)
        new_val -= 9;
      res[row * (WIDTH_PT_2) + col] = new_val;
    }
  }
  return res;
}


void drawSteps(int *prev, int *data, int width, int height) {
  int *idxs = calloc(sizeof(int), width * height);
  int pos = 0;
  int idx = width * height - 1;
  while (idx) {
    idxs[pos++] = idx;
    idx = prev[idx];
  }
  idxs[pos] = 0;
  int *map = calloc(sizeof(int), width * height);
  for(int i = 0; i < pos + 1;i++) {
    map[idxs[i]] = 1;
  }
  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      if (map[row*width + col]) {
        printf("\x1b[38;2;0;0;0m\x1b[48;2;255;255;255m%d\x1b[0m",
               data[row * width + col]);
      } else {
        printf("%d", data[row * width + col]);
      }
    }
    putchar('\n');
  }
}

void updateDist(Heap *heap, associated_data **handles, int *dist, int *data, 
    int *prev, int to_set_idx, int curr_idx) {
  int new_dist = dist[curr_idx] + data[to_set_idx];
  if (new_dist <= dist[to_set_idx]) {
    dist[to_set_idx] = new_dist;
    siftUp(heap->data, 0, handles[to_set_idx]->heap_idx, heap->type);
    prev[to_set_idx] = curr_idx;
  }
}
