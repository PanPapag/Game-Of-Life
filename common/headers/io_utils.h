#ifndef __COMMON_IO_UTILS__
	#define __COMMON_IO_UTILS__

	#include <getopt.h>

	#include "../../mpi/headers/mpi_utils.h"

  static struct option options_spec[] = {
    {"l",    required_argument, NULL, 'l'},
    {"i",    required_argument, NULL, 'i'},
		{"n",    required_argument, NULL, 'n'},
    {"help", no_argument,       NULL, 'h'},
    {0, 0, 0}
  };

  typedef struct program_options {
    int loops;
    char* input_file;
		int size;
  } program_options;

  void parse_command_line_arguments(int argc, char *argv[]);

	// Paralell I/O functions
	char** parallel_read(const char* input_file, int rank, int size, subgrid_info* subgrid);
	void parallel_write(const char* output_file, int rank, int size, subgrid_info* subgrid, char** local_grid);

#endif
