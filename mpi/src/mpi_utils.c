#include <math.h>
#include <stdio.h>

#include <mpi.h>

#include "../headers/mpi_utils.h"

void compute_neighbors(MPI_Comm comm2D, process_neighbors* p) {
  MPI_Cart_shift(comm2D, SHIFT_ROW, DISP, &p->north, &p->south);
  MPI_Cart_shift(comm2D, SHIFT_COL, DISP, &p->west, &p->east);
  p->south_west = (p->west > 0) ? p->south - 1 : -2;
  p->south_east = (p->east > 0) ? p->south + 1 : -2;
  p->north_west = (p->west > 0) ? p->north - 1 : -2;
  p->north_east = (p->east > 0) ? p->north + 1 : -2;

  if (p->south < 0) p->south = MPI_PROC_NULL;
  if (p->north < 0) p->north = MPI_PROC_NULL;
  if (p->west < 0) p->west = MPI_PROC_NULL;
  if (p->east < 0) p->east = MPI_PROC_NULL;
  if (p->south_west < 0) p->south_east = MPI_PROC_NULL;
  if (p->south_east < 0) p->south_east = MPI_PROC_NULL;
  if (p->north_west < 0) p->north_west = MPI_PROC_NULL;
  if (p->north_east < 0) p->north_east = MPI_PROC_NULL;
}

void split_grid(int rank, int num_processes, int size, subgrid_info* subgrid) {
  subgrid->div_factor = sqrt((double)num_processes);
  subgrid->rows = subgrid->cols = size / subgrid->div_factor;
}
