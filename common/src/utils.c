#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../headers/utils.h"

char** evolution(char** old_grid, int size) {
  char** new_grid = malloc(size * sizeof(char*));
  for (int i = 0; i < size; i++) {
    new_grid[i] = malloc(size * sizeof(char));
    for (int j = 0; j < size; j++) {
      // compute how many organizations exist near the cell we are about to evolve
      int alive_neighbours = (old_grid[i-1][j-1] == '1') + (old_grid[i-1][j] == '1') +
                             (old_grid[i-1][j+1] == '1') + (old_grid[i][j-1] == '1') +
                             (old_grid[i][j+1] == '1') + (old_grid[i+1][j-1] == '1') +
                             (old_grid[i+1][j] == '1') +(old_grid[i+1][j+1] == '1');
      // rules regarding an alive cell
      if (old_grid[i][j] == '1') {
        // 0 or 1 neighbours -> the cell dies
        if (alive_neighbours < 2) {
          new_grid[i][j] = '0';
        }
        // 2 or 3 neighbours -> the cell survives
        else if (alive_neighbours < 4) {
          new_grid[i][j] = '1';
        }
        // more than 4 neighbours -> the cell dies due to overpopulation
        else {
          new_grid[i][j] = '0';
        }
      }
      // rules regarding dead cells
      else {
        // exactly 3 neighbours -> a new cell is born
        if (alive_neighbours == 3) {
          new_grid[i][j] = '1';
        }
      }
    }
    free(old_grid[i]);
  }
  free(old_grid);

  return new_grid;
}

char** load_from_file(const char* filepath) {
  // open file
  FILE* input = fopen(filepath, "r");
  // the input file should be as following:
  // 1st line: size (n * n)
  // each line after: n chars (followed by space), 0->dead 1->alive
  char* first_line = malloc(2 * sizeof(char));
  fscanf(input, "%s\n", first_line);
  int size = atoi(first_line);
  char** grid = malloc(size * sizeof(char*));
  for (int i = 0; i < size; i++) {
    grid[i] = malloc(size * sizeof(char));
    for (int j = 0; j < size; j++) {
      char current;
      // read each character from the file, and assign it to the grid
      current = getc(input);
      grid[i][j] = current;
    }
    // read the \n
    getc(input);
  }
  return grid;
}

bool str_to_ui64_with_end_ptr(const char* source, uint64_t* destination, char** end_ptr) {
  errno = 0;
  uint64_t value = strtol(source, end_ptr, 10);
  if ((errno == ERANGE && value == LONG_MAX)
      || (errno != 0 && value == 0U)
      || (end_ptr != NULL && *end_ptr == source)) {
      return false;
  }
  *destination = value;
  return true;
}

bool str_to_uint64(const char* source, uint64_t* destination) {
  return str_to_ui64_with_end_ptr(source, destination, NULL);
}

bool file_exists(const char* restrict path) {
  struct stat info = {0};
  return stat(path, &info) == 0;
}
