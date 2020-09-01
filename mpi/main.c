#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include <mpi.h>

#include "../common/headers/io_utils.h"
#include "../common/headers/utils.h"

extern program_options options;

int main(int argc, char** argv) {
  // Initializing MPI stuff
  int size, rank;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  // Arguments' parsing
  parse_command_line_arguments(argc, argv);
  printf("TEST: %d %s\n", options.loops, options.input_file);

  char** grid = load_from_file(options.input_file);
  printf("Completed\n");
  MPI_Finalize();

  return 0;
}
