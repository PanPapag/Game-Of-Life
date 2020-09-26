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

  // Read grid TODO: Implement parallel read
  char** grid = load_from_file(options.input_file);

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

  if (my_rank == 0) {
    for (int i = 0; i < options.size; ++i) {
      for (int j = 0; j < options.size; ++j) {
        printf("%c ", grid[i][j]);
      }
      printf("\n");
    }
  }
  // // find my coordinates in the cartesian communicator group
  // MPI_Cart_coords(comm2D, my_rank, ndims, coord);
  // // use my cartesian coordinates to find my rank in cartesian group
  // MPI_Cart_rank(comm2D, coord, &my_cart_rank);
  // // printf("PW[%2d] Coord(%d,%d)\n", my_rank, coord[0], coord[1]);

  // Compute standard neighbors of each process
  process_neighbors p;
  compute_neighbors(comm2D, &p);
  // printf("PW[%2d] ---> W: %d - E: %d -  S: %d - N: %d - SW: %d - SE: %d - NW: %d - NE: %d\n",
  //   my_rank, p.west, p.east, p.south, p.north, p.south_west, p.south_east, p.north_west, p.north_east);

  // Split the grid and compute start and end row/column for each process
  subgrid_info subgrid;
  process_grid p_grid;
  split_grid(my_rank, size, options.size, &subgrid, &p_grid);
  // printf("P[%d] SR: %d - SC: %d\n",my_rank, p_grid.start_row, p_grid.start_col);

  // Create row and column MPI_datatypes
  MPI_Datatype column_datatype;
  MPI_Datatype row_datatype;

  MPI_Type_vector(subgrid.rows, 1, options.size, MPI_INT, &column_datatype);
  MPI_Type_contiguous(subgrid.cols, MPI_CHAR, &row_datatype);
  MPI_Type_commit(&column_datatype);
  MPI_Type_commit(&row_datatype);

  enum rank_roles { SENDER, RECEIVER };
  switch(my_rank)
  {
      case RECEIVER:
      {
        // Receive the message
        char received[subgrid.rows];
        MPI_Recv(&received, subgrid.rows, MPI_INT, SENDER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("MPI process %d received values.\n", my_rank);
        for (int i = 0; i < subgrid.rows; ++i) {
          printf("%d\n", received[i]);
        }
        break;
      }
      case SENDER:
      {
        // Send the message
        int buffer[8][8] = {1, 1, 0, 0, 0, 0, 1 ,1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1 ,1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1 ,1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1 ,1, 0, 1, 0, 1, 0, 1, 0, 1};
        MPI_Request request;
        printf("MPI process %d sends values - SR: %d - SC: %d - R/C: %d/%d\n", my_rank, p_grid.start_row, p_grid.start_col, subgrid.rows, subgrid.cols);
        MPI_Send(&buffer[p_grid.start_row][p_grid.start_col], 1, column_datatype, RECEIVER, 0, MPI_COMM_WORLD);
        break;
      }
  }



  MPI_Finalize();

  return 0;
}
