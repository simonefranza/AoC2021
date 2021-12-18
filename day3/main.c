//
// Advent of Code 2021
//
// Day 3
//
// Author: Simone Franza
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "input"
#define NUM_BIT 12

int main() {
  // Part 1
  FILE *fp = fopen(FILENAME, "r");
  char *str = NULL;
  int num_el = 0;
  int frequencies[NUM_BIT] = {0};
  size_t len_read = 0;

  while ((str = fgetln(fp, &len_read))) {
    for (int i = 0; i < NUM_BIT; i++) {
      frequencies[i] += str[i] == '1' ? 1 : 0;
    }
    num_el++;
  }
  int gamma_rate = 0;
  int epsilon_rate = 0;
  for (int i = 0; i < NUM_BIT; i++) {
    gamma_rate |= frequencies[i] > (num_el / 2.0) ? 1 << (NUM_BIT - 1 - i) : 0;
    epsilon_rate |= (!(gamma_rate & (1 << (NUM_BIT - 1 - i))))
                    << (NUM_BIT - 1 - i);
  }
  printf("The power consumption is %d\n", gamma_rate * epsilon_rate);

  // Part 2
  // Note: I could've saved only the strings
  fseek(fp, 0, SEEK_SET);
  int *oxygen_data = (int *)calloc(sizeof(int), num_el);
  int *CO2_data = (int *)calloc(sizeof(int), num_el);
  int data_len = 0;
  while ((str = fgetln(fp, &len_read))) {
    for (int i = 0; i < NUM_BIT; i++) {
      oxygen_data[data_len] |= str[i] == '1' ? 1 << (NUM_BIT - 1 - i) : 0;
    }
    CO2_data[data_len] = oxygen_data[data_len];
    data_len++;
  }
  int oxygen_data_left = data_len;
  int CO2_data_left = data_len;
  int oxygen_last = 0;
  int CO2_last = 0;

  for (int bit = 0; bit < NUM_BIT && (oxygen_data_left > 1 || CO2_data_left > 1); bit++) {
    // Recompute frequencies
    int oxygen_ones_frequency = 0;
    int CO2_ones_frequency = 0;
    for (int el_idx = 0; el_idx < num_el; el_idx++) {
      if (oxygen_data[el_idx] != -1)
        oxygen_ones_frequency += oxygen_data[el_idx] & (1 << (NUM_BIT - 1 - bit)) ? 1 : 0;

      if (CO2_data[el_idx] != -1)
        CO2_ones_frequency += CO2_data[el_idx] & (1 << (NUM_BIT - 1 - bit)) ? 1 : 0;
    }

    int oxygen_most_frequent = oxygen_ones_frequency >= (oxygen_data_left/ 2.0) ? 1 : 0;
    int CO2_most_frequent = CO2_ones_frequency < (CO2_data_left / 2.0) ? 1 : 0;
    for (int el_idx = 0; el_idx < num_el; el_idx++) {
      if (oxygen_data[el_idx] != -1 && oxygen_data_left > 1) {
        if (!!(oxygen_data[el_idx] & (1 << (NUM_BIT - 1 - bit))) != oxygen_most_frequent) {
          oxygen_data[el_idx] = -1;
          oxygen_data_left--;
        } else
          oxygen_last = oxygen_data[el_idx];
      }

      if (CO2_data[el_idx] != -1 && CO2_data_left > 1) {
        if (!!(CO2_data[el_idx] & (1 << (NUM_BIT - 1 - bit))) != CO2_most_frequent) {
          CO2_data[el_idx] = -1;
          CO2_data_left--;
        } else 
          CO2_last = CO2_data[el_idx];
        
      }
    }
  }
  printf("Life support rating %d\n", oxygen_last * CO2_last);
  fclose(fp);
  return 0;
}
