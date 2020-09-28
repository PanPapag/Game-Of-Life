#ifndef __COMMON_UTILS__
  #define __COMMON_UTILS__

  #include <stdbool.h>
  #include <stddef.h>
  #include <stdint.h>

  char** evolution(char** old_grid, int size);

  char** allocate_memory(int rows, int columns);
  
  bool str_to_ui64_with_end_ptr(const char *source, uint64_t *destination, char **end_ptr);

  bool str_to_uint64(const char *source, uint64_t *destination);

  bool file_exists(const char *restrict path);

#endif
