#ifndef __COMMON_IO_UTILS__
	#define __COMMON_IO_UTILS__

	#include <getopt.h>

  static struct option options_spec[] = {
    {"l",    required_argument, NULL, 'l'},
    {"i",    required_argument, NULL, 'i'},
    {"help", no_argument,       NULL, 'h'},
    {0, 0, 0}
  };

  typedef struct program_options {
    int loops;
    char* input_file;
  } program_options;

  void parse_command_line_arguments(int argc, char *argv[]);

#endif