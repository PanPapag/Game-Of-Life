#include <stdio.h>
#include <stdlib.h>

#include <mpi.h>
#include <omp.h>

#include "../headers/game_utils.h"
#include "../../common/headers/io_utils.h"

int exec_game_rules(char** prev_gen, char** next_gen, int i, int j, int alive_neighbours) {
  int changed = 0;

  // rules regarding an alive cell
  if (prev_gen[i][j] == '1') {
    if (alive_neighbours != 2 && alive_neighbours != 3) {
      next_gen[i][j] = '0';
      changed = 1;
    } else {
      next_gen[i][j] = '1';
    }
  }
  // rules regarding dead cells
  else {
    // exactly 3 neighbours -> a new cell is born
    if (alive_neighbours == 3) {
      next_gen[i][j] = '1';
      changed = 1;
    } else {
      next_gen[i][j] = '0';
    }
  }
  return changed;
}

int calculate_inner_gen(char** prev_gen, char** next_gen, subgrid_info* subgrid) {
  int changed = 0;
  #pragma omp parallel for collapse(2) schedule(static)
  for (int i = 2; i <= subgrid->rows-1; i++) {
    for (int j = 2; j <= subgrid->cols-1; j++) {
      // compute how many organizations exist near the cell we are about to evolve
      int alive_neighbours = (prev_gen[i-1][j-1] == '1') + (prev_gen[i-1][j] == '1') +
                             (prev_gen[i-1][j+1] == '1') + (prev_gen[i][j-1] == '1') +
                             (prev_gen[i][j+1] == '1') + (prev_gen[i+1][j-1] == '1') +
                             (prev_gen[i+1][j] == '1') + (prev_gen[i+1][j+1] == '1');
      // printf("(%d, %d) - N: %d - RANK: %d\n",i,j,alive_neighbours,rank);
      if (exec_game_rules(prev_gen, next_gen, i, j, alive_neighbours) == 1) {
        changed = 1;
      }
    }
  }
  return changed;
}

int calculate_outter_gen(char** prev_gen, char** next_gen, subgrid_info* subgrid) {
  int alive_neighbours, changed = 0;
  // First row
  #pragma omp parallel for schedule(static)
  for (int j = 1; j <= subgrid->cols; ++j) {
    int i = 1;
    int alive_neighbours = (prev_gen[i-1][j-1] == '1') + (prev_gen[i-1][j] == '1') +
                           (prev_gen[i-1][j+1] == '1') + (prev_gen[i][j-1] == '1') +
                           (prev_gen[i][j+1] == '1') + (prev_gen[i+1][j-1] == '1') +
                           (prev_gen[i+1][j] == '1') + (prev_gen[i+1][j+1] == '1');
    if (exec_game_rules(prev_gen, next_gen, i, j, alive_neighbours) == 1) {
      changed = 1;
    }
  }
  // Last row
  #pragma omp parallel for schedule(static)
  for (int j = 1; j <= subgrid->cols; ++j) {
    int i = subgrid->rows;
    int alive_neighbours = (prev_gen[i-1][j-1] == '1') + (prev_gen[i-1][j] == '1') +
                           (prev_gen[i-1][j+1] == '1') + (prev_gen[i][j-1] == '1') +
                           (prev_gen[i][j+1] == '1') + (prev_gen[i+1][j-1] == '1') +
                           (prev_gen[i+1][j] == '1') + (prev_gen[i+1][j+1] == '1');
    if (exec_game_rules(prev_gen, next_gen, i, j, alive_neighbours) == 1) {
      changed = 1;
    }
  }
  // First column
  #pragma omp parallel for schedule(static)
  for (int i = 1; i <= subgrid->rows; ++i) {
    int j = 1;
    int alive_neighbours = (prev_gen[i-1][j-1] == '1') + (prev_gen[i-1][j] == '1') +
                           (prev_gen[i-1][j+1] == '1') + (prev_gen[i][j-1] == '1') +
                           (prev_gen[i][j+1] == '1') + (prev_gen[i+1][j-1] == '1') +
                           (prev_gen[i+1][j] == '1') + (prev_gen[i+1][j+1] == '1');
    if (exec_game_rules(prev_gen, next_gen, i, j, alive_neighbours) == 1) {
      changed = 1;
    }
  }
  // Last column
  #pragma omp parallel for schedule(static)
  for (int i = 1; i <= subgrid->rows; ++i) {
    int j = subgrid->cols;
    int alive_neighbours = (prev_gen[i-1][j-1] == '1') + (prev_gen[i-1][j] == '1') +
                           (prev_gen[i-1][j+1] == '1') + (prev_gen[i][j-1] == '1') +
                           (prev_gen[i][j+1] == '1') + (prev_gen[i+1][j-1] == '1') +
                           (prev_gen[i+1][j] == '1') + (prev_gen[i+1][j+1] == '1');
    if (exec_game_rules(prev_gen, next_gen, i, j, alive_neighbours) == 1) {
      changed = 1;
    }
  }
  return changed;
}
