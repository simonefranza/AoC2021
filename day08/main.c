//
// Advent of Code 2021
//
// Day 8
//
// Author: Simone Franza
//
// Thought process:
// For now ignore part before | and count how many elements have 2,3,4,7 letters
// Part 2:
// Use 8 bit number to represent 7 segments display, LSB is 0
//  -5-
// |   |
// 4   0
// |   |
//  -6-
// |   |
// 3   1
// |   |
//  -2-
//

#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "input"
#define ARR_SIZE 100

enum possibleValues {
  ZERO_OR_SIX_OR_NINE = 6,
  ONE = 2,
  TWO_OR_THREE_OR_FIVE = 5,
  FOUR = 4,
  SEVEN = 3,
  EIGHT = 7,
};
typedef enum SEGMENTS_ {
  TOP_RIGHT = 0,
  BOTTOM_RIGHT = 1,
  BOTTOM = 2,
  BOTTOM_LEFT = 3,
  TOP_LEFT = 4,
  TOP = 5,
  MID = 6
} SEGMENTS;

typedef struct symbol_ {
  char letter;
  SEGMENTS *possibilities;
  int num_poss;
} symbol;

typedef struct encoding_ {
  symbol *sym;
  int len;
  int value;
} encoding;

int solveLine(char *line);
int parseSevenSegment(int arr[7]);
int findLetterInEncoding(encoding *enc, char letter);
int findLetter(char *str, char letter, int len);
void printMappings(symbol *mappings);
void addEncoding(encoding *enc, int value, char *token, int num_letters,
                 int num_seg, ...);
void shiftValidValues(symbol *el, int removed_el);
void createEncodings(encoding *encodings, char *token, char *one_letters,
                     int *len_one_idx, char *four_letters, int *len_four_idx,
                     char *seven_letters, int *len_seven_idx);
void reduceMappings(encoding *encodings, symbol *mappings);
int decodeToken(char *token, symbol *mappings, char *pair_double);

int main() {
  // Part 1
  FILE *fp = fopen(FILENAME, "r");
  size_t read_len;
  char *str;
  int num = 0;
  while ((str = fgetln(fp, &read_len))) {
    char *copy = (char *)calloc(sizeof(char), read_len + 1);
    memcpy(copy, str, read_len);
    char *token = strtok(copy, "|");
    token = strtok(NULL, " ");
    while (token) {
      int len = 0;
      while (*(token + ++len) && *(token + len) != '\n')
        ;
      if (len == 2 || len == 3 || len == 4 || len == 7)
        num++;
      token = strtok(NULL, " ");
    }
  }
  printf("Digits 1, 4, 7, or 8 appear a total of %d times.\n", num);

  // Part 2
  fseek(fp, 0, SEEK_SET);
  int sum = 0;
  while ((str = fgetln(fp, &read_len))) {
    char *copy = (char *)calloc(sizeof(char), read_len + 1);
    memcpy(copy, str, read_len);
    sum += solveLine(copy);
    free(copy);
  }
  printf("Sum of all outputs: %d\n", sum);

  // Cleanup
  fclose(fp);

  return 0;
}

int solveLine(char *line) {
  printf("Line %s\n", line);
  char *token = strtok(line, " ");
  encoding encodings[10] = {0};

  int len_one_idx = -1;
  char one_letters[2] = {0};
  int len_four_idx = -1;
  char four_letters[4] = {0};
  int len_seven_idx = -1;
  char seven_letters[3] = {0};

  createEncodings(encodings, token, one_letters, &len_one_idx, four_letters,
                  &len_four_idx, seven_letters, &len_seven_idx);

  // Create mappings
  symbol mappings[7] = {0};
  for (int i = 0; i < 7; i++) {
    mappings[i].letter = 97 + i;
    mappings[i].num_poss = 7;
    mappings[i].possibilities = calloc(sizeof(SEGMENTS), 7);
    for (int j = 0; j < 7; j++)
      mappings[i].possibilities[j] = (SEGMENTS)j;
  }

  reduceMappings(encodings, mappings);

  // Remove positions that appear in the letters of numer 1
  for (int j = 0; j < 7; j++) {
    if (mappings[j].letter == one_letters[0] ||
        mappings[j].letter == one_letters[1])
      continue;
    int removed_el = 0;
    for (int k = 0; k < mappings[j].num_poss; k++) {
      if (mappings[j].possibilities[k] ==
              encodings[len_one_idx].sym[0].possibilities[0] ||
          mappings[j].possibilities[k] ==
              encodings[len_one_idx].sym[0].possibilities[1]) {
        mappings[j].possibilities[k] = -1;
        removed_el++;
      }
    }
    shiftValidValues(mappings + j, removed_el);
  }

  // Remove positions that appear in the extra (extra wrt to the letters
  // appearing in the value 1) letter of numer 7
  char seven_extra_letter = 0;
  for (int i = 0; i < 3; i++) {
    if (seven_letters[i] != one_letters[0] &&
        seven_letters[i] != one_letters[1])
      seven_extra_letter = encodings[len_seven_idx].sym[i].letter;
  }

  for (int j = 0; j < 7; j++) {
    if (mappings[j].letter == seven_extra_letter)
      continue;
    int removed_el = 0;
    for (int k = 0; k < mappings[j].num_poss; k++) {
      if (mappings[j].possibilities[k] ==
          mappings[seven_extra_letter - 97].possibilities[0]) {
        mappings[j].possibilities[k] = -1;
        removed_el++;
      }
    }
    shiftValidValues(mappings + j, removed_el);
  }

  // Remove positions that appear in the extra (extra wrt to the letters
  // appearing in the value 1) letter of numer 4
  char four_extra_letters[2] = {0};
  int pos = 0;
  for (int i = 0; i < 4; i++) {
    if (four_letters[i] != one_letters[0] && four_letters[i] != one_letters[1])
      four_extra_letters[pos++] = encodings[len_four_idx].sym[i].letter;
  }

  for (int j = 0; j < 7; j++) {
    if (mappings[j].letter == four_extra_letters[0] ||
        mappings[j].letter == four_extra_letters[1])
      continue;
    int removed_el = 0;
    for (int k = 0; k < mappings[j].num_poss; k++) {
      if (mappings[j].possibilities[k] ==
              mappings[four_extra_letters[0] - 97].possibilities[0] ||
          mappings[j].possibilities[k] ==
              mappings[four_extra_letters[0] - 97].possibilities[1]) {
        mappings[j].possibilities[k] = -1;
        removed_el++;
      }
    }
    shiftValidValues(mappings + j, removed_el);
  }

  char last_pair[2] = {0};
  int last_idx = 0;
  for (int i = 0; i < 7; i++) {
    if (i + 97 != one_letters[0] && i + 97 != one_letters[1] &&
        i + 97 != four_extra_letters[0] && i + 97 != four_extra_letters[1] &&
        i + 97 != seven_extra_letter)
      last_pair[last_idx++] = i + 97;
  }
  // With the help of encodings of length 6 we can solve 2 mappings pairs
  int old_six_idx = -1;
  int len_six_idx = -1;
  for (int repetition = 0; repetition < 2; repetition++) {
    // Find index of encoding of length 6
    for (int i = 0; i < 10; i++)
      if (encodings[i].len == 6 && i != old_six_idx)
        len_six_idx = i;

    int arr[7] = {0};
    // Determine which pair of letters is not fully included in the token
    int one_letters_present =
        findLetterInEncoding(encodings + len_six_idx, one_letters[0]) &&
        findLetterInEncoding(encodings + len_six_idx, one_letters[1]);
    int four_extra_letters_present =
        findLetterInEncoding(encodings + len_six_idx, four_extra_letters[0]) &&
        findLetterInEncoding(encodings + len_six_idx, four_extra_letters[1]);
    int seven_letter_present =
        findLetterInEncoding(encodings + len_six_idx, seven_extra_letter);
    char letter_not_present = 0;

    for (int i = 0; i < 7; i++)
      if (!findLetterInEncoding(encodings + len_six_idx, 97 + i))
        letter_not_present = i + 97;

    int last_pair_present =
        findLetterInEncoding(encodings + len_six_idx, last_pair[0]) &&
        findLetterInEncoding(encodings + len_six_idx, last_pair[1]);
    char *arr_to_check;
    if (!one_letters_present)
      arr_to_check = one_letters;
    else if (!four_extra_letters_present)
      arr_to_check = four_extra_letters;
    else if (!last_pair_present)
      arr_to_check = last_pair;

    // Set all other values to 1
    for (int i = 0; i < 7; i++) {
      if (i == mappings[arr_to_check[0] - 97].possibilities[0] ||
          i == mappings[arr_to_check[1] - 97].possibilities[1] ||
          i == mappings[letter_not_present - 97].possibilities[0])
        continue;
      arr[i] = 1;
    }
    char letter_of_pair_present = letter_not_present == arr_to_check[0]
                                      ? arr_to_check[1]
                                      : arr_to_check[0];
    arr[mappings[arr_to_check[0] - 97].possibilities[0]] = 1;
    if (parseSevenSegment(arr) != -1) {
      mappings[letter_of_pair_present - 97].num_poss = 1;
      mappings[letter_not_present - 97].num_poss = 1;
      mappings[letter_not_present - 97].possibilities[0] =
          mappings[letter_of_pair_present - 97].possibilities[1];
    } else {
      arr[mappings[arr_to_check[0] - 97].possibilities[0]] = 0;
      arr[mappings[arr_to_check[0] - 97].possibilities[1]] = 1;
      if (parseSevenSegment(arr) != -1) {
        mappings[letter_of_pair_present - 97].num_poss = 1;
        mappings[letter_of_pair_present - 97].possibilities[0] =
            mappings[letter_of_pair_present - 97].possibilities[1];
        mappings[letter_not_present - 97].num_poss = 1;
      }
    }
    old_six_idx = len_six_idx;
  }
  // Print resulting mappings
  printMappings(mappings);

  // Save output tokens
  char *tokens[4] = {0};
  token = strtok(NULL, " | ");
  tokens[0] = token;
  int token_pos = 1;
  while ((token = strtok(NULL, " "))) {
    tokens[token_pos++] = token;
  }

  // Find pair which still has more than one possibility
  char pair_double[2] = {0};
  int pair_pos = 0;
  for (int i = 0; i < 7; i++) {
    if (mappings[i].num_poss > 1)
      pair_double[pair_pos++] = i + 97;
  }

  // Finally reconstruct the value
  int res = 0;
  for (int tok_idx = 0; tok_idx < 4; tok_idx++) {
    res += pow(10, 3 - tok_idx) *
           decodeToken(tokens[tok_idx], mappings, pair_double);
  }
  printf("Output decoded: %d\n\n", res);
  return res;
}

int findLetter(char *str, char letter, int len) {
  for (int i = 0; i < len; i++) {
    if (str[i] == letter)
      return 1;
  }
  return 0;
}

int findLetterInEncoding(encoding *enc, char letter) {
  for (int i = 0; i < enc->len; i++) {
    if (enc->sym[i].letter == letter)
      return 1;
  }
  return 0;
}

int parseSevenSegment(int arr[7]) {
  if (arr[0] && arr[1] && arr[2] && arr[3] && arr[4] && arr[5] && !arr[6])
    return 0;
  else if (arr[0] && arr[1] && !arr[2] && !arr[3] && !arr[4] && !arr[5] &&
           !arr[6])
    return 1;
  else if (arr[0] && !arr[1] && arr[2] && arr[3] && !arr[4] && arr[5] && arr[6])
    return 2;
  else if (arr[0] && arr[1] && arr[2] && !arr[3] && !arr[4] && arr[5] && arr[6])
    return 3;
  else if (arr[0] && arr[1] && !arr[2] && !arr[3] && arr[4] && !arr[5] &&
           arr[6])
    return 4;
  else if (!arr[0] && arr[1] && arr[2] && !arr[3] && arr[4] && arr[5] && arr[6])
    return 5;
  else if (!arr[0] && arr[1] && arr[2] && arr[3] && arr[4] && arr[5] && arr[6])
    return 6;
  else if (arr[0] && arr[1] && !arr[2] && !arr[3] && !arr[4] && arr[5] &&
           !arr[6])
    return 7;
  else if (arr[0] && arr[1] && arr[2] && arr[3] && arr[4] && arr[5] && arr[6])
    return 8;
  else if (arr[0] && arr[1] && arr[2] && !arr[3] && arr[4] && arr[5] && arr[6])
    return 9;
  return -1;
}

void printMappings(symbol *mappings) {
  printf("MAPPINGS\n");
  for (int i = 0; i < 7; i++) {
    printf("Letter %c (%d mappings): ", mappings[i].letter,
           mappings[i].num_poss);
    for (int j = 0; j < mappings[i].num_poss; j++)
      printf("%d ", mappings[i].possibilities[j]);
    putchar('\n');
  }
}

void addEncoding(encoding *enc, int value, char *token, int num_letters,
                 int num_seg, ...) {
  enc->value = value;
  enc->sym = calloc(sizeof(symbol), num_letters);
  SEGMENTS *segs = calloc(sizeof(SEGMENTS), num_seg);
  va_list ap;
  va_start(ap, num_seg);
  for (int i = 0; i < num_seg; i++)
    segs[i] = va_arg(ap, SEGMENTS);
  va_end(ap);

  for (int j = 0; j < num_letters; j++) {
    enc->sym[j].num_poss = num_seg;
    enc->sym[j].letter = token[j];
    enc->sym[j].possibilities = calloc(sizeof(SEGMENTS), num_seg);
    for (int i = 0; i < num_seg; i++) {
      enc->sym[j].possibilities[i] = segs[i];
    }
  }
}

void shiftValidValues(symbol *el, int removed_el) {
  int idx = 0;
  for (int k = 0; k < el->num_poss; k++) {
    if (el->possibilities[k] == -1)
      continue;
    el->possibilities[idx++] = el->possibilities[k];
  }
  el->num_poss -= removed_el;
}

void createEncodings(encoding *encodings, char *token, char *one_letters,
                     int *len_one_idx, char *four_letters, int *len_four_idx,
                     char *seven_letters, int *len_seven_idx) {
  for (int i = 0; i < 10; i++) {
    int len = 0;
    while (*(token + ++len) && *(token + len) != '\n')
      ;
    encodings[i].len = len;
    switch (len) {
    case ZERO_OR_SIX_OR_NINE:
      addEncoding(encodings + i, -1, token, len, 7, TOP_RIGHT, BOTTOM_RIGHT,
                  BOTTOM, BOTTOM_LEFT, TOP_LEFT, TOP, MID);
      break;
    case ONE:
      addEncoding(encodings + i, 1, token, len, len, TOP_RIGHT, BOTTOM_RIGHT);
      *len_one_idx = i;
      one_letters[0] = encodings[i].sym[0].letter;
      one_letters[1] = encodings[i].sym[1].letter;
      break;
    case TWO_OR_THREE_OR_FIVE:
      addEncoding(encodings + i, -1, token, len, 7, TOP_RIGHT, BOTTOM_RIGHT,
                  BOTTOM, BOTTOM_LEFT, TOP_LEFT, TOP, MID);
      break;
    case FOUR:
      addEncoding(encodings + i, 4, token, len, len, TOP_RIGHT, BOTTOM_RIGHT,
                  MID, TOP_LEFT);
      *len_four_idx = i;
      four_letters[0] = encodings[i].sym[0].letter;
      four_letters[1] = encodings[i].sym[1].letter;
      four_letters[2] = encodings[i].sym[2].letter;
      four_letters[3] = encodings[i].sym[3].letter;
      break;
    case SEVEN:
      addEncoding(encodings + i, 7, token, len, len, TOP_RIGHT, BOTTOM_RIGHT,
                  TOP);
      *len_seven_idx = i;
      seven_letters[0] = encodings[i].sym[0].letter;
      seven_letters[1] = encodings[i].sym[1].letter;
      seven_letters[2] = encodings[i].sym[2].letter;
      break;
    case EIGHT:
      addEncoding(encodings + i, 8, token, len, len, TOP_RIGHT, BOTTOM_RIGHT,
                  BOTTOM, BOTTOM_LEFT, TOP_LEFT, TOP, MID);
      break;
    default:
      printf("What\n");
    }
    token = strtok(NULL, " ");
  }
}

void reduceMappings(encoding *encodings, symbol *mappings) {
  for (int enc_idx = 0; enc_idx < 10; enc_idx++) {
    for (int sym_idx = 0; sym_idx < encodings[enc_idx].len; sym_idx++) {
      if (encodings[enc_idx].sym[sym_idx].num_poss == 7)
        continue;
      char letter = encodings[enc_idx].sym[sym_idx].letter;
      SEGMENTS *values = encodings[enc_idx].sym[sym_idx].possibilities;
      int num_values = encodings[enc_idx].sym[sym_idx].num_poss;
      int removed_el = 0;
      for (int j = 0; j < mappings[letter - 97].num_poss; j++) {
        int is_present = 0;
        for (int jj = 0; jj < num_values; jj++) {
          if (values[jj] == mappings[letter - 97].possibilities[j]) {
            is_present = 1;
            break;
          }
        }
        if (!is_present) {
          mappings[letter - 97].possibilities[j] = -1;
          removed_el++;
        }
      }
      shiftValidValues(mappings + (letter - 97), removed_el);
    }
  }
}

int decodeToken(char *token, symbol *mappings, char *pair_double) {
  int tok_len = 0;
  while (*(token + ++tok_len) && *(token + tok_len) != '\n')
    ;

  // If length is unique then return number
  int value = -1;
  if (tok_len == 2)
    return 1;
  if (tok_len == 4)
    return 4;
  if (tok_len == 3)
    return 7;
  if (tok_len == 7)
    return 8;

  int both_present = findLetter(token, pair_double[0], tok_len) &&
                     findLetter(token, pair_double[1], tok_len);
  // If both letter where we are unsure are present than it doesn't matter
  // since we will fill both segments anyway
  if (both_present) {
    int arr[7] = {0};
    for (int i = 0; i < tok_len; i++)
      arr[mappings[token[i] - 97].possibilities[0]] = 1;

    arr[mappings[pair_double[0] - 97].possibilities[1]] = 1;
    return parseSevenSegment(arr);
  }
  // Else try to find which of the two possible representation is the correct
  // one
  int first_try = -1;
  int second_try = -1;
  int arr[7] = {0};
  for (int i = 0; i < tok_len; i++) {
    if (token[i] == pair_double[0] || token[i] == pair_double[1])
      continue;
    arr[mappings[token[i] - 97].possibilities[0]] = 1;
  }
  arr[mappings[pair_double[0] - 97].possibilities[0]] = 1;
  first_try = parseSevenSegment(arr);
  arr[mappings[pair_double[0] - 97].possibilities[0]] = 0;
  arr[mappings[pair_double[0] - 97].possibilities[1]] = 1;
  second_try = parseSevenSegment(arr);
  return first_try > second_try ? first_try : second_try;
}
