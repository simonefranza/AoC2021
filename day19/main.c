//
// Advent of Code 2021
//
// Day 19
//
// Author: Simone Franza
//
// Thought process:
// This one is though, I'm thinking about using a matrix to first rotate the points
// of one scanner in one of the 24 directions, then find the translation (aka distance)
// between one point and one of another scanner, however we don't know which 
// one (if any at all) so it's gonna be brute force.
// The idea is that the difference between a scanner and another one is a rotation +
// translation of the points. If we find the matrix we can transform the coordinates
// and see if the scanners have at least 12 points that are equal after the rotation.
// I'm gonna assume that if 12 points match, then they are really the same scanners
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define FILENAME "input"
#define NUM_SCANNERS 26
#define DATA_PER_SCANNER 30
#define PAIRS (NUM_SCANNERS*NUM_SCANNERS)
#define MIN_SIM 12

typedef struct Node_{
  struct Node_ *prev;
  struct Node_ **children;
  int num_children;
  int value;
} Node;

typedef struct Vec3_ {
  int x;
  int y;
  int z;
} Vec3;

typedef struct Scanner_ {
  Vec3 *data;
  int num_data;
  int capacity;
} Scanner;

typedef struct ScannerTransform_{
  Scanner *from;
  Scanner *to;
  int from_idx;
  int to_idx;
  int matrix[3*4];
} ScannerTransform;

Scanner **readData();
Scanner *readScannerData(FILE *fp);
int generateRotationsMatrix(int *mat);
ScannerTransform *findPair(Scanner *from, Scanner *to, int *matrices);
void matrixVecMult(int mat[9], Vec3 vec, Vec3 *res);
void matrixMatrixMult(int a[9], int b[9], int *res);
int numberSamePoints(Vec3 *a, int a_len, Vec3 *b, int b_len);
ScannerTransform *genTransform(Scanner *from, Scanner *to, int *mat, Vec3 translate);
void translateData(Vec3 *rotated, int len, Vec3 translation, Vec3 *translated);
ScannerTransform *transform(Scanner *from, Scanner *to, int *mat);
Vec3 *rotateTranslate(int *mat, Vec3 *data, int len);
int isMatrixPresent(int *matrices, int num_mat, int mat[9]);
int findPath(Node* root, int *path, int target);
int getPairIdx(ScannerTransform **pairs, int num_pairs, int scanners[2]);
void addDataToPoints(Vec3 *uniquePoints, int *num_points, Vec3 *curr_data, int num_data);
Node *createPathTree(ScannerTransform **pairs, int num_pairs);

int main() {
  // Part 1
  Scanner **scanners = readData();
  ScannerTransform **pairs = calloc(PAIRS, sizeof(ScannerTransform*));
  int *matrices = calloc(9*50, sizeof(int));
  generateRotationsMatrix(matrices);
  int num_pairs = 0;
  for(int i = 0; i < NUM_SCANNERS - 1; i++) {
    for(int j = i + 1; j < NUM_SCANNERS; j++) {
      printf("\rMap from %2d to %2d", i, j);
      fflush(stdout);
      ScannerTransform *transform = findPair(scanners[i], scanners[j], matrices);
      if(!transform)
        continue;
      transform->from_idx = i;
      transform->to_idx = j;
      pairs[num_pairs++] = transform;
    }
  }
  putchar('\n');
  if(!num_pairs) 
    return 0;
  Node *root = createPathTree(pairs, num_pairs);
  int *path = calloc(NUM_SCANNERS, sizeof(int));
  int path_len = 0;
  Vec3 *uniquePoints = calloc(NUM_SCANNERS*25, sizeof(Vec3));
  int num_points = 0;
  for(int i = 0; i< scanners[0]->num_data; i++) {
    uniquePoints[num_points].x =   scanners[0]->data[i].x;
    uniquePoints[num_points].y =   scanners[0]->data[i].y;
    uniquePoints[num_points++].z = scanners[0]->data[i].z;
  }
  for(int i = 1; i < NUM_SCANNERS; i++) {
    path_len = findPath(root, path, i);
    Vec3 *curr_data = calloc(path_len * 25, sizeof(Vec3));
    int num_data = 0;
    for(int j = 0; j < scanners[path[0]]->num_data; j++) {
      curr_data[num_data++] = scanners[path[0]]->data[j];
    }
    for(int j = 0; j < path_len-1;j++) {
      int scans[] ={path[j], path[j+1]};
      int pairIdx = getPairIdx(pairs, num_pairs, scans);
      ScannerTransform *transform = pairs[pairIdx];
      int *matrix = transform->matrix;
      if(pairs[pairIdx]->from_idx != path[j]) {
        transform = findPair(scanners[scans[0]], scanners[scans[1]], matrices);
        matrix = transform->matrix;
      }
      Vec3* temp = rotateTranslate(matrix, curr_data, num_data);
      free(curr_data);
      curr_data = temp;
    }
    addDataToPoints(uniquePoints, &num_points, curr_data, num_data);
    memset(path, 0, sizeof(int)*NUM_SCANNERS);
  }
  printf("Number of unique beacons %d\n", num_points);

  // Part 2
  Vec3 *scannerPos = calloc(NUM_SCANNERS, sizeof(Vec3));
  for(int i = 0; i < NUM_SCANNERS; i++) {
    path_len = findPath(root, path, i);
    Vec3 *curr_data = calloc(1, sizeof(Vec3));
    curr_data[0].x = 0;
    curr_data[0].y = 0;
    curr_data[0].z = 0;
    int num_data = 1;
    for(int j = 0; j < path_len-1;j++) {
      int scans[] ={path[j], path[j+1]};
      int pairIdx = getPairIdx(pairs, num_pairs, scans);
      ScannerTransform *transform = pairs[pairIdx];
      int *matrix = transform->matrix;
      if(pairs[pairIdx]->from_idx != path[j]) {
        transform = findPair(scanners[scans[0]], scanners[scans[1]], matrices);
        matrix = transform->matrix;
      }
      Vec3* temp = rotateTranslate(matrix, curr_data, num_data);
      curr_data = temp;
    }
    scannerPos[i].x = curr_data[0].x;
    scannerPos[i].y = curr_data[0].y;
    scannerPos[i].z = curr_data[0].z;
    memset(path, 0, sizeof(int)*NUM_SCANNERS);
  }
  int max_dist = 0;
  int max_i, max_j;
  for(int i = 0; i < NUM_SCANNERS; i++) {
    for(int j = 0; j < NUM_SCANNERS; j++) {
      if(i == j)
        continue;
      int dist = abs(scannerPos[i].x - scannerPos[j].x) + 
                 abs(scannerPos[i].y - scannerPos[j].y) + 
                 abs(scannerPos[i].z - scannerPos[j].z);
      if(dist > max_dist) {
        max_dist = dist;
        max_i = i;
        max_j = j;
      }
    }
  }
  printf("Maximum distance between scanners %d and %d is: %d\n", max_i, max_j,max_dist);


  return 0;
}

Scanner **readData() {
  Scanner **scanners = calloc(NUM_SCANNERS, sizeof(Scanner*));
  FILE *fp = fopen(FILENAME, "r");
  for(int i = 0; i < NUM_SCANNERS; i++)
    scanners[i] = readScannerData(fp);

  fclose(fp);
  return scanners;
}

Scanner *readScannerData(FILE *fp) {
  size_t len = 0;
  char *str;
  Scanner *scanner = calloc(1, sizeof(Scanner));
  scanner->data = calloc(DATA_PER_SCANNER, sizeof(Vec3));
  scanner->capacity = DATA_PER_SCANNER;
  //Scanner line
  fgetln(fp, &len);
  str = fgetln(fp, &len);
  char *copy = calloc(len, sizeof(char));
  memcpy(copy, str, len);
  while(strcmp(copy, "\n")) {
    scanner->data[scanner->num_data].x = atoi(strtok(copy, ","));
    scanner->data[scanner->num_data].y = atoi(strtok(NULL, ","));
    scanner->data[scanner->num_data++].z = atoi(strtok(NULL, ","));
    memset(copy, 0, len);
    str = fgetln(fp, &len);
    if(!str)
      break;
    memcpy(copy, str, len);
  }
  free(copy);
  return scanner;
}

ScannerTransform *findPair(Scanner *from, Scanner *to, int *matrices) {
  Vec3 *rotated = calloc(from->num_data, sizeof(Vec3));
  for(int i = 0; i < 24; i++) {
    ScannerTransform *res = transform(from, to, matrices + i*9);
    if(res)
      return res;
    memset(rotated, 0, from->num_data);
  }
  return NULL;
}

void matrixVecMult(int mat[9], Vec3 vec, Vec3 *res) {
  res->x = mat[0]*vec.x + mat[1]*vec.y + mat[2]*vec.z;
  res->y = mat[3]*vec.x + mat[4]*vec.y + mat[5]*vec.z;
  res->z = mat[6]*vec.x + mat[7]*vec.y + mat[8]*vec.z;
}

int numberSamePoints(Vec3 *a, int a_len, Vec3 *b, int b_len) {
  int num = 0;
  for(int i_a = 0; i_a < a_len; i_a++) {
    for(int i_b = 0; i_b < b_len; i_b++) {
      if(a[i_a].x == b[i_b].x && a[i_a].y == b[i_b].y && a[i_a].z == b[i_b].z) {
        num++;
      }
    }
  }
  return num;
}

ScannerTransform *genTransform(Scanner *from, Scanner *to, int mat[9], Vec3 translate) {
  ScannerTransform *trans = calloc(1, sizeof(ScannerTransform));
  trans->from = from;
  trans->to = to;
  for(int i = 0; i < 9; i++)
    trans->matrix[i % 3 + i/3*4] = mat[i];
  trans->matrix[3] = translate.x;
  trans->matrix[7] = translate.y;
  trans->matrix[11] = translate.z;
  return trans;
}

void translateData(Vec3 *rotated, int len, Vec3 translation, Vec3 *translated) {
  for(int i = 0; i < len; i++) {
    translated[i].x = rotated[i].x + translation.x;
    translated[i].y = rotated[i].y + translation.y;
    translated[i].z = rotated[i].z + translation.z;
  }
}

ScannerTransform *transform(Scanner *from, Scanner *to, int *mat) {
  Vec3 *rotated = calloc(from->num_data, sizeof(Vec3));

  for(int i = 0; i < from->num_data; i++)
    matrixVecMult(mat, from->data[i], &rotated[i]);

  if(numberSamePoints(rotated, from->num_data, to->data, to->num_data) >= MIN_SIM) {
    Vec3 zero;
    zero.x = 0;
    zero.y = 0;
    zero.z = 0;
    return genTransform(from, to, mat, zero);
  }

  Vec3 translation = {0};
  Vec3 *translated = calloc(from->num_data, sizeof(Vec3));
  for(int i = 0; i < from->num_data; i++) {
    for(int j = 0; j < to->num_data; j++) {
      translation.x = to->data[j].x - rotated[i].x;
      translation.y = to->data[j].y - rotated[i].y;
      translation.z = to->data[j].z - rotated[i].z;
      translateData(rotated, from->num_data, translation, translated);
      if(numberSamePoints(translated, from->num_data, to->data, to->num_data) >= MIN_SIM) {
        free(translated);
        return genTransform(from, to, mat, translation);
      }
    }
  }
  free(translated);
  return NULL;
}

int generateRotationsMatrix(int *mat) {
  int num_mat = 0;
  int xRot[4][9] = {
                    {
                      1, 0, 0,
                      0, 1, 0,
                      0, 0, 1
                    },{
                      1, 0, 0,
                      0, 0, 1,
                      0,-1, 0
                    },{
                      1, 0, 0,
                      0,-1, 0,
                      0, 0,-1
                    },{
                      1, 0, 0,
                      0, 0,-1,
                      0, 1, 0
                    }};
  int yRot[4][9] = {
                    {
                      1, 0, 0,
                      0, 1, 0,
                      0, 0, 1
                    },{
                      0, 0, 1,
                      0, 1, 0,
                     -1, 0, 0
                    },{
                     -1, 0, 0,
                      0, 1, 0,
                      0, 0,-1
                    },{
                      0, 0,-1,
                      0, 1, 0,
                      1, 0, 0
                    }};
  int zRot[4][9] = {
                    {
                      1, 0, 0,
                      0, 1, 0,
                      0, 0, 1
                    },{
                      0, 1, 0,
                     -1, 0, 0,
                      0, 0, 1
                    },{
                     -1, 0, 0,
                      0,-1, 0,
                      0, 0, 1
                    },{
                      0,-1, 0,
                      1, 0, 0,
                      0, 0, 1
                    }};
  for(int x = 0; x < 4; x++) {
    for(int y = 0; y < 4; y++) {
      int new_mat[9] = {0};
      matrixMatrixMult(xRot[x], yRot[y], new_mat);
      if(isMatrixPresent(mat, num_mat, new_mat))
        continue;
      for(int i = 0; i < 9; i++)
        mat[num_mat*9 + i] = new_mat[i];
      num_mat++;
    }
  }
  for(int x = 0; x < 4; x++) {
    for(int z = 0; z < 4; z++) {
      int new_mat[9] = {0};
      matrixMatrixMult(xRot[x], zRot[z], new_mat);
      if(isMatrixPresent(mat, num_mat, new_mat))
        continue;
      for(int i = 0; i < 9; i++)
        mat[num_mat*9 + i] = new_mat[i];
      num_mat++;
    }
  }
  return num_mat;
}

Vec3 *rotateTranslate(int *mat, Vec3 *data, int len) {
  Vec3 *res = calloc(len, sizeof(Vec3));
  for(int i = 0; i < len; i++) {
    res[i].x = mat[0] * data[i].x + mat[1] * data[i].y + mat[2] * data[i].z + mat[3];
    res[i].y = mat[4] * data[i].x + mat[5] * data[i].y + mat[6] * data[i].z + mat[7];
    res[i].z = mat[8] * data[i].x + mat[9] * data[i].y + mat[10] * data[i].z + mat[11];
  }
  return res;
}

void matrixMatrixMult(int a[9], int b[9], int *res) {
  for(int i = 0; i < 3; i++)
    for(int j = 0; j < 3; j++)
      res[i * 3 + j] = a[i*3]*b[j] + a[i*3 + 1]*b[j + 3] + a[i*3 + 2]*b[j + 6];
}

int isMatrixPresent(int *matrices, int num_mat, int mat[9]) {
  int isSame= 1;
  for(int i = 0; i < num_mat; i++) {
    isSame = 1;
    for(int j = 0; j < 9; j++) {
      if(matrices[i*9 + j] != mat[j]) {
        isSame = 0;
        break;
      }
    }
    if(isSame) return 1;
  }
  return 0;
}

int findPath(Node *root, int *path, int target) {
  int path_len = 0;
  Node **queue = calloc(NUM_SCANNERS, sizeof(Node));
  int queue_pos = 0;
  int queue_len = 0;
  queue[queue_len++] = root;
  Node *curr = queue[queue_pos++];
  while(curr) {
    if(curr->value == target)
      break;
    for(int i = 0; i < curr->num_children; i++) {
      queue[queue_len++] = curr->children[i];
    }
    curr = queue[queue_pos++];
  }
  while(curr) {
    path[path_len++] = curr->value;
    curr = curr->prev;
  }
  return path_len;
}

int getPairIdx(ScannerTransform **pairs, int num_pairs, int scanners[2]) {
  for(int i = 0; i < num_pairs; i++) {
    if((pairs[i]->from_idx == scanners[0] && pairs[i]->to_idx == scanners[1]) ||
      (pairs[i]->from_idx == scanners[1] && pairs[i]->to_idx == scanners[0]))
      return i;
  }
  return -1;
}

void addDataToPoints(Vec3 *uniquePoints, int *num_points, Vec3 *curr_data, int num_data) {
  for(int i = 0; i < num_data; i++) {
    int found = 0;
    for(int j = 0; j < *num_points; j++) {
      if(curr_data[i].x == uniquePoints[j].x && curr_data[i].y == uniquePoints[j].y && 
          curr_data[i].z == uniquePoints[j].z) {
        found = 1;
        break;
      }
    }
    if(found) 
      continue;
    uniquePoints[*num_points].x = curr_data[i].x;
    uniquePoints[*num_points].y = curr_data[i].y;
    uniquePoints[(*num_points)++].z = curr_data[i].z;
  }
}

Node *createPathTree(ScannerTransform **pairs, int num_pairs) {
  Node *root = calloc(1, sizeof(Node));
  root->value = 0;
  root->children = calloc(NUM_SCANNERS, sizeof(Node));
  Node **queue = calloc(NUM_SCANNERS, sizeof(Node));
  int queue_pos = 0;
  int queue_len = 0;
  queue[queue_len++] = root;
  while(queue_pos < queue_len) {
    Node *curr = queue[queue_pos++];
    for(int i = 0; i < num_pairs; i++) {
      if(pairs[i]->from_idx != curr->value && pairs[i]->to_idx != curr->value)
        continue;
      int value_to_add;
      if(pairs[i]->from_idx == curr->value)
        value_to_add = pairs[i]->to_idx;
      else
        value_to_add = pairs[i]->from_idx;
      int isPresent = 0;
      for(int j = 0; j < queue_len; j++) {
        if(queue[j]->value == value_to_add) {
          isPresent = 1;
          break;
        }
      }
      if(isPresent)
        continue;
      Node *new_node = calloc(1, sizeof(Node));
      new_node->children = calloc(NUM_SCANNERS, sizeof(Node));
      new_node->value = value_to_add;
      new_node->prev = curr;
      curr->children[curr->num_children++] = new_node;
      queue[queue_len++] = new_node;
    }
  }
  return root;
}
