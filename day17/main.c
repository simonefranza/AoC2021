//
// Advent of Code 2021
//
// Day 17
//
// Author: Simone Franza
//
// Thought process:
// The min x velocity is the solution to x*(x+1)/2 >= x_min since the final x 
// position can be modeled as x_pos + (x_pos - 1) + .. + 1. The max x velocity
// is x_max since otherwise we will overshoot the target
// The min y velocity is y_min since otherwise we will go too low.
// To compute the max y velocity I noticed that if the starting y velocity is 
// positive, then at some point the object returns exactly y = 0 and then
// moves down by (starting_y_velocity + 1). For example if the starting y velocity
// is 3 we will have the following y positions: 0, 3, 5, 6, 6, 5, 3, 0, -4
// so the max y velocity has to be smaller than -y_min + 1, so that on the way
// down we won't overshoot
//
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define FILENAME "input"

typedef struct Window_ {
  int x_min;
  int x_max;
  int y_min;
  int y_max;
} Window;

typedef struct Traj_ {
  int x_vel;
  int y_vel;
  int y_top;
} Traj;

int computeTrajectory(int x_vel, int y_vel, Window *wd);

int main() {
  // Part 1
  FILE *fp = fopen(FILENAME, "r");
  size_t len_read = 0;
  char *str = fgetln(fp, &len_read);
  char *copy = calloc(sizeof(char), len_read);
  memcpy(copy, str, len_read - 1);
  strtok(copy, "=..");
  Window wd;
  wd.x_min = atoi(strtok(NULL, "=.."));
  wd.x_max = atoi(strtok(NULL, "..,"));
  wd.y_min = atoi(strtok(NULL, " y=.."));
  wd.y_max = atoi(strtok(NULL, "."));
  int x_min_vel = ceil((-1 + sqrt(1 + 8 * wd.x_min))/2.0);
  int y_max_vel = -wd.y_min + 1;
  Traj *traj = calloc(sizeof(Traj), (wd.x_max - x_min_vel + 1) * (y_max_vel - wd.y_min + 1));
  int num_traj = 0;
  for(int x_vel = x_min_vel; x_vel <= wd.x_max; x_vel++) {
    for(int y_vel = wd.y_min; y_vel <= y_max_vel; y_vel++) {
      int y_top = computeTrajectory(x_vel, y_vel, &wd);
      if(y_top != -1) {
        traj[num_traj].x_vel = x_vel;
        traj[num_traj].y_vel = y_vel;
        traj[num_traj++].y_top = y_top;
      }
    }
  }
  int y_top = -1;
  int idx = -1;
  for(int i = 0; i < num_traj; i++) {
    if(traj[i].y_top > y_top) {
      y_top = traj[i].y_top;
      idx = i;
    }
  }
  printf("Highest point reached with x vel %d y vel %d. Highest y %d\n", traj[idx].x_vel, 
      traj[idx].y_vel, traj[idx].y_top);
  printf("Number of trajectories that hit %d\n", num_traj);

  return 0;
}

int computeTrajectory(int x_vel, int y_vel, Window *wd) {
  int x_pos = 0;
  int y_pos = 0;
  int y_max = 0;
  while(x_pos <= wd->x_max && y_pos >= wd->y_min) {
    x_pos += x_vel;
    y_pos += y_vel;
    if(y_pos > y_max)
      y_max = y_pos;
    if(x_vel > 0)
      x_vel--;
    else if(x_vel < 0)
      x_vel++;
    y_vel--;
    if(x_pos >= wd->x_min && x_pos <= wd->x_max && y_pos >= wd->y_min && y_pos <= wd->y_max) {
      return y_max;
    }
  }
  return -1;
}
