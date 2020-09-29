#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../headers/io_utils.h"
#include "../headers/report_utils.h"
#include "../headers/utils.h"

program_options options;

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

void parallel_write(const char* output_file, int rank, int size, subgrid_info* subgrid, char** local_grid) {
  int start_row = (rank / subgrid->div_factor) * subgrid->rows;
  int start_col = (rank % subgrid->div_factor) * subgrid->cols;

  MPI_File out_file_handle;
  MPI_Offset offset;
	MPI_File_open(MPI_COMM_WORLD, output_file, MPI_MODE_CREATE | MPI_MODE_WRONLY | MPI_MODE_APPEND, MPI_INFO_NULL, &out_file_handle);
  int err = MPI_File_get_position(out_file_handle, &offset);
  printf("OFFSET %d\n",offset);

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
  // Add '\n' in the end TODO
  if (rank == 3) {
    MPI_File_write(out_file_handle, "\n", 1, MPI_CHAR, MPI_STATUS_IGNORE);
  }
	free(line_buffer);

	MPI_File_close(&out_file_handle);
}

static inline
void __usage() {
    fprintf(stderr,
            "You must provide the following arguments:\n"
            "\t-l <Number of Loops>\n"
            "\t-i <Input File>\n"
            "\t-n <Grid size>\n");
    exit(EXIT_FAILURE);
}

void parse_command_line_arguments(int argc, char *argv[]) {
  if (argc < 5) __usage();
  int option;
  while (1) {
    int option_index;
    option = getopt_long_only(argc, argv, "l:i:n:", options_spec, &option_index);
    if (option == -1) break;
    switch (option) {
      case 'l': {
        uint64_t value;
        if (!str_to_uint64(optarg, &value)) {
          die("Invalid <Number of Loops> parameter. Exiting...");
        }
        options.loops = (int) value;
        break;
      }
      case 'i': {
        options.input_file = strdup(optarg);
        if (!file_exists(options.input_file)) {
          die("Input file <%s> does not exists. Exiting...", options.input_file);
        }
        break;
      }
      case 'n': {
        uint64_t value;
        if (!str_to_uint64(optarg, &value)) {
          die("Invalid <Grid size> parameter. Exiting...");
        }
        options.size = (int) value;
        if (options.size <= 0) {
          die("Grid size should be greater than 0. Exiting...");
        }
        break;
      }
      case 'h':
        __usage();
      case '?':
        break;
      default:
        abort();
    }
  }
}
