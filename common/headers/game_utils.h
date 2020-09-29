#ifndef __GAME_UTILS__
  #define __GAME_UTILS__

  #include <stdbool.h>
  #include <stddef.h>
  #include <stdint.h>

  #include "../../mpi/headers/mpi_utils.h"

  void calculate_inner_gen(char** prev_gen, char** next_gen, subgrid_info* subgrid);

  void calculate_outter_gen(char** prev_gen, char** next_gen, subgrid_info* subgrid);

#endif
