//
// Advent of Code 2021
//
// Day 13
//
// Author: Simone Franza
//
// Thought process:
// Create string, move by one and consider 2 chars, create new string with new
// chars repeat
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "input"
#define STEPS_PT_1 10
#define STEPS_PT_2 40
#define STRING_SIZE 21
#define ARR_SIZE 200
#define A_VAL 65

char *readStartString(FILE *fp);
char *readMappings(FILE *fp);
size_t *performSubstitutions(char *str, char *map, int steps);
void findMinMax(size_t *pairs, char first_char, char last_char);

int main(int argv, char *argc[]) {
  // Part 1
  FILE *fp = fopen(FILENAME, "r");
  char *str = readStartString(fp);
  char first_char = str[0];
  int len = 0;
  while(*(str + ++len));
  char last_char = str[len-1];
  size_t read_len;
  fgetln(fp, &read_len);
  char *map = readMappings(fp);
  size_t *subbed = performSubstitutions(str, map, STEPS_PT_1);
  findMinMax(subbed, first_char, last_char);

  // Part 2
  fseek(fp, 0, SEEK_SET);
  str = readStartString(fp);
  subbed = performSubstitutions(str, map, STEPS_PT_2);
  findMinMax(subbed, first_char, last_char);

  // Cleanup
  fclose(fp);
  free(subbed);
  free(str);

  return 0;
}

char *readStartString(FILE *fp) {
  char *res = calloc(sizeof(char), STRING_SIZE);
  size_t read_len;
  char *str = fgetln(fp, &read_len);
  memcpy(res, str, read_len - 1);
  return res;
}

char *readMappings(FILE *fp) {
  char *str;
  size_t read_len;
  char *data = calloc(sizeof(char), 26 * 26);
  while ((str = fgetln(fp, &read_len))) {
    char *copy = calloc(sizeof(char), read_len);
    memcpy(copy, str, read_len - 1);
    char *from = strtok(copy, " -> ");
    char *to = strtok(NULL, " -> ");
    data[(from[0] - A_VAL) * 26 + (from[1] - A_VAL)] = to[0];
  }
  return data;
}

size_t *performSubstitutions(char *str, char *map, int steps) {
  size_t str_len = 0;
  while (*(str + ++str_len));
  size_t *pairs = calloc(sizeof(size_t), 26 * 26);
  for (int i = 0; i < str_len - 1; i++) {
    char from = str[i] - A_VAL;
    char to = str[i + 1] - A_VAL;
    pairs[from * 26 + to]++;
  }

  for (int step = 0; step < steps; step++) {
    size_t *new_pairs = calloc(sizeof(size_t), 26 * 26);
    for (int first = 0; first < 26; first++) {
      for (int second = 0; second < 26; second++) {
        if (!pairs[first * 26 + second])
          continue;
        char to = map[first * 26 + second] - A_VAL;
        new_pairs[first * 26 + to] += pairs[first * 26 + second];
        new_pairs[to * 26 + second] += pairs[first * 26 + second];
      }
    }
    free(pairs);
    pairs = new_pairs;
  }
  size_t num_vals = 0;
  for (int first = 0; first < 26; first++)
    for (int second = 0; second < 26; second++)
      num_vals += pairs[first * 26 + second];

  return pairs;
}

void findMinMax(size_t *pairs, char first_char, char last_char) {
  size_t freq[26] = {0};
  for (int first = 0; first < 26; first++) {
    for (int second = 0; second < 26; second++) {
      if (!pairs[first * 26 + second])
        continue;
      freq[first] += pairs[first * 26 + second];
      freq[second] += pairs[first * 26 + second];
    }
  }
  freq[first_char - A_VAL]++;
  freq[last_char - A_VAL]++;
  ssize_t min = -1;
  ssize_t max = -1;
  for (int i = 0; i < 26; i++) {
    if (freq[i] > max || max == -1)
      max = freq[i];
    if (freq[i] && (freq[i] < min || min == -1))
      min = freq[i];
  }

  printf("Difference between most and least frequent is: %zu\n", (max - min) / 2);
}
