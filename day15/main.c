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

#define FILENAME "input"
#define WIDTH 100
#define HEIGHT 100
#define WIDTH_PT_2 WIDTH * 5
#define HEIGHT_PT_2 HEIGHT * 5
#define ZERO_VAL 48
#define BIG_NUMBER 99999

int *readInput();
void performDijkstra(int *data, int width, int height);
int *scaleData(int *data);
void drawSteps(int *prev, int *data, int width, int height);
int findNextIdx(int *dist, int *visited, int width, int height);

int main() {
  // Part 1
  int *data = readInput();
  performDijkstra(data, WIDTH, HEIGHT);

  // Part 2
  data = scaleData(data);
  performDijkstra(data, WIDTH_PT_2, HEIGHT_PT_2);

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
void performDijkstra(int *data, int width, int height) {
  int *dist = calloc(sizeof(int), width * height);
  int *visited = calloc(sizeof(int), width * height);
  int *prev = calloc(sizeof(int), width * height);
  for (int i = 1; i < width * height; i++) {
    dist[i] = BIG_NUMBER;
    visited[i] = 0;
    prev[i] = -1;
  }
  int idx;
  while ((idx = findNextIdx(dist, visited, width, height)) != -1) {
    int col = idx % width;
    int row = idx / width;
    //printf("Idx %d col %d row %d\n", idx, col, row);
    // Right
    if (col < width - 1) {
      int new_dist = dist[row * width + col] + data[row * width + col + 1];
      if (new_dist <= dist[row * width + col + 1]) {
        dist[row * width + col + 1] = new_dist;
        prev[row * width + col + 1] = row * width + col;
      }
    }
    // Bottom
    if (row < height - 1) {
      int new_dist = dist[row * width + col] + data[(row + 1) * width + col];
      if (new_dist <= dist[(row + 1) * width + col]) {
        dist[(row + 1) * width + col] = new_dist;
        prev[(row + 1) * width + col] = row * width + col;
      }
    }
    // Left
    if (col) {
      int new_dist = dist[row * width + col] + data[row * width + col - 1];
      if (new_dist < dist[row * width + col - 1]) {
        dist[row * width + col - 1] = new_dist;
        prev[row * width + col - 1] = row * width + col;
      }
    }
    // Top
    if (row) {
      int new_dist = dist[row * width + col] + data[(row - 1) * width + col];
      if (new_dist < dist[(row - 1) * width + col]) {
        dist[(row - 1) * width + col] = new_dist;
        prev[(row - 1) * width + col] = row * width + col;
      }
    }
    visited[row * width + col] = 1;
  }
  printf("Total to reach last tile %d\n", dist[width * height - 1]);
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
  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      if (row * width + col == idxs[pos]) {
        printf("\x1b[38;2;0;0;0m\x1b[48;2;255;255;255m%d\x1b[0m",
               data[row * width + col]);
        pos--;
      } else {
        printf("%d", data[row * width + col]);
      }
    }
    putchar('\n');
  }
}

int findNextIdx(int *dist, int *visited, int width, int height) {
  int min = BIG_NUMBER;
  int arg_min = -1;
  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      if (visited[row * width + col])
        continue;
      if (dist[row * width + col] < min) {
        min = dist[row * width + col];
        arg_min = row * width + col;
      }
    }
  }
  return arg_min;
}
