#include <math.h>
#include <stdio.h>

#include <mpi.h>

#include "../headers/mpi_utils.h"

void compute_neighbors(MPI_Comm comm2D, process_neighbors* p) {
  MPI_Cart_shift(comm2D, SHIFT_ROW, DISP, &p->south, &p->north);
  MPI_Cart_shift(comm2D, SHIFT_COL, DISP, &p->west, &p->east);
  p->south_west = (p->west > 0) ? p->south - 1 : -2;
  p->south_east = (p->east > 0) ? p->south + 1 : -2;
  p->north_west = (p->west > 0) ? p->north - 1 : -2;
  p->north_east = (p->east > 0) ? p->north + 1 : -2;
}

void split_grid(int rank, int num_processes, int size,
                subgrid_info* subgrid, process_grid* p_grid) {

  subgrid->div_factor = sqrt((double)num_processes);
  subgrid->rows = subgrid->cols = size / subgrid->div_factor;

  p_grid->start_row = (rank / subgrid->div_factor) * subgrid->rows;
  p_grid->start_col = (rank % subgrid->div_factor) * subgrid->cols;
}
