#ifndef __COMMON_IO_UTILS__
	#define __COMMON_IO_UTILS__

	#include <getopt.h>
	#include <stdbool.h>

  static struct option options_spec[] = {
    {"l",    required_argument, NULL, 'l'},
		{"r",    optional_argument, NULL, 'r'},
    {"i",    required_argument, NULL, 'i'},
		{"o",    optional_argument, NULL, 'o'},
		{"n",    required_argument, NULL, 'n'},
    {"help", no_argument,       NULL, 'h'},
    {0, 0, 0}
  };

  typedef struct program_options {
    int loops;
    char* input_file;
		char* output_file;
		int size;
		bool reduce;
  } program_options;

  void parse_command_line_arguments(int argc, char *argv[]);

#endif
