#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void HandleError( cudaError_t err,
                         const char *file,
                         int line ) {
    if (err != cudaSuccess) {
        printf( "%s in %s at line %d\n", cudaGetErrorString( err ),
                file, line );
        exit( EXIT_FAILURE );
    }
}

#define HANDLE_ERROR( err ) (HandleError( err, __FILE__, __LINE__ ))


#define BLOCK_SIZE 4

typedef struct options {
  int loops;
  int size;
  char* input_file;
  char* output_file;
} program_options;

program_options options;

void parse_command_line_arguments(int argc, char* argv[]) {
  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-l")) {
      options.loops = atoi(argv[i+1]);
    } else if (!strcmp(argv[i], "-n")) {
      options.size = atoi(argv[i+1]);
    } else if (!strcmp(argv[i], "-i")) {
      options.input_file = strdup(argv[i+1]);
    } else if (!strcmp(argv[i], "-o")) {
      options.output_file = strdup(argv[i+1]);
    }
  }
}

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
    } else {
      next_grid[index] = '0';
    }
  }
}

__global__ void evolution(char* cur_grid, char* next_grid, int size) {
  int x = blockIdx.x * BLOCK_SIZE + threadIdx.x;
  int y = blockIdx.y * BLOCK_SIZE + threadIdx.y;
  int index = x * size + y;

  int alive_neighbours = count_alive_neighbours(x, y, size, cur_grid);

  apply_game_rules(index, cur_grid, next_grid, alive_neighbours);
}

void read_input(const char* input_file, char* grid) {
  FILE *fp;
  int i = 0;
  char c;
  fp = fopen(input_file, "r");
  while ((c = (char) fgetc(fp)) != '\n') {
    grid[i++] = c;
  }
}

int main(int argc, char* argv[]) {
  char* h_grid;            // Grid on host
  char* d_grid;            // Grid on device
  char* d_next_gen_grid;   // Next generation grid used on device
  char* d_tmp_grid;        // tmp grid pointer used to switch between grid and next_gen_grid
  float time;
  cudaEvent_t start, stop;

  parse_command_line_arguments(argc, argv);

  size_t grid_bytes = options.size * options.size * sizeof(char);

  // Allocate memory for host grid
  h_grid = (char*) malloc(grid_bytes);

  // Allocate memory for device grids
  cudaMalloc((void **)&d_grid, grid_bytes);
  cudaMalloc((void **)&d_next_gen_grid, grid_bytes);

  // Read input file to host grid and copy over device grid
  read_input(options.input_file, h_grid);
  cudaMemcpy(d_grid, h_grid, grid_bytes, cudaMemcpyHostToDevice);

  // Define block size as well as number of blocks
  dim3 block_size(BLOCK_SIZE, BLOCK_SIZE);
	dim3 grid_size(options.size / BLOCK_SIZE, options.size / BLOCK_SIZE);

  HANDLE_ERROR(cudaEventCreate(&start));
  HANDLE_ERROR(cudaEventCreate(&stop));
  HANDLE_ERROR(cudaEventRecord(start, 0));

  for (int i = 0; i < options.loops; ++i) {
    evolution<<<grid_size, block_size>>>(d_grid, d_next_gen_grid, options.size);
    d_tmp_grid = d_grid;
    d_grid = d_next_gen_grid;
    d_next_gen_grid = d_tmp_grid;
    cudaMemcpy(h_grid, d_next_gen_grid, grid_bytes, cudaMemcpyDeviceToHost);
  }

  // Copy results back to host grid
  cudaMemcpy(h_grid, d_grid, grid_bytes, cudaMemcpyDeviceToHost);

  HANDLE_ERROR(cudaEventRecord(stop, 0));
  HANDLE_ERROR(cudaEventSynchronize(stop));
  HANDLE_ERROR(cudaEventElapsedTime(&time, start, stop));

  printf("Time elapsed:  %3.1f ms\n", time);

  // Free resources
  cudaFree(d_grid);
  cudaFree(d_next_gen_grid);
  free(h_grid);
  free(options.input_file);
  free(options.output_file);

  return 0;
}
