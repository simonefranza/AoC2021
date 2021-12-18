//
// Advent of Code 2021
//
// Day 5
//
// Author: Simone Franza
//
// Thought process:
// Create 2D array where each entry represents the number of vents on that spot
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "input"
#define BOARD_EDGE 1000
#define NUM_POINTS 500

typedef struct point_ {
  int x;
  int y;
} point;

typedef struct segment_ {
  point start;
  point end;
} segment;

int main() {
  // Part 1
  FILE *fp = fopen(FILENAME, "r");
  segment *segments = (segment *)calloc(sizeof(segment), NUM_POINTS);
  int num_segments = 0;
  char *str;
  size_t len_read = 0;
  int *board = (int *)calloc(sizeof(int), BOARD_EDGE * BOARD_EDGE);
  while ((str = fgetln(fp, &len_read))) {
    char *copied_str = (char *)calloc(sizeof(char), len_read + 1);
    memcpy(copied_str, str, len_read);
    char *token = strtok(copied_str, ",");
    int start_x = atoi(token);
    char *sep = " -> ";
    token = strtok(NULL, sep);
    int start_y = atoi(token);
    token = strtok(NULL, ",");
    int end_x = atoi(token + 3);
    token = strtok(NULL, "");
    int end_y = atoi(token);
    segments[num_segments].start.x = start_x;
    segments[num_segments].start.y = start_y;
    segments[num_segments].end.x = end_x;
    segments[num_segments++].end.y = end_y;
    if (start_x != end_x && start_y != end_y)
      continue;
    int is_same_x = start_x == end_x;
    int start_val = is_same_x ? start_y : start_x;
    int end_val = is_same_x ? end_y : end_x;
    int sign = start_val < end_val ? 1 : -1;

    for (int i = start_val; sign > 0 ? i <= end_val : i >= end_val; i += sign)
      board[is_same_x ? i * BOARD_EDGE + start_x : i + start_y * BOARD_EDGE]++;
  }
  int two_overlaps = 0;
  for (int i = 0; i < BOARD_EDGE * BOARD_EDGE; i++)
    if (board[i] >= 2)
      two_overlaps++;

  printf("Part 1: At %d points overlap at least two lines\n", two_overlaps);

  // Part 2
  for (int seg = 0; seg < num_segments; seg++) {
    segment curr_segment = segments[seg];
    int start_x = curr_segment.start.x;
    int start_y = curr_segment.start.y;
    int end_x = curr_segment.end.x;
    int end_y = curr_segment.end.y;
    if (start_x == end_x || start_y == end_y)
      continue;
    int sign_x = start_x < end_x ? 1 : -1;
    int sign_y = start_y < end_y ? 1 : -1;
    while (1) {
      board[start_y * BOARD_EDGE + start_x]++;
      if (start_y == end_y && start_x == end_x)
        break;
      start_y += sign_y;
      start_x += sign_x;
    }
  }

  two_overlaps = 0;
  for (int i = 0; i < BOARD_EDGE * BOARD_EDGE; i++)
    if (board[i] >= 2)
      two_overlaps++;

  printf("Part 2: At %d points overlap at least two lines\n", two_overlaps);

  // Cleanup
  free(segments);

  return 0;
}
