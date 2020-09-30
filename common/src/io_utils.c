#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../headers/io_utils.h"
#include "../headers/report_utils.h"
#include "../headers/utils.h"

program_options options;

static inline
void __usage() {
    fprintf(stderr,
            "You must provide the following arguments:\n"
            "\t-l <Number of Loops>\n"
            "\t-r <Reduce Flag>\n"
            "\t-i <Input File>\n"
            "\t-o <Output File>"
            "\t-n <Grid size>\n");
    exit(EXIT_FAILURE);
}

void parse_command_line_arguments(int argc, char *argv[]) {
  if (argc < 9) __usage();
  int option;
  while (1) {
    int option_index;
    option = getopt_long_only(argc, argv, "l:i:o:n:r:", options_spec, &option_index);
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
      case 'r': {
        options.reduce = (bool) optarg;
        break;
      }
      case 'i': {
        options.input_file = strdup(optarg);
        if (!file_exists(options.input_file)) {
          die("Input file <%s> does not exists. Exiting...", options.input_file);
        }
        break;
      }
      case 'o': {
        options.output_file = strdup(optarg);
        if (file_exists(options.output_file)) {
          remove(options.output_file);
        } else {
          FILE* fp = fopen(options.output_file, "w");
          fclose(fp);
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
