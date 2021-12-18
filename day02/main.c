//
// Advent of Code 2021
//
// Day 2
//
// Author: Simone Franza
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "input"

void parseLine2(char *line, int *depth, int *horizontal, int *aim) {
  char command[10] = {0}; 
  int value = 0;
  sscanf(line, "%s %d\n", command, &value);
  if(!strcmp(command, "forward")) {
    *horizontal += value;
    *depth += *aim * value;
  }
  else if(!strcmp(command, "down"))
    *aim += value;
  else if(!strcmp(command, "up"))
    *aim -= value;
}

void parseLine(char *line, int *depth, int *horizontal) {
  char command[10] = {0}; 
  int value = 0;
  sscanf(line, "%s %d\n", command, &value);
  if(!strcmp(command, "forward"))
    *horizontal += value;
  else if(!strcmp(command, "down"))
    *depth += value;
  else if(!strcmp(command, "up"))
    *depth -= value;
}

int main() {
  // Part 1
  FILE *fp = fopen(FILENAME, "r");
  char *str = NULL;
  size_t len_read = 0;
  int depth = 0;
  int horizontal = 0;
  while ((str = fgetln(fp, &len_read))) 
    parseLine(str, &depth, &horizontal);
  printf("Final horizontal position multiplied by final depth %d\n", depth*horizontal);

  // Part 2
  fseek(fp, 0, SEEK_SET);
  depth = 0;
  horizontal = 0;
  int aim = 0;
  while ((str = fgetln(fp, &len_read))) 
    parseLine2(str, &depth, &horizontal, &aim);
  printf("Final horizontal position multiplied by final depth %d\n", depth*horizontal);

  fclose(fp);
  return 0;
}
