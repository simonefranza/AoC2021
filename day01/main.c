//
// Advent of Code 2021
//
// Day 1
//
// Author: Simone Franza
//

#include <stdio.h>
#include <stdlib.h>

#define FILENAME "input"

int main() {
  // Part 1
  FILE *fp = fopen(FILENAME, "r");
  char* str = NULL;
  size_t len_read = 0;
  str = fgetln(fp, &len_read);
  int num_increases = 0;
  int last_element = atoi(str);
  int curr_element = 0;
  while ((str = fgetln(fp, &len_read))) {
    curr_element = atoi(str);
    if(curr_element > last_element)
      num_increases++;
    last_element = curr_element;
  }
  printf("%d measurements are larger than the previous measurement.\n", num_increases);

  // Part 2
  // Sliding window with 3 elements is the same as comparing values every 3 elements
  fseek(fp, 0, SEEK_SET);
  FILE *fp2 = fopen(FILENAME, "r");

  str = NULL;
  char *str2 = NULL;
  len_read = 0;
  str = fgetln(fp, &len_read);
  for(int i = 0; i < 4; i++)
    str2 = fgetln(fp2, &len_read);
  int element = atoi(str);
  int element_shifted = atoi(str2);
  num_increases = element_shifted > element ? 1 : 0;

  while ((str = fgetln(fp, &len_read)) && (str2 = fgetln(fp2, &len_read)))
    if(atoi(str2) > atoi(str))
      num_increases++;
  printf("%d sums are larger than the previous sum.\n", num_increases);

  fclose(fp);
  fclose(fp2);
  return 0;
}
