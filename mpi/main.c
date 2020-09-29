#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include <mpi.h>

#include "../common/headers/io_utils.h"
#include "../common/headers/game_utils.h"
#include "../common/headers/utils.h"
#include "./headers/mpi_utils.h"

extern program_options options;

int main(int argc, char** argv) {
  int ndims=2, size, my_rank, reorder, my_cart_rank, ierr, errs, nrows, ncols;
  int local_start_time, local_time_elapsed;
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

  // MAIN ALGORITHM
  MPI_Request send_req[8], recv_req[8];
  MPI_Status status[8];

  // printf("Rank: %d - S: %d - N: %d - W: %d - E: %d SW: %d - SE: %d - NW: %d - NE: %d\n",
  //   my_rank, p.south, p.north, p.west, p.east, p.south_west, p.south_east, p.north_west, p.north_east);

  MPI_Barrier(MPI_COMM_WORLD);
  local_start_time = MPI_Wtime();

  // Top
  MPI_Irecv(&local_grid[0][1], 1, row_datatype, p.north, 0, comm2D, &recv_req[0]);
  MPI_Isend(&local_grid[1][1], 1, row_datatype, p.north, 0, comm2D, &send_req[0]);

  // Bottom
  MPI_Irecv(&local_grid[subgrid.rows+1][1], 1, row_datatype, p.south, 0, comm2D, &recv_req[1]);
  MPI_Isend(&local_grid[subgrid.rows][1], 1, row_datatype, p.south, 0, comm2D, &send_req[1]);

  // Right
  MPI_Irecv(&local_grid[1][subgrid.cols+1], 1, column_datatype, p.east, 0, comm2D, &recv_req[2]);
  MPI_Isend(&local_grid[1][subgrid.cols], 1, column_datatype, p.east, 0, comm2D, &send_req[2]);

  // Left
  MPI_Irecv(&local_grid[1][0], 1, column_datatype, p.west, 0, comm2D, &recv_req[3]);
  MPI_Isend(&local_grid[1][1], 1, column_datatype, p.west, 0, comm2D, &send_req[3]);

  // Top-Right
  MPI_Irecv(&local_grid[0][subgrid.cols+1], 1, MPI_CHAR, p.north_east, 0, comm2D, &recv_req[4]);
  MPI_Isend(&local_grid[1][subgrid.cols], 1, MPI_CHAR, p.north_east, 0, comm2D, &send_req[4]);

  // Top-Left
  MPI_Irecv(&local_grid[0][0], 1, MPI_CHAR, p.north_west, 0, comm2D, &recv_req[5]);
  MPI_Isend(&local_grid[1][1], 1, MPI_CHAR, p.north_west, 0, comm2D, &send_req[5]);

  // Bottom-Right
  MPI_Irecv(&local_grid[subgrid.rows+1][subgrid.cols+1], 1, MPI_CHAR, p.south_east, 0, comm2D, &recv_req[6]);
  MPI_Isend(&local_grid[subgrid.rows][subgrid.cols], 1, MPI_CHAR, p.south_east, 0, comm2D, &send_req[6]);

  // Bottom-Left
  MPI_Irecv(&local_grid[subgrid.rows+1][0], 1, MPI_CHAR, p.south_west, 0, comm2D, &recv_req[7]);
  MPI_Isend(&local_grid[subgrid.rows][1], 1, MPI_CHAR, p.south_west, 0, comm2D, &send_req[7]);

  MPI_Waitall(8, recv_req, status);

  // ADD ALGORITHM for calculate_inner_gen / calculate_outter_gen

  MPI_Waitall(8, send_req, status);

  local_time_elapsed = MPI_Wtime() - local_start_time;

  // parallel_write("../test", my_rank, options.size, &subgrid, local_grid);

  MPI_Finalize();

  return 0;
}
