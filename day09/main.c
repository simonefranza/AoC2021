//
// Advent of Code 2021
//
// Day 9
//
// Author: Simone Franza
//
// Thought process:
// Save the map in array, then iterate through map, use another array to remember
// position already visited
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "input"
#define WIDTH 100
#define HEIGHT 100

#define ARR_SIZE 10

typedef struct point_ {
  int line;
  int col;
  int basin_size;
}point;

int argMax(point *basins, int num_basins);

int main() {
  // Part 1
  FILE *fp = fopen(FILENAME, "r");
  size_t read_len;
  uint8_t *data = calloc(sizeof(uint8_t), WIDTH * HEIGHT);
  uint8_t *data_visited = calloc(sizeof(uint8_t), WIDTH * HEIGHT);
  for (int line = 0; line < HEIGHT; line++) {
    char *str = fgetln(fp, &read_len);
    char *copy = (char *)calloc(sizeof(char), read_len + 1);
    memcpy(copy, str, read_len);
    char *token = strtok(copy, "");
    for (int col = 0; col < WIDTH; col++) {
      data[line * WIDTH + col] = token[col] - 48;
    }
    free(copy);
  }
  // Leave out to find result of part 1
  point *basins = calloc(sizeof(point), ARR_SIZE);
  int num_basins = 0;
  int max_basins = ARR_SIZE;
  int sum = 0;
  for (int line = 0; line < HEIGHT; line++) {
    for (int col = 0; col < WIDTH; col++) {
      if (data_visited[line * WIDTH + col])
        continue;
      int curr_line = line;
      int curr_col = col;
      int done = 0;
      while (!done) {
        if(data_visited[curr_line*WIDTH + curr_col])
          break;
        int min_col = -1;
        int min_line = -1;
        int min_val = data[curr_line*WIDTH+ curr_col];
        // Check top
        if(curr_line - 1 >= 0) {
          int val = data[(curr_line - 1) * WIDTH+ curr_col];
          if(val <= min_val) {
            min_val = val;
            min_col = curr_col;
            min_line = curr_line - 1;
          }
        }
        // Check right
        if(curr_col + 1 < WIDTH) {
          int val = data[curr_line * WIDTH + curr_col + 1];
          if(val <= min_val) {
            min_val = val;
            min_col = curr_col + 1;
            min_line = curr_line;
          }
        }
        // Check bottom 
        if(curr_line + 1 < HEIGHT) {
          int val = data[(curr_line + 1) * WIDTH + curr_col];
          if(val <= min_val) {
            min_val = val;
            min_col = curr_col;
            min_line = curr_line + 1;
          }
        }
        // Check left 
        if(curr_col - 1 >= 0) {
          int val = data[curr_line * WIDTH + curr_col - 1];
          if(val <= min_val) {
            min_val = val;
            min_col = curr_col - 1;
            min_line = curr_line;
          }
        }
        data_visited[curr_line * WIDTH + curr_col] = 1;
        if(min_col == -1) {
          sum += data[curr_line * WIDTH + curr_col] + 1;
          basins[num_basins].basin_size = 0;
          basins[num_basins].line = curr_line;
          basins[num_basins++].col = curr_col;
          if(num_basins == max_basins) {
            basins = realloc(basins, sizeof(point)*(num_basins + ARR_SIZE));
            max_basins += ARR_SIZE;
          }
          break;
        }
        curr_col = min_col;
        curr_line = min_line;
      }
    }
  }
  printf("The sum of the risk levels of all low points (%d) on the heightmap is %d\n", num_basins, sum);

  // Part 2
  memset(data_visited, 0, sizeof(uint8_t)*WIDTH*HEIGHT);
  for (int line = 0; line < HEIGHT; line++) {
    for (int col = 0; col < WIDTH; col++) {
      int curr_line = line;
      int curr_col = col;
      if(data[curr_line*WIDTH+ curr_col] == 9)
        continue;
      int done = 0;
      while (!done) {
        int min_col = -1;
        int min_line = -1;
        int min_val = data[curr_line*WIDTH+ curr_col];
        // Check top
        if(curr_line - 1 >= 0) {
          int val = data[(curr_line - 1) * WIDTH+ curr_col];
          if(val <= min_val) {
            min_val = val;
            min_col = curr_col;
            min_line = curr_line - 1;
          }
        }
        // Check right
        if(curr_col + 1 < WIDTH) {
          int val = data[curr_line * WIDTH + curr_col + 1];
          if(val <= min_val) {
            min_val = val;
            min_col = curr_col + 1;
            min_line = curr_line;
          }
        }
        // Check bottom 
        if(curr_line + 1 < HEIGHT) {
          int val = data[(curr_line + 1) * WIDTH + curr_col];
          if(val <= min_val) {
            min_val = val;
            min_col = curr_col;
            min_line = curr_line + 1;
          }
        }
        // Check left 
        if(curr_col - 1 >= 0) {
          int val = data[curr_line * WIDTH + curr_col - 1];
          if(val <= min_val) {
            min_val = val;
            min_col = curr_col - 1;
            min_line = curr_line;
          }
        }
        data_visited[curr_line * WIDTH + curr_col] = 1;
        if(min_col == -1) {
          // Find basin and increase size
          for(int i = 0; i < num_basins; i++) {
            if(basins[i].col == curr_col && basins[i].line == curr_line) {
              basins[i].basin_size++;
              break;
            }
          }
          break;
        }
        curr_col = min_col;
        curr_line = min_line;
      }
    }
  }
  int res = 1;
  for(int i = 0; i < 3; i++) {
    int idx = argMax(basins, num_basins);
    res *= basins[idx].basin_size;
    basins[idx].basin_size = 0;
  }
  printf("The product of the sizes of the three largest basins is %d\n", res);

  // Cleanup
  free(data);
  fclose(fp);

  return 0;
}

int argMax(point *basins, int num_basins) {
  int max = -1;
  int max_idx = -1;
  for(int i = 0; i < num_basins; i++) {
    if(basins[i].basin_size > max) {
      max = basins[i].basin_size;
      max_idx = i;
    }
  }
  return max_idx;
}
