#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <mpi.h>

#include "../headers/game_utils.h"
#include "../headers/mpi_utils.h"

#include "../../common/headers/io_utils.h"

extern program_options options;

void run_game(char** local_grid, int rank, int no_processes,
              MPI_Comm comm2D, process_neighbors* p, subgrid_info* subgrid) {

  // Create row and column MPI_datatypes
  MPI_Datatype column_datatype;
  MPI_Datatype row_datatype;

  MPI_Type_vector(subgrid->rows, 1, subgrid->cols+2, MPI_CHAR, &column_datatype);
  MPI_Type_contiguous(subgrid->cols, MPI_CHAR, &row_datatype);
  MPI_Type_commit(&column_datatype);
  MPI_Type_commit(&row_datatype);

  MPI_Request send_req[8], recv_req[8];
  MPI_Status status[8];

  int local_change, changed;

  // Allocate memory for next_gen local grid and declare temp to swap prev_gen
  // with next gen
  char** next_gen = allocate_memory(subgrid->rows+2, subgrid->cols+2);
  char** temp;

  for (int i = 0; i < options.loops; ++i) {
    // Top
    MPI_Irecv(&local_grid[0][1], 1, row_datatype, p->north, 0, comm2D, &recv_req[0]);
    MPI_Isend(&local_grid[1][1], 1, row_datatype, p->north, 0, comm2D, &send_req[0]);

    // Bottom
    MPI_Irecv(&local_grid[subgrid->rows+1][1], 1, row_datatype, p->south, 0, comm2D, &recv_req[1]);
    MPI_Isend(&local_grid[subgrid->rows][1], 1, row_datatype, p->south, 0, comm2D, &send_req[1]);

    // Right
    MPI_Irecv(&local_grid[1][subgrid->cols+1], 1, column_datatype, p->east, 0, comm2D, &recv_req[2]);
    MPI_Isend(&local_grid[1][subgrid->cols], 1, column_datatype, p->east, 0, comm2D, &send_req[2]);

    // Left
    MPI_Irecv(&local_grid[1][0], 1, column_datatype, p->west, 0, comm2D, &recv_req[3]);
    MPI_Isend(&local_grid[1][1], 1, column_datatype, p->west, 0, comm2D, &send_req[3]);

    // Top-Right
    MPI_Irecv(&local_grid[0][subgrid->cols+1], 1, MPI_CHAR, p->north_east, 0, comm2D, &recv_req[4]);
    MPI_Isend(&local_grid[1][subgrid->cols], 1, MPI_CHAR, p->north_east, 0, comm2D, &send_req[4]);

    // Top-Left
    MPI_Irecv(&local_grid[0][0], 1, MPI_CHAR, p->north_west, 0, comm2D, &recv_req[5]);
    MPI_Isend(&local_grid[1][1], 1, MPI_CHAR, p->north_west, 0, comm2D, &send_req[5]);

    // Bottom-Right
    MPI_Irecv(&local_grid[subgrid->rows+1][subgrid->cols+1], 1, MPI_CHAR, p->south_east, 0, comm2D, &recv_req[6]);
    MPI_Isend(&local_grid[subgrid->rows][subgrid->cols], 1, MPI_CHAR, p->south_east, 0, comm2D, &send_req[6]);

    // Bottom-Left
    MPI_Irecv(&local_grid[subgrid->rows+1][0], 1, MPI_CHAR, p->south_west, 0, comm2D, &recv_req[7]);
    MPI_Isend(&local_grid[subgrid->rows][1], 1, MPI_CHAR, p->south_west, 0, comm2D, &send_req[7]);

    // Calculate inner data points (white)
    local_change = calculate_inner_gen(local_grid, next_gen, subgrid);

    MPI_Waitall(8, recv_req, status);

    // Calculate outter data points (green)
    local_change = calculate_outter_gen(local_grid, next_gen, subgrid);

    // Swap local arrays (previous generation <-> next generation)
    temp = local_grid;
  	local_grid = next_gen;
  	next_gen = temp;

    if (options.reduce) {
      MPI_Allreduce(&local_change, &changed, 1, MPI_INT, MPI_SUM, comm2D);
      if (changed == 0) {
        if (rank == 0) {
          printf("Terminated at loop: %d out of %d\n", i+1, options.loops);
        }
        break;
      }
    }

    MPI_Waitall(8, send_req, status);

    // Write i-th generation using parallel I/O
    parallel_write(options.output_file, rank, no_processes, options.size, subgrid, local_grid);
  }

  // Free resources
  MPI_Type_free(&column_datatype);
  MPI_Type_free(&row_datatype);

  free(local_grid[0]);
	free(local_grid);

  free(next_gen[0]);
  free(next_gen);
}

char** parallel_read(const char* input_file, int rank, int size, subgrid_info* subgrid) {
  int start_row = (rank / subgrid->div_factor) * subgrid->rows;
  int start_col = (rank % subgrid->div_factor) * subgrid->cols;

  MPI_File in_file_handle;
	MPI_File_open(MPI_COMM_WORLD, input_file, MPI_MODE_RDONLY, MPI_INFO_NULL, &in_file_handle);

  char* line_buffer = malloc(subgrid->cols * sizeof(char));

  char** local_grid = allocate_memory(subgrid->rows+2, subgrid->cols+2);

  for (int row = 0; row != subgrid->rows; ++row) {
    int row_pos = (start_row + row) * size;
		int read_pos = (row_pos + start_col);

		MPI_File_seek(in_file_handle, read_pos, MPI_SEEK_SET);
		MPI_File_read(in_file_handle, line_buffer, subgrid->cols, MPI_CHAR, MPI_STATUS_IGNORE);

		for (int i = 0; i != subgrid->cols; ++i)
			local_grid[row+1][i+1] = (char) line_buffer[i];
	}

	free(line_buffer);

	MPI_File_close(&in_file_handle);

  return local_grid;
}

void parallel_write(const char* output_file, int rank, int no_processes, int size, subgrid_info* subgrid, char** local_grid) {
  int start_row = (rank / subgrid->div_factor) * subgrid->rows;
  int start_col = (rank % subgrid->div_factor) * subgrid->cols;

  MPI_File out_file_handle;
  MPI_Offset offset;
	MPI_File_open(MPI_COMM_WORLD, output_file, MPI_MODE_CREATE | MPI_MODE_WRONLY | MPI_MODE_APPEND, MPI_INFO_NULL, &out_file_handle);
  int err = MPI_File_get_position(out_file_handle, &offset);

	char* line_buffer = malloc(subgrid->cols * sizeof(char));

	for (int row = 0; row != subgrid->rows; ++row) {
		int row_pos = (start_row + row) * size;
		int write_pos = offset + (row_pos + start_col);

    MPI_File_seek(out_file_handle, write_pos, MPI_SEEK_SET);

		for (int i = 0; i != subgrid->cols; ++i) {
			line_buffer[i] = (char) local_grid[row+1][i+1];
    }

		MPI_File_write(out_file_handle, line_buffer, subgrid->cols, MPI_CHAR, MPI_STATUS_IGNORE);
	}

  if (rank == no_processes - 1) {
    MPI_File_write(out_file_handle, "\n", 1, MPI_CHAR, MPI_STATUS_IGNORE);
  }
	free(line_buffer);

	MPI_File_close(&out_file_handle);
}

void compute_neighbors(MPI_Comm comm2D, process_neighbors* p) {
  MPI_Cart_shift(comm2D, SHIFT_ROW, DISP, &p->north, &p->south);
  MPI_Cart_shift(comm2D, SHIFT_COL, DISP, &p->west, &p->east);
  p->north_west = (p->west >= 0) ? p->north - 1 : -2;
  p->north_east = (p->east >= 0) ? p->north + 1 : -2;
  p->south_west = (p->west >= 0) ? p->south - 1 : -2;
  p->south_east = (p->east >= 0) ? p->south + 1 : -2;

  if (p->south < 0) p->south = MPI_PROC_NULL;
  if (p->north < 0) p->north = MPI_PROC_NULL;
  if (p->west < 0) p->west = MPI_PROC_NULL;
  if (p->east < 0) p->east = MPI_PROC_NULL;
  if (p->south_west < 0) p->south_west = MPI_PROC_NULL;
  if (p->south_east < 0) p->south_east = MPI_PROC_NULL;
  if (p->north_west < 0) p->north_west = MPI_PROC_NULL;
  if (p->north_east < 0) p->north_east = MPI_PROC_NULL;
}

void split_grid(int rank, int num_processes, int size, subgrid_info* subgrid) {
  subgrid->div_factor = sqrt((double)num_processes);
  subgrid->rows = subgrid->cols = size / subgrid->div_factor;
}
