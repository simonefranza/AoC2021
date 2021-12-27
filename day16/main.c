//
// Advent of Code 2021
//
// Day 16
//
// Author: Simone Franza
//
// Thought process:
// What is this description? Wtf
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define FILENAME "input"

enum ID_TYPE {
  SUM = 0,
  PRODUCT = 1,
  MIN = 2,
  MAX = 3,
  LITERAL_VALUE = 4,
  GT = 5,
  LT = 6,
  EQ = 7
};

size_t *decodePacket(char *str, int len, size_t *total_values, size_t *read_pos, size_t *header_sum);
size_t decodeLiteralValue(char *str, size_t *val);
size_t readBits(char *str, int num_bits);
void decodeMathOP(size_t *ret, size_t *total_values, int total_len, int num_packets, char *str,
    int *pos, size_t initial_value, size_t op(size_t, size_t), size_t *header_sum);
void decodeComparisonOP(size_t *ret, size_t *total_values, int total_len, int num_packets, 
    char *str, int *pos, size_t op(size_t, size_t), size_t *header_sum);

// Operations
size_t sum(size_t a, size_t b) { return a + b; }
size_t product(size_t a, size_t b) { return a * b; }
size_t min(size_t a, size_t b) { return a < b ? a : b; }
size_t max(size_t a, size_t b) { return a > b ? a : b; }
size_t gt(size_t a, size_t b) { return a > b; }
size_t lt(size_t a, size_t b) { return a < b; }
size_t equal(size_t a, size_t b) { return a == b; }

int main() {
  // Part 1
  FILE *fp = fopen(FILENAME, "r");
  size_t read_len;
  char *str = fgetln(fp, &read_len);
  char *hex = (char *)calloc(sizeof(char), (read_len - 1) * 4);
  for (int i = 0; i < read_len - 1; i++) {
    char el[2] = {str[i], 0};
    int val = strtol(el, NULL, 16);
    for(int j = 0; j < 4; j++) 
      hex[i * 4 + j] = val & (1 << (3 - j)) ? 1 : 0;
  }
  size_t total_values = 0;
  size_t read_pos = 0;
  size_t header_sum = 0;
  size_t *res = decodePacket(hex, (read_len - 1) * 4, &total_values, &read_pos, &header_sum);
  printf("Sum of the headers %zu\n", header_sum);
  printf("The hexadecimal-encoded BITS transmission evaluates to %zu\n", res[0]);

  // Cleanup
  fclose(fp);

  return 0;
}

size_t *decodePacket(char *str, int len, size_t *total_values, size_t *read_pos,
    size_t *header_sum) {
  int pos = 0;
  *read_pos = 0;
  size_t *ret = calloc(sizeof(size_t), 100);
  *total_values = 0;
  while (pos < len || len == -1) {
    int version = str[pos] << 2 | str[pos + 1] << 1 | str[pos + 2];
    *header_sum += version;
    int typeID = str[pos + 3] << 2 | str[pos + 4] << 1 | str[pos + 5];
    pos += 6;
    size_t total_len = -1;
    size_t num_packets = -1;
    if (typeID != LITERAL_VALUE) {
      size_t val = readBits(str + pos + 1, !str[pos] ? 15 : 11);
      if(!str[pos])
        total_len = val;
      else
        num_packets = val;
      pos += !str[pos] ? 16 : 12;
    }
    switch (typeID) {
    case SUM:
      decodeMathOP(ret, total_values, total_len, num_packets, str, &pos, 0, sum, header_sum);
      break;
    case PRODUCT:
      decodeMathOP(ret, total_values, total_len, num_packets, str, &pos, 1, product, header_sum);
      break;
    case MIN:
      decodeMathOP(ret, total_values, total_len, num_packets, str, &pos, 9999999, min, header_sum);
      break;
    case MAX:
      decodeMathOP(ret, total_values, total_len, num_packets, str, &pos, 0, max, header_sum);
      break;
    case LITERAL_VALUE:
      pos += decodeLiteralValue(str + pos, ret + (*total_values)++);
      break;
    case GT:
      decodeComparisonOP(ret, total_values, total_len, num_packets, str, &pos, gt, header_sum);
      break;
    case LT:
      decodeComparisonOP(ret, total_values, total_len, num_packets, str, &pos, lt, header_sum);
      break;
    case EQ:
      decodeComparisonOP(ret, total_values, total_len, num_packets, str, &pos, equal, header_sum);
      break;
    default:
      printf("What is a typeID %d??\n", typeID);
      exit(-1);
    }
    if (len == pos || len == -1) {
      *read_pos = pos;
      return ret;
    }
  }
  *read_pos = pos;
  return ret;
}

size_t decodeLiteralValue(char *str, size_t *val) {
  int pos = 0;
  size_t arr[100] = {0};
  int arr_pos = 0;
  int existNext;
  do {
    existNext = str[pos];
    arr[arr_pos++] = str[pos + 1] << 3 | str[pos + 2] << 2 | str[pos + 3] << 1 |
                     str[pos + 4];
    pos += 5;
  } while(existNext);

  *val = 0;
  for (int i = 0; i < arr_pos; i++)
    *val += arr[i] << 4 * (arr_pos - 1 - i);
  return pos;
}

size_t readBits(char *str, int num_bits) {
  size_t val = 0;
  for (int i = 0; i < num_bits; i++)
    val += str[i] << (num_bits - 1 - i);
  return val;
}

void decodeMathOP(size_t *ret, size_t *total_values, int total_len, int num_packets, 
    char *str, int *pos, size_t initial_value, size_t op(size_t, size_t), size_t *header_sum) {
  size_t read_num;
  size_t intern_total_values;
  ret[*total_values] = initial_value;

  if (total_len != -1) {
    size_t *res = decodePacket(str + *pos, total_len, &intern_total_values,
        &read_num, header_sum);
    *pos += read_num;
    for (int i = 0; i < intern_total_values; i++) {
      ret[*total_values] = op(ret[*total_values], res[i]);
    }
    (*total_values)++;
    return;
  }
  // Got num of packets
  for (int i = 0; i < num_packets; i++) {
    ret[*total_values] = op(*decodePacket(str + (*pos), -1, &intern_total_values,
        &read_num, header_sum), ret[*total_values]);
    *pos += read_num;
  }
  (*total_values)++;
}

void decodeComparisonOP(size_t *ret, size_t *total_values, int total_len, int num_packets, 
    char *str, int *pos, size_t op(size_t, size_t), size_t *header_sum) {
  size_t read_num;
  size_t intern_total_values;

  if (total_len != -1) {
    size_t *res = decodePacket(str + *pos, total_len, &intern_total_values,
        &read_num, header_sum);
    *pos += read_num;
    assert(intern_total_values == 2 && "Got more than 2 values for comparison operator");
    ret[(*total_values)++] = op(res[0], res[1]);
    return;
  }
  // Got num of packets
  assert(num_packets == 2 && "Waiting for something other than 2 values for comparison operator");
  size_t res[2] = {0};
  for (int i = 0; i < num_packets; i++) {
    res[i] = *decodePacket(str + (*pos), -1, &intern_total_values, &read_num, header_sum);
    *pos += read_num;
  }
  ret[(*total_values)++] = op(res[0], res[1]);
}
