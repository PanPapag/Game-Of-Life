#include <stdio.h>
#include <stdlib.h>

#include <mpi.h>

#include "../headers/game_utils.h"
#include "../../common/headers/io_utils.h"

int exec_game_rules(char** prev_gen, char** next_gen, int i, int j, int alive_neighbours) {
  int changed = 0;
  // rules regarding an alive cell
  if (prev_gen[i][j] == '1') {
    // 0 or 1 neighbours -> the cell dies
    if (alive_neighbours < 2) {
      next_gen[i][j] = '0';
      changed = 1;
    }
    // 2 or 3 neighbours -> the cell survives
    else if (alive_neighbours < 4) {
      next_gen[i][j] = '1';
    }
    // more than 4 neighbours -> the cell dies due to overpopulation
    else {
      next_gen[i][j] = '0';
      changed = 1;
    }
  }
  // rules regarding dead cells
  else {
    // exactly 3 neighbours -> a new cell is born
    if (alive_neighbours == 3) {
      next_gen[i][j] = '1';
      changed = 1;
    }
  }
  return changed;
}

int calculate_inner_gen(char** prev_gen, char** next_gen, subgrid_info* subgrid) {
  int changed;
  for (int i = 1; i <= subgrid->rows; i++) {
    for (int j = 1; j <= subgrid->cols; j++) {
      // compute how many organizations exist near the cell we are about to evolve
      int alive_neighbours = (prev_gen[i-1][j-1] == '1') + (prev_gen[i-1][j] == '1') +
                             (prev_gen[i-1][j+1] == '1') + (prev_gen[i][j-1] == '1') +
                             (prev_gen[i][j+1] == '1') + (prev_gen[i+1][j-1] == '1') +
                             (prev_gen[i+1][j] == '1') + (prev_gen[i+1][j+1] == '1');
      if (exec_game_rules(prev_gen, next_gen, i, j, alive_neighbours) == 1) {
        changed = 1;
      }
    }
  }
  return changed;
}

int calculate_outter_gen(char** prev_gen, char** next_gen, subgrid_info* subgrid) {
  int alive_neighbours, changed;
  // First row
  for (int j = 0; j <= subgrid->cols+1; ++j) {
    if (j == 0) {
      alive_neighbours = (prev_gen[0][1] == '1' + prev_gen[1][0] == '1' + prev_gen[1][1] == '1');
    } else if (j == subgrid->cols+1) {
      alive_neighbours = (prev_gen[0][subgrid->cols] == '1' + prev_gen[1][subgrid->cols+1] == '1' +
                          prev_gen[1][subgrid->cols] == '1');
    } else {
      alive_neighbours = (prev_gen[0][j-1] == '1' + prev_gen[0][j+1] == '1' + prev_gen[1][j] == '1' +
                          prev_gen[1][j-1] == '1' + prev_gen[1][j+1] == '1');
    }
    if (exec_game_rules(prev_gen, next_gen, 0, j, alive_neighbours) == 1) {
      changed = 1;
    }
  }
  // Last row
  for (int j = 0; j <= subgrid->cols+1; ++j) {
    if (j == 0) {
      alive_neighbours = (prev_gen[subgrid->rows+1][1] == '1' + prev_gen[subgrid->rows][0] == '1' +
                          prev_gen[subgrid->rows][1] == '1');
    } else if (j == subgrid->cols+1) {
      alive_neighbours = (prev_gen[subgrid->rows+1][subgrid->cols] == '1' +
                          prev_gen[subgrid->rows][subgrid->cols+1] == '1' +
                          prev_gen[subgrid->rows][subgrid->cols] == '1');
    } else {
      alive_neighbours = (prev_gen[subgrid->rows+1][j-1] == '1' +
                          prev_gen[subgrid->rows+1][j+1] == '1' +
                          prev_gen[subgrid->rows][j] == '1' +
                          prev_gen[subgrid->rows][j-1] == '1' +
                          prev_gen[subgrid->rows][j+1] == '1');
    }
    if (exec_game_rules(prev_gen, next_gen, subgrid->rows+1, j, alive_neighbours) == 1) {
      changed = 1;
    }
  }
  // First column
  for (int i = 0; i <= subgrid->rows+1; ++i) {
    if (i == 0) {
      alive_neighbours = (prev_gen[0][1] == '1' + prev_gen[1][0] == '1' +
                          prev_gen[1][1] == '1');
    } else if (i == subgrid->rows+1) {
      alive_neighbours = (prev_gen[subgrid->rows+1][1] == '1' +
                          prev_gen[subgrid->rows][0] == '1' +
                          prev_gen[subgrid->rows][1] == '1');
    } else {
      alive_neighbours = (prev_gen[i-1][0] == '1' +
                          prev_gen[i+1][0] == '1' +
                          prev_gen[i][1] == '1' +
                          prev_gen[i-1][1] == '1' +
                          prev_gen[i+1][1] == '1');
    }
    if (exec_game_rules(prev_gen, next_gen, i, 0, alive_neighbours) == 1) {
      changed = 1;
    }
  }
  // Last column
  for (int i = 0; i <= subgrid->rows+1; ++i) {
    if (i == 0) {
      alive_neighbours = (prev_gen[0][subgrid->cols] == '1' +
                          prev_gen[1][subgrid->cols+1] == '1' +
                          prev_gen[1][subgrid->cols] == '1');
    } else if (i == subgrid->rows+1) {
      alive_neighbours = (prev_gen[subgrid->rows+1][subgrid->cols] == '1' +
                          prev_gen[subgrid->rows][subgrid->cols+1] == '1' +
                          prev_gen[subgrid->rows][subgrid->cols] == '1');
    } else {
      alive_neighbours = (prev_gen[i-1][subgrid->cols+1] == '1' +
                          prev_gen[i+1][subgrid->cols+1] == '1' +
                          prev_gen[i][subgrid->cols] == '1' +
                          prev_gen[i-1][subgrid->cols] == '1' +
                          prev_gen[i+1][subgrid->cols] == '1');
    }
    if (exec_game_rules(prev_gen, next_gen, i, subgrid->cols+1, alive_neighbours) == 1) {
      changed = 1;
    }
  }
}
