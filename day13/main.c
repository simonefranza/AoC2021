//
// Advent of Code 2021
//
// Day 13
//
// Author: Simone Franza
//
// Thought process:
// Folding is only a reflection so save data in array  and then do reflections
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "input"
#define ARR_SIZE 100

typedef struct Pt_ {
  int x;
  int y;
} Pt;

typedef struct Fold_ {
  char axis;
  int val;
} Fold;

Pt *readData(Pt **data, int *num_data, int *max_data, Fold **folds,
             int *num_folds, int *max_folds);
void printData(uint8_t *data, Pt *max_coord, int hide_points);
void printFold(uint8_t *data, Pt *max_coord, Fold *fold);
uint8_t *gen2DArray(Pt *data, int num_data, Pt *max_coord);
void foldData(uint8_t **data, Pt *max_coord, Fold *fold);
int countPoints(uint8_t *data, Pt *max_coord);

int main(int argv, char *argc[]) {
  int enable_out = 0;
  // Enable output
  if (argv == 2 && !strcmp(argc[1], "-v"))
    enable_out = 1;
  // Part 1
  Pt *sparse = calloc(sizeof(Pt), ARR_SIZE);
  int max_data = ARR_SIZE;
  int num_data = 0;
  Fold *folds = calloc(sizeof(Fold), ARR_SIZE);
  int max_folds = ARR_SIZE;
  int num_folds = 0;
  Pt *max_coord =
      readData(&sparse, &num_data, &max_data, &folds, &num_folds, &max_folds);
  uint8_t *data = gen2DArray(sparse, num_data, max_coord);
  if (enable_out) {
    printData(data, max_coord, 0);
    printFold(data, max_coord, folds);
  }
  foldData(&data, max_coord, folds);
  if (enable_out)
    printData(data, max_coord, 0);
  printf("There are a total of %d points\n", countPoints(data, max_coord));

  //Part 2
  for(int i = 1; i < num_folds; i++) {
    if (enable_out) {
      printData(data, max_coord, 0);
      printFold(data, max_coord, folds + i);
    }
    foldData(&data, max_coord, folds + i);
    if (enable_out)
      printData(data, max_coord, 0);
  }
  printData(data, max_coord, 1);

  return 0;
}
Pt *readData(Pt **data, int *num_data, int *max_data, Fold **folds,
             int *num_folds, int *max_folds) {
  Pt *max_coord = calloc(sizeof(Pt), 1);
  FILE *fp = fopen(FILENAME, "r");
  size_t read_len = 0;
  char *str = fgetln(fp, &read_len);
  char *copy = calloc(sizeof(char), read_len);
  memcpy(copy, str, read_len - 1);
  while (strcmp(copy, "")) {
    char *sep = ",";
    char *x = strtok(copy, sep);
    (*data)[*num_data].x = (int)atoi(x);
    char *y = strtok(NULL, "\n");
    (*data)[*num_data].y = (int)atoi(y);
    if ((*data)[*num_data].x > max_coord->x)
      max_coord->x = (*data)[*num_data].x;
    if ((*data)[*num_data].y > max_coord->y)
      max_coord->y = (*data)[*num_data].y;
    *num_data += 1;
    if (*num_data == *max_data) {
      *data = realloc(*data, sizeof(Pt) * (*num_data + ARR_SIZE));
      *max_data += ARR_SIZE;
    }
    free(copy);
    str = fgetln(fp, &read_len);
    copy = calloc(sizeof(char), read_len);
    memcpy(copy, str, read_len - 1);
  }
  free(copy);
  str = fgetln(fp, &read_len);
  copy = calloc(sizeof(char), read_len);
  memcpy(copy, str, read_len - 1);
  while (copy) {
    strtok(copy, " ");
    strtok(NULL, " ");
    char *axis = strtok(NULL, "=");
    (*folds)[*num_folds].axis = axis[0];
    char *val = strtok(NULL, "");
    (*folds)[(*num_folds)++].val = atoi(val);
    if (*num_folds == *max_folds) {
      *folds = realloc(*folds, sizeof(Fold) * (*num_folds + ARR_SIZE));
      *max_folds += ARR_SIZE;
    }
    free(copy);
    str = fgetln(fp, &read_len);
    if (!str)
      break;
    copy = calloc(sizeof(char), read_len);
    memcpy(copy, str, read_len - 1);
  }
  max_coord->x++;
  max_coord->y++;
  return max_coord;
}

void printData(uint8_t *data, Pt *max_coord, int hide_points) {
  putchar('\n');
  char *void_pt = hide_points ? " " : ".";
  char *pt = hide_points ? "█" : "#";
  for (int row = 0; row < max_coord->y; row++) {
    for (int col = 0; col < max_coord->x; col++)
      printf("%s", data[col + row * max_coord->x] ? pt : void_pt);
    putchar('\n');
  }
}

uint8_t *gen2DArray(Pt *data, int num_data, Pt *max_coord) {
  uint8_t *new_data = calloc(sizeof(uint8_t), max_coord->x * max_coord->y);
  for (int i = 0; i < num_data; i++) {
    new_data[(data[i].x + data[i].y * max_coord->x)] = 1;
  }
  return new_data;
}

void foldData(uint8_t **data, Pt *max_coord, Fold *fold) {
  int new_max_x = 0;
  int new_max_y = 0;
  int *to_change;
  if (fold->axis == 'y') {
    for (int row = fold->val + 1; row < max_coord->y; row++) {
      for (int col = 0; col < max_coord->x; col++) {
        if (!(*data)[col + row * max_coord->x])
          continue;
        int new_y = fold->val - (row - fold->val);
        (*data)[col + new_y * max_coord->x] = 1;
      }
    }
    to_change = &max_coord->y;
    new_max_x = max_coord->x;
    new_max_y = fold->val;
  } else {
    for (int row = 0; row < max_coord->y; row++) {
      for (int col = fold->val + 1; col < max_coord->x; col++) {
        if (!(*data)[col + row * max_coord->x])
          continue;
        int new_x = fold->val - (col - fold->val);
        (*data)[new_x + row * max_coord->x] = 1;
      }
    }
    to_change = &max_coord->x;
    new_max_x = fold->val;
    new_max_y = max_coord->y;
  }
  uint8_t *temp = calloc(sizeof(uint8_t), new_max_x * new_max_y);
  for (int row = 0; row < new_max_y; row++) {
    for (int col = 0; col < new_max_x; col++) {
      temp[col + row * new_max_x] = (*data)[col + row * max_coord->x];
    }
  }
  *to_change = fold->val;
  free(*data);
  *data = temp;
}

void printFold(uint8_t *data, Pt *max_coord, Fold *fold) {
  printf("\nFold @ %c,%d\n", fold->axis, fold->val);
  for (int row = 0; row < max_coord->y; row++) {
    for (int col = 0; col < max_coord->x; col++) {
      if (fold->axis == 'x' && fold->val == col)
        printf("|");
      else if (fold->axis == 'y' && fold->val == row)
        printf("-");
      else
        printf("%c", data[col + row * max_coord->x] ? '#' : '.');
    }
    putchar('\n');
  }
}

int countPoints(uint8_t *data, Pt *max_coord) {
  int res = 0;
  for (int row = 0; row < max_coord->y; row++)
    for (int col = 0; col < max_coord->x; col++)
      res += data[col + row * max_coord->x] ? 1 : 0;
  return res;
}
