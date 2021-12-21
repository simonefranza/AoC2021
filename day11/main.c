//
// Advent of Code 2021
//
// Day 11
//
// Author: Simone Franza
//
// Thought process:
// Once an octopus flashed set its value to -9999, then after the step,
// all negative entries are set to 0
// Part 2: count flashes in a cycle and when it is == EDGE*EDGE stop
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "input"
#define EDGE 10
#define STEPS 100

void printField(int *data);

int main(int argv, char *argc[]) {
  int enable_out = 0;
  // Enable output
  if (argv == 2 && !strcmp(argc[1], "-v"))
    enable_out = 1;

  // Part 1
  int *data = calloc(sizeof(int), EDGE * EDGE);
  int *data_pt2 = calloc(sizeof(int), EDGE * EDGE);
  FILE *fp = fopen(FILENAME, "r");
  size_t read_len;
  for (int row = 0; row < EDGE; row++) {
    char *str = fgetln(fp, &read_len);
    for (int col = 0; col < EDGE; col++) {
      data[row * EDGE + col] = str[col] - 48;
    }
  }
  memcpy(data_pt2, data, sizeof(int)*EDGE*EDGE);
  if(enable_out)
    printField(data);
  int flashes = 0;
  for (int i = 0; i < STEPS; i++) {
    // Update values
    for (int j = 0; j < EDGE * EDGE; j++)
      data[j]++;
    // Flash
    int total_flashes = 0;
    int something_flashed = 1;
    while(something_flashed) {
      something_flashed = 0;
      for (int row = 0; row < EDGE; row++) {
        for (int col = 0; col < EDGE; col++) {
          if (data[row * EDGE + col] <= 9)
            continue;
          // Top
          if (row > 0)
            data[(row - 1) * EDGE + col]++;
          // Top right
          if (row > 0 && col < EDGE - 1)
            data[(row - 1) * EDGE + col + 1]++;
          // Right
          if (col < EDGE - 1)
            data[row * EDGE + col + 1]++;
          // Bottom right
          if (col < EDGE - 1 && row < EDGE - 1)
            data[(row + 1) * EDGE + col + 1]++;
          // Bottom
          if (row < EDGE - 1)
            data[(row + 1) * EDGE + col]++;
          // Bottom left
          if (row < EDGE - 1 && col > 0)
            data[(row + 1) * EDGE + col - 1]++;
          // Left
          if(col > 0)
            data[row * EDGE + col - 1]++;
          // Top left
          if(col > 0 && row > 0)
            data[(row - 1) * EDGE + col - 1]++;

          something_flashed = 1;
          total_flashes++;
          data[row * EDGE + col] = -9999;
        }
      }
    }
    if(enable_out)
      printField(data);
    // Reset to zero
    for(int i = 0; i < EDGE * EDGE; i++)
      if(data[i] < 0)
        data[i] = 0;
    flashes += total_flashes;
  }
  printf("Total of %d flashes\n", flashes);

  // Part 2
  int res_steps = 1;
  while(1) {
    // Update values
    for (int j = 0; j < EDGE * EDGE; j++)
      data_pt2[j]++;
    // Flash
    int total_flashes = 0;
    int something_flashed = 1;
    while(something_flashed) {
      something_flashed = 0;
      for (int row = 0; row < EDGE; row++) {
        for (int col = 0; col < EDGE; col++) {
          if (data_pt2[row * EDGE + col] <= 9)
            continue;
          // Top
          if (row > 0)
            data_pt2[(row - 1) * EDGE + col]++;
          // Top right
          if (row > 0 && col < EDGE - 1)
            data_pt2[(row - 1) * EDGE + col + 1]++;
          // Right
          if (col < EDGE - 1)
            data_pt2[row * EDGE + col + 1]++;
          // Bottom right
          if (col < EDGE - 1 && row < EDGE - 1)
            data_pt2[(row + 1) * EDGE + col + 1]++;
          // Bottom
          if (row < EDGE - 1)
            data_pt2[(row + 1) * EDGE + col]++;
          // Bottom left
          if (row < EDGE - 1 && col > 0)
            data_pt2[(row + 1) * EDGE + col - 1]++;
          // Left
          if(col > 0)
            data_pt2[row * EDGE + col - 1]++;
          // Top left
          if(col > 0 && row > 0)
            data_pt2[(row - 1) * EDGE + col - 1]++;

          something_flashed = 1;
          total_flashes++;
          data_pt2[row * EDGE + col] = -9999;
        }
      }
    }
    if(enable_out)
      printField(data_pt2);
    // Reset to zero
    for(int i = 0; i < EDGE * EDGE; i++)
      if(data_pt2[i] < 0)
        data_pt2[i] = 0;
    if(total_flashes == EDGE*EDGE)
      break;
    res_steps++;
  }
  printf("Took %d steps for full flash\n", res_steps);

  // Cleanup
  free(data);
  free(data_pt2);
  fclose(fp);

  return 0;
}

void printField(int *data) {
  for (int i = 0; i < EDGE * EDGE; i++) {
    if (data[i] < 0)
      printf("\x1b[38;2;0;0;0m\x1b[48;2;255;255;255m0\x1b[0m");
    else
      printf("%d", data[i]);
    if (i && (i + 1) % EDGE == 0)
      putchar('\n');
  }
  putchar('\n');
}
