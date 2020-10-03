#include <stdio.h>
#include <stdlib.h>

#include "../common/headers/io_utils.h"
#include "../common/headers/utils.h"

#define BLOCK_SIZE 4

extern program_options options;

__device__ char get_cell_inbounds(int x, int y, int size, char* grid) {
  if (x >= 0 && x < size && y >= 0 && y < size) {
    return grid[x * size + y];
  }
  return '0';
}

__device__ int count_alive_neighbours(int x, int y, int size, char* grid) {
  int alive_neighbours = (get_cell_inbounds(x-1, y, size, grid) == '1') +
                         (get_cell_inbounds(x+1, y, size, grid) == '1') +
                         (get_cell_inbounds(x, y-1, size, grid) == '1') +
                         (get_cell_inbounds(x, y+1, size, grid) == '1') +
                         (get_cell_inbounds(x-1, y-1, size, grid) == '1') +
                         (get_cell_inbounds(x-1, y+1, size, grid) == '1') +
                         (get_cell_inbounds(x+1, y-1, size, grid) == '1') +
                         (get_cell_inbounds(x+1, y+1, size, grid) == '1');
  return alive_neighbours;
}

__device__ void apply_game_rules(int index, char* cur_grid, char* next_grid, int alive_neighbours) {
  if (cur_grid[index] == '1') {
    // 0 or 1 neighbours -> the cell dies
    if (alive_neighbours < 2) {
      next_grid[index] = '0';
    }
    // 2 or 3 neighbours -> the cell survives
    else if (alive_neighbours < 4) {
      next_grid[index] = '1';
    }
    // more than 4 neighbours -> the cell dies due to overpopulation
    else {
      next_grid[index] = '0';
    }
  }
  // rules regarding dead cells
  else {
    // exactly 3 neighbours -> a new cell is born
    if (alive_neighbours == 3) {
      next_grid[index] = '1';
    }
  }
}

__global__ void evolution(char* cur_grid, char* next_grid, int size) {
  int x = blockDim.x * BLOCK_SIZE + threadIdx.x;
  int y = blockDim.y * BLOCK_SIZE + threadIdx.y;
  int index = x * size + y;

  int alive_neighbours = count_alive_neighbours(x, y, size, cur_grid);

  apply_game_rules(index, cur_grid, next_grid, alive_neighbours);
}

void read_input(const char* input_file, char* grid) {
  int i = 0;
  char c;
  while ((c = getchar()) != '\n') {
    grid[i++] = c;
  }
}

int main(int argc, char* argv[]) {
  char* h_grid;            // Grid on host
  char* d_grid;            // Grid on device
  char* d_next_gen_grid;   // Next generation grid used on device
  char* d_tmp_grid;        // tmp grid pointer used to switch between grid and next_gen_grid

  parse_command_line_arguments(argc, argv);

  size_t grid_bytes = options.size * options.size * sizeof(char);

  // Allocate memory for host grid
  h_grid = (char*) malloc(grid_bytes);

  // Allocate memory for device grids
  cudaMalloc(&d_grid, grid_bytes);
  cudaMalloc(&d_next_gen_grid, grid_bytes);

  // Read input file to host grid and copy over device grid
  read_input(options.input_file, h_grid);
  cudaMemcpy(d_grid, h_grid, grid_bytes, cudaMemcpyHostToDevice);

  // Define block size as well as number of blocks
  dim3 block_size(BLOCK_SIZE, BLOCK_SIZE);
	dim3 grid_size(options.size / BLOCK_SIZE, options.size / BLOCK_SIZE);

  for (int i = 0; i < options.loops; ++i) {
    evolution<<<grid_size, block_size>>>(d_grid, d_next_gen_grid, options.size);
    // Swap grids
    d_tmp_grid = d_grid;
    d_grid = d_next_gen_grid;
    d_next_gen_grid = d_tmp_grid;
  }

  // Copy results back to host grid
  cudaMemcpy(h_grid, d_grid, grid_bytes, cudaMemcpyDeviceToHost);

  // Free resources
  cudaFree(d_grid);
  cudaFree(d_next_gen_grid);
  free(h_grid);

  return 0;
}
