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

void run_game(char** local_grid, int rank, int no_processes,
              MPI_Comm comm2D, process_neighbors* p, subgrid_info* subgrid);

void compute_neighbors(MPI_Comm comm2D, process_neighbors* p);

void split_grid(int my_rank, int num_processes, int size, subgrid_info* subgrid);

// Paralell I/O functions
char** parallel_read(const char* input_file, int rank, int size, subgrid_info* subgrid);
void parallel_write(const char* output_file, int rank, int no_processes,
                    int size, subgrid_info* subgrid, char** local_grid);
