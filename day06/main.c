//
// Advent of Code 2021
//
// Day 6
//
// Author: Simone Franza
//
// Thought process:
// Use array, each element is a single lanternfish. Each turn save how many
// lanternfishes will spawn and add them at the end of the loop
// This idea failed for part 2 since the array was getting quite huge (1976596756
// fishes for the example input on the website)
// Instead I'm gonna have an array with 9 entries, where each one says how many
// fishes there are with that count
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "input"
#define NUM_DAYS 80
#define NUM_DAYS_PART_2 256 
#define ARR_SIZE 100

int main() {
  // Part 1
  FILE *fp = fopen(FILENAME, "r");
  size_t read_len;
  char *str = fgetln(fp, &read_len);
  char *copy = (char *)calloc(sizeof(char), read_len + 1);
  memcpy(copy, str, read_len);
  char *token = strtok(copy, ",");
  uint8_t *lanternfishes = (uint8_t *)calloc(sizeof(uint8_t), ARR_SIZE);
  size_t num_lanternfishes = 0;
  size_t max_lanternfishes = ARR_SIZE;
  while (token) {
    lanternfishes[num_lanternfishes++] = atoi(token);
    if (num_lanternfishes == max_lanternfishes) {
      uint8_t *temp = realloc(lanternfishes, num_lanternfishes + ARR_SIZE);
      if (!temp) {
        free(copy);
        free(lanternfishes);
        fclose(fp);
        return -1;
      }
      lanternfishes = temp;
      max_lanternfishes += ARR_SIZE;
    }
    token = strtok(NULL, ",");
  }

  for (int day = 0; day < NUM_DAYS; day++) {
    int to_add = 0;
    for (int fish = 0; fish < num_lanternfishes; fish++) {
      if (lanternfishes[fish])
        lanternfishes[fish]--;
      else {
        lanternfishes[fish] = 6;
        to_add++;
      }
    }
    if (num_lanternfishes + to_add >= max_lanternfishes) {
      uint8_t *temp = realloc(lanternfishes, sizeof(uint8_t)*(num_lanternfishes + to_add + ARR_SIZE));
      if (!temp) {
        free(copy);
        free(lanternfishes);
        fclose(fp);
        return -1;
      }
      lanternfishes = temp;
      max_lanternfishes += ARR_SIZE + to_add;
    }
    for(int fish = num_lanternfishes; fish < num_lanternfishes + to_add; fish++)
      lanternfishes[fish] = 8;
    num_lanternfishes += to_add;
  }
  printf("After %d days there are %zu lanternfishes.\n", NUM_DAYS, num_lanternfishes);

  // Part 2
  fseek(fp, 0, SEEK_SET);
  size_t data[9] = {0};
  memcpy(copy, str, read_len);
  token = strtok(copy, ",");
  while (token) {
    data[atoi(token)]++;
    token = strtok(NULL, ",");
  }

  for(int day = 0; day < NUM_DAYS_PART_2; day++) {
    size_t to_add = data[0];
    for(int i = 1; i < 9; i++) {
      data[i - 1] = data[i];
    }
    data[6] += to_add;
    data[8] = to_add;
  }
  num_lanternfishes = 0;
  for(int i = 0; i < 9; i++)
    num_lanternfishes += (size_t)data[i];

  printf("After %d days there are %zu lanternfishes.\n", NUM_DAYS_PART_2, num_lanternfishes);

  // Cleanup
  free(copy);
  free(lanternfishes);
  fclose(fp);
  return 0;
}
