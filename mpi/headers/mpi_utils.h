#pragma once

#include <mpi.h>

#define SHIFT_ROW 0
#define SHIFT_COL 1
#define DISP 1

typedef struct neighbors {
  int west;
  int east;
  int north;
  int south;
  int south_west;
  int south_east;
  int north_west;
  int north_east;
} process_neighbors;

typedef struct subgrid {
  int div_factor;
  int cols;
  int rows;
} subgrid_info;

void compute_neighbors(MPI_Comm comm2D, process_neighbors* p);

void split_grid(int my_rank, int num_processes, int size, subgrid_info* subgrid);
