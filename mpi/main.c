#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include <mpi.h>

#include "../common/common_functions.h"

// we are going to run as:  mpiexec [-n nproc]./gol [-s size -l loops -i input]
int main(int argc, char** argv) {
    // initialize the mpi stuff
    int grid_size, n_loops;
    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // argument checking
    if (!strcmp(argv[1], "-s")) {
        grid_size = atoi(argv[2]);
    } else {
        printf("Run as : mpiexec [-n <n_processes>] ./gol -s <grid size> -l <maximum loops> [-i <input file>]");
        exit(EXIT_FAILURE);
    }
    if (!strcmp(argv[3], "-l")) {
        n_loops = atoi(argv[4]);
    } else {
        printf("Run as : mpiexec [-n <n_processes>] ./gol -s <grid size> -l <maximum loops> [-i <input file>]");
        exit(EXIT_FAILURE);
    }
    bool user_input = false;
    char* input_file_name;
    // user wants an extra argument, probably it's an input file
    // if (argv[5]) {
    //     if (strcmp(argv[5], "-i")) {
    //         user_input = true;
    //     } else {
    //         printf("Run as : mpiexec [-n <n_processes>] ./gol -s <grid size> -l <maximum loops> [-i <input file>]");
    //         exit(EXIT_FAILURE);
    //     }
    // } else {
    //     // user does not want an input file,
    //     user_input = false;
    //     input_file_name = strdup(argv[6]);
    // }

    char** grid;
    // if we have user input, read from the file and fill the grid
    if (user_input) {
        FILE* input = fopen(input_file_name, "r");
        grid = load_from_file(input);
    }
    // else, randomize the grid
    else {
        srand(time(NULL));
        grid = malloc(grid_size * sizeof(char*));
        for (int i = 0; i < grid_size; i++) {
            grid[i] = malloc(grid_size * sizeof(char));
            for (int j = 0; j < grid_size; j++) {
                grid[i][j] = rand() % 2;
            }
        }
    }
    for (int i = 0; i < grid_size; i++) {
        for (int j = 0; j < grid_size; j++) {
            printf("%c ",grid[i][j]);
        }
        printf("\n");
    }

    MPI_Finalize();

}
