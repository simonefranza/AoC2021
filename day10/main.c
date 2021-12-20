//
// Advent of Code 2021
//
// Day 10
//
// Author: Simone Franza
//
// Thought process:
// Use stack, push opening parentheses, pop when closing
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "input"
#define ARR_SIZE 50

int findValuePart1(char el);
int findValuePart2(char el);

int main() {
  // Part 1
  FILE *fp = fopen(FILENAME, "r");
  size_t read_len;
  char *str;
  int score = 0;
  char **incomplete_lines = calloc(sizeof(char *), ARR_SIZE);
  int num_lines = 0;
  int max_lines = ARR_SIZE;
  while ((str = fgetln(fp, &read_len))) {
    int stack[80] = {0};
    int stack_pos = 0;
    for (int i = 0; i < read_len; i++) {
      if (str[i] == '(' || str[i] == '[' || str[i] == '{' || str[i] == '<')
        stack[stack_pos++] = str[i];
      else if ((str[i] == ')' && stack[stack_pos - 1] == '(') ||
               (str[i] == ']' && stack[stack_pos - 1] == '[') ||
               (str[i] == '}' && stack[stack_pos - 1] == '{') ||
               (str[i] == '>' && stack[stack_pos - 1] == '<'))
        stack_pos--;
      else if (str[i] == '\n') {
        char *copy = calloc(sizeof(char), read_len + 1);
        memcpy(copy, str, read_len);
        incomplete_lines[num_lines++] = copy;
        if (num_lines == max_lines) {
          // I know this isn't the proper way to realloc
          incomplete_lines = realloc(incomplete_lines, num_lines + ARR_SIZE);
          max_lines += ARR_SIZE;
        }
        break;
      } else {
        printf("Expected %c, got %c\n", stack[stack_pos - 1], str[i]);
        score += findValuePart1(str[i]);
        break;
      }
    }
  }
  printf("The total syntax error score is %d\n", score);

  // Part 2
  printf("There are a total of %d incomplete lines\n", num_lines);
  ssize_t *scores = calloc(sizeof(size_t), num_lines);
  for(int i = 0; i < num_lines; i++) {
    scores[i] = -1;
  }
  for (int line_idx = 0; line_idx < num_lines; line_idx++) {
    int stack[80] = {0};
    int stack_pos = 0;
    ssize_t internal_score = 0;
    int i = 0;
    char *str = incomplete_lines[line_idx];
    while(*(str + i)) {
      if (str[i] == '(' || str[i] == '[' || str[i] == '{' || str[i] == '<')
        stack[stack_pos++] = str[i];
      else if ((str[i] == ')' && stack[stack_pos - 1] == '(') ||
               (str[i] == ']' && stack[stack_pos - 1] == '[') ||
               (str[i] == '}' && stack[stack_pos - 1] == '{') ||
               (str[i] == '>' && stack[stack_pos - 1] == '<'))
        stack_pos--;
      else if (str[i] == '\n') {
        for(int j = stack_pos - 1; j >= 0; j--) {
          internal_score *= 5;
          internal_score += findValuePart2(stack[j]);
        }
        break;
      } else {
        printf("This shouldn't happen\n");
        score += findValuePart1(str[i]);
        break;
      }
      i++;
    }
    int in_idx = -1;
    // Insert sort
    for(int i = 0; i < line_idx; i++) {
      if(i == 0 && internal_score < scores[i]) {
        in_idx = 0;
        break;
      }
      if((internal_score >= scores[i] && scores[i+1] == -1) || 
          (internal_score >= scores[i] && internal_score <= scores[i+1])) {
        in_idx = i+1;
        break;
      }
    }
    if(!line_idx) {
      scores[0] = internal_score;
      continue;
    }
    for(int i = line_idx - 1; i >= in_idx; i--) {
      scores[i + 1] = scores[i];
    }
    scores[in_idx] = internal_score;
  }
  printf("Auto-completion middle score %zd\n", scores[(num_lines+1)/2-1]);

  // Cleanup
  fclose(fp);

  return 0;
}

int findValuePart1(char el) {
  if (el == ')')
    return 3;
  if (el == ']')
    return 57;
  if (el == '}')
    return 1197;
  return 25137;
}

int findValuePart2(char el) {
  if (el == '(')
    return 1;
  if (el == '[')
    return 2;
  if (el == '{')
    return 3;
  return 4;
}
