//
// Advent of Code 2021
//
// Day 7
//
// Author: Simone Franza
//
// Thought process:
// Try to find the minimum via a binary search
// For part 2, the function to minimize is probably not convex so binary search
// won't work. Just iterate from min value to max and compute sum. 
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define FILENAME "input"
#define ARR_SIZE 100

int main() {
  // Part 1
  FILE *fp = fopen(FILENAME, "r");
  size_t read_len;
  char *str = fgetln(fp, &read_len);
  char *copy = (char *)calloc(sizeof(char), read_len + 1);
  memcpy(copy, str, read_len);
  char *token = strtok(copy, ",");
  int *crabs = (int*)calloc(sizeof(int), sizeof(int)*ARR_SIZE);
  size_t num_crabs = 0;
  size_t max_crabs = ARR_SIZE;
  int min_val = 999999;
  int max_val = -1;
  while (token) {
    crabs[num_crabs++] = atoi(token);
    if(crabs[num_crabs - 1] > max_val)
      max_val = crabs[num_crabs - 1];
    if(crabs[num_crabs - 1] < min_val)
      min_val = crabs[num_crabs - 1];
    if (num_crabs == max_crabs) {
      int *temp = realloc(crabs, sizeof(int)*(num_crabs + ARR_SIZE));
      if (!temp) {
        free(copy);
        free(crabs);
        fclose(fp);
        return -1;
      }
      crabs = temp;
      max_crabs += ARR_SIZE;
    }
    token = strtok(NULL, ",");
  }
  int start = min_val;
  int end = max_val;
  while(start < end - 1) {
    double mid = (end + start) / 2.0;
    double left_mid = (start + mid) / 2.0;
    double right_mid = (end + mid) / 2.0;
    int left_sum = 0;
    int right_sum = 0;
    for(int i = 0; i < num_crabs; i++) {
      left_sum += fabs(crabs[i] - left_mid);
      right_sum += fabs(crabs[i] - right_mid);
    }
    if(left_sum < right_sum)
      end = mid;
    else
      start = mid;
  }
  int left_sum = 0;
  int right_sum = 0;
  for(int i = 0; i < num_crabs; i++) {
    left_sum += abs(crabs[i] - start);
    right_sum += abs(crabs[i] - end);
  }
  int min_fuel = left_sum < right_sum ? left_sum : right_sum;
  int min_pos = left_sum < right_sum ? start : end;
  printf("Part 1: Minimum fuel used at position %d, total fuel %d\n", min_pos, min_fuel);

  // Part 2
  min_fuel = -1;
  min_pos = 0;
  for(int pos = min_val; pos <= max_val; pos++) {
    int sum = 0;
    for(int i = 0; i < num_crabs; i++) {
      int dist = abs(crabs[i] - pos);
      sum += (dist*(dist + 1))/2;
    }
    if(sum < min_fuel || min_fuel == -1)
    {
      min_fuel = sum;
      min_pos = pos;
    }
  }
  printf("Part 2: Minimum fuel used at position %d, total fuel %d\n", min_pos, min_fuel);

  // Cleanup
  free(copy);
  free(crabs);
  fclose(fp);

  return 0;
}
