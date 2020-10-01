#ifndef __GAME_UTILS__
  #define __GAME_UTILS__

  #include "../../common/headers/utils.h"

  int exec_game_rules(char** prev_gen, char** next_gen, int i, int j, int alive_neighbours);

  int calculate_inner_gen(char** prev_gen, char** next_gen, subgrid_info* subgrid);

  int calculate_outter_gen(char** prev_gen, char** next_gen, subgrid_info* subgrid);

#endif
