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

char** allocate_memory(int rows, int columns) {
  char* data = calloc(rows * columns, sizeof(char));
  memset(data, '0', rows * columns);
  char** arr = calloc(rows, sizeof(char*));
  for (int i = 0; i < rows; i++) {
    arr[i] = &(data[i*columns]);
  }
  return arr;
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
