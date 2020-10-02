#include <stdio.h>
#include <stdlib.h>

#include "../../common/headers/io_utils.h"
#include "../../common/headers/utils.h"

#define BLOCK_SIZE 4

extern program_options options;

__device__ char get_cell_inbounds(int x, int y, char* grid) {
  if (x >=0 && x < options.size && y >= 0 && y < options.size) {
    return grid[x * options.size + y];
  }
  return '0';
}

__device__ void count_alive_neighbors(int x, int y, char* cur_grid) {
  int alive_neighbors = 0;

  alive_neighbors += 

}

__global__ void evolution(char* cur_grid, char* next_grid) {
  int x = blockDim.x * BLOCK_SIZE + threadIdx.x;
  int y = blockDim.y * BLOCK_SIZE + threadIdx.y;
  int index = x * options.size + y;

  int alive_neighbors = count_alive_neighbors(x, y, cur_grid);
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
  dim3 block_size(BLOCK_SIDE, BLOCK_SIDE);
	dim3 grid_size(options.size / BLOCK_SIZE, options.size / BLOCK_SIZE);

  for (int i = 0; i < options.loops; ++i) {

    // Swap grids
    d_tmp_grid = d_grid;
    d_grid = d_next_gen_grid;
    d_next_gen_grid = d_tmp_rid;
  }

  // Copy results back to host grid
  cudaMemcpy(h_grid, d_grid, grid_bytes, cudaMemcpyDeviceToHost);

  // Free resources
  cudaFree(d_grid);
  cudaFree(d_next_gen_grid);
  free(h_grid);

  return 0;
}
