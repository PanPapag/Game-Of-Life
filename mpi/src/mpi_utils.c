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
