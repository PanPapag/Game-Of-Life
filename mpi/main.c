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
  printf("TEST: %d %s\n", options.loops, options.input_file);

  // Read grid TODO: Implement parallel read
  char** grid = load_from_file(options.input_file);

  /*************************************************************/
  /* create cartesian topology for processes */
  /*************************************************************/
  nrows = ncols = (int) sqrt(size);
  dims[0] = dims[1] = nrows;
  MPI_Dims_create(size, ndims, dims);
  /* create cartesian mapping */
  wrap_around[0] = wrap_around[1] = 0;  // periodic shift is .false.
  reorder = 1;
  ierr = 0;
  ierr = MPI_Cart_create(MPI_COMM_WORLD, ndims, dims, wrap_around, reorder, &comm2D);
  if (ierr != 0) {
    printf("ERROR[%d] creating CART\n",ierr);
  }

  /* find my coordinates in the cartesian communicator group */
  MPI_Cart_coords(comm2D, my_rank, ndims, coord);
  /* use my cartesian coordinates to find my rank in cartesian group*/
  MPI_Cart_rank(comm2D, coord, &my_cart_rank);

  // printf("PW[%2d] Coord(%d,%d)\n", my_rank, coord[0], coord[1]);

  process_neighbors p;
  compute_neighbors(comm2D, &p);

  // printf("PW[%2d] ---> W: %d - E: %d -  S: %d - N: %d - SW: %d - SE: %d - NW: %d - NE: %d\n",
  //   my_rank, p.west, p.east, p.south, p.north, p.south_west, p.south_east, p.north_west, p.north_east);

  MPI_Finalize();

  return 0;
}
