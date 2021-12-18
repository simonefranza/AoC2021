//
// Advent of Code 2021
//
// Day 4
//
// Author: Simone Franza
//
// Thought process:
// The values of the board go from 0 to 99 so 7 bits are enough to
// store the value of an element of the board and the MSB can be used to mark
// them
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "input"
#define ARR_SIZE 100

typedef struct board_ {
  uint8_t values[25];
} board;

int markValue(uint8_t extraction, board *boards, int num_boards);
int markValue2(uint8_t extraction, board *boards, int num_boards, int* finished_board);
int checkRowsNCols(board *curr_board);
int computeScore(board* curr_board, int value);

int main() {
  // Part 1
  uint8_t *extraction = (uint8_t *)calloc(sizeof(uint8_t), ARR_SIZE);
  int extraction_len = 0;
  int extraction_max_len = ARR_SIZE;
  FILE *fp = fopen(FILENAME, "r");
  size_t num_read = 0;
  char *str = fgetln(fp, &num_read);
  char *copied_str = (char*)calloc(sizeof(char), num_read + 1);
  memcpy(copied_str, str, num_read);
  const char s[2] = ",";
  char *token = strtok(copied_str, s);
  while (token != NULL) {
    extraction[extraction_len++] = atoi(token);
    token = strtok(NULL, s);
    if (extraction_len == extraction_max_len) {
      uint8_t *temp = realloc(extraction, extraction_len + ARR_SIZE);
      if (!temp) {
        free(extraction);
        return -1;
      }
      extraction = temp;
      extraction_max_len += ARR_SIZE;
    }
  }
  free(copied_str);

  board *boards = (board *)calloc(sizeof(board), ARR_SIZE);
  int num_boards = 0;
  int max_num_boards = 0;
  fgetln(fp, &num_read);
  while ((str = fgetln(fp, &num_read))) {
    copied_str = (char*)calloc(sizeof(char), num_read + 1);
    memcpy(copied_str, str, num_read);
    char sep[2] = " ";
    for (int i = 0; i < 5; i++) {
      token = strtok(copied_str, sep);
      int token_n = 0;
      for(int j = 0; j < 5; j++) {
        boards[num_boards].values[i*5 + token_n++] = atoi(token);
        token = strtok(NULL, sep);
      }
      free(copied_str);
      str = fgetln(fp, &num_read);
      copied_str = (char*)calloc(sizeof(char), num_read + 1);
      memcpy(copied_str, str, num_read);
    }
    num_boards++;
    if (num_boards == max_num_boards) {
      board *temp = realloc(boards, num_boards + ARR_SIZE);
      if (!temp) {
        free(extraction);
        free(boards);
        free(copied_str);
        return -1;
      }
      boards = temp;
      max_num_boards += ARR_SIZE;
    }
  }
  int res = 0;
  for(int ext = 0; ext < extraction_len; ext++)
  {
    if((res = markValue(extraction[ext], boards, num_boards)))
      break;
  }
  printf("The final score is %d\n", res);
  //Part 2
  // Clean boards
  for(int i = 0; i < num_boards; i++)
    for(int j = 0; j < 25; j++)
      boards[i].values[j] &= 0x7f;
  int *finished_board = (int*)calloc(sizeof(int), num_boards);
  int score_last_board = 0;
  for(int ext = 0; ext < extraction_len; ext++)
  {
    if((res = markValue2(extraction[ext], boards, num_boards, finished_board)) != -1) {
      score_last_board = computeScore(boards + res, extraction[ext]);
      break;
    }
  }
  printf("The score of the last board to finish is %d\n", score_last_board);

  // Cleanup
  free(extraction);
  free(boards);
  free(copied_str);
  free(finished_board);

  fclose(fp);
  return 0;
}

int markValue(uint8_t extraction, board *boards, int num_boards) {
  for(int board_idx = 0; board_idx < num_boards; board_idx++) {
    for(int el = 0; el < 25; el++) {
      if((boards[board_idx].values[el] & 0x7f) != extraction)
        continue;
      boards[board_idx].values[el] |= 0x80;
      if(checkRowsNCols(boards + board_idx)) {
        return computeScore(boards + board_idx, extraction);
      }
    }
  }
  return 0;
}

int markValue2(uint8_t extraction, board *boards, int num_boards, int* finished_board) {
  for(int board_idx = 0; board_idx < num_boards; board_idx++) {
    if(finished_board[board_idx])
      continue;
    for(int el = 0; el < 25; el++) {
      if((boards[board_idx].values[el] & 0x7f) != extraction)
        continue;
      boards[board_idx].values[el] |= 0x80;
      if(checkRowsNCols(boards + board_idx)) {
        finished_board[board_idx] = computeScore(boards + board_idx, extraction);
        int all_finished = 1;
        for(int idx = 0; idx < num_boards; idx++) {
          if(!finished_board[idx]) {
            all_finished = 0;
            break;
          }
        }
        if(all_finished)
          return board_idx;
      }
    }
  }
  return -1;
}

int checkRowsNCols(board *curr_board) {
  for(int outer = 0; outer < 5; outer++) {
    int whole_row_marked = 1;
    int whole_col_marked = 1;
    for(int inner = 0; inner < 5; inner++) {
      if(!(curr_board->values[outer*5 + inner] & 0x80)) {
        whole_row_marked = 0;
      }
      if(!(curr_board->values[outer + inner*5] & 0x80)) {
        whole_col_marked = 0;
      }
    }
    if(whole_row_marked || whole_col_marked)
      return 1;
  }
  return 0;
}

int computeScore(board* curr_board, int value) {
  int res = 0;
  for(int outer = 0; outer < 5; outer++) {
    for(int inner = 0; inner < 5; inner++) {
      if(!(curr_board->values[outer*5 + inner] & 0x80)) {
        res += (curr_board->values[outer*5 + inner] & 0x7f);
      }
    }
  }
  return res*value;
}
