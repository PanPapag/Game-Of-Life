#pragma once

#include <mpi.h>

#define SHIFT_ROW 0
#define SHIFT_COL 1
#define DISP 1

typedef struct neighbors{
    int west;
    int east;
    int north;
    int south;
    int south_west;
    int south_east;
    int north_west;
    int north_east;
} process_neighbors;

void compute_neighbors(MPI_Comm comm2D, process_neighbors* p);
