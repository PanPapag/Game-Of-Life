#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include <mpi.h>

#include "../common/headers/io_utils.h"
#include "../common/headers/utils.h"
#include "./headers/mpi_utils.h"

extern program_options options;

int main(int argc, char** argv) {
  int ndims=2, size, my_rank, reorder, my_cart_rank, ierr, errs, nrows, ncols;
  int dims[ndims], coord[ndims];
  int wrap_around[ndims];
  MPI_Comm comm2D;

  // Initializing MPI stuff
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  // Arguments' parsing
  parse_command_line_arguments(argc, argv);

  /*****************************************************************/
  /* Create cartesian topology for processes and compute neighbors */
  /*****************************************************************/
  nrows = ncols = (int) sqrt(size);
  dims[0] = dims[1] = nrows;
  MPI_Dims_create(size, ndims, dims);
  // Create cartesian mapping
  wrap_around[0] = wrap_around[1] = 0;  // periodic shift is .false.
  reorder = 1;
  ierr = 0;
  ierr = MPI_Cart_create(MPI_COMM_WORLD, ndims, dims, wrap_around, reorder, &comm2D);
  if (ierr != 0) {
    printf("ERROR[%d] creating CART\n",ierr);
  }

  // Compute standard neighbors of each process
  process_neighbors p;
  compute_neighbors(comm2D, &p);

  /*********************************************************************/
  /* Split the grid to rows/cols per process and perform parallel read */
  /*********************************************************************/

  // Split the grid to rows/cols per process
  subgrid_info subgrid;
  split_grid(my_rank, size, options.size, &subgrid);

  // Paralell read using MPI I/0
  char** local_grid = parallel_read(options.input_file, my_rank, options.size, &subgrid);

  // Create row and column MPI_datatypes
  MPI_Datatype column_datatype;
  MPI_Datatype row_datatype;

  MPI_Type_vector(subgrid.rows, 1, subgrid.cols+2, MPI_CHAR, &column_datatype);
  MPI_Type_contiguous(subgrid.cols, MPI_CHAR, &row_datatype);
  MPI_Type_commit(&column_datatype);
  MPI_Type_commit(&row_datatype);

  MPI_Finalize();

  return 0;
}
