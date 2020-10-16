# Game-Of-Life

## About GoL

John Conway came up with the Game of Life in 1970. The game demonstrates the fact that some simple local rules can lead to interesting large-scale life behavior(birth, reproduction and death). The game is played in a 2 dimensional grid N x N, made of cells, that can be either alive, or dead. The game does not have any players, thus it does not require any input by the user. Each cell has at  most 8 neighbours, that determine its state in the next generation. The re-formation of the grid from generation to generation is done simultaneously, meaning that each state in the next generation depends exclusively in the state of the cell and its neighbours. __Our goal__ is to implement this game, using __Parallel programming__. This program is a pure example of the value of parallelization, because of the grid, and the ability to split it to equal pieces.

## Implementation

Our code for this assignment is spitted into many files. We have a directory which contains common files used by all the methods that we have developed. For each method (MPI, OpenMP, CUDA), we have a directory containing all the useful files. In the directory misc, there are various input and output files, while in the directory plots there are vrious plots regarding our time achievements while running this program in a supercomputer.

### MPI - OpenMP

The program takes as input a  (N x N grid, which is provided in a simple text file. The data (0s and 1s) must be all in one row. So, if for example we want a  4 x 4 grid, we are going to provide a file containing 16 characters.

Our goal was to design the MPI code by following the given instructions. Our main objectives were:
   - Reduction of idle time
   - Reduction of unnecessary computations
   - Parallelization of a great portion of the program
   - Use of datatypes

### CUDA

CUDA is a platform designed jointly at software and hardware levels to make use of the GPU computational power in general-purpose applications at three levels:

  - __Software__: It allows to program the GPU with minimal but powerful SIMD extensions to enable heterogeneous programming and attain an efficient and scalable execution.
  - __Firmware__ : It offers a driver oriented to GPGPU programming, which is compatible with that used for rendering. Straightforward APIs to manage devices, memory, etc.
  - __Hardware__: It exposes GPU parallelism for general-purpose computing via a number of multiprocessors endowed with cores and a memory hierarchy.

 In this project, we will use the GPUs provided in ARGO in order to measure our timings for our CUDA implementation. Before the metrics though, we are going to briefly explain our code for CUDA, which can be found in cuda/gol.cu.

 ## Benchmarking

 The most important aspect when using parallel computing, is the time benefits that we get. There are a few metrics that we are taught, that can show whether or not the application of parallelism was successful. Those metrics are:

- __Parallel Run Time__: he time that elapses from the moment that a parallel computation starts to the moment that the last processor finishes execution.
- __Speedup__: The ratio of the serial runtime of the best sequential algorithm for solving a problem to the time taken by the parallel algorithm to solve the same problem on p processors.
- __Efficiency__: The ratio of speedup to the number of processors. Efficiency measures the fraction of time for which a processor is usefully utilized.

Those metrics for the MPI code without the `allreduce` parameter are plotted as following:


![alt text](plots/MPI%20W_O%20All_Reduce%20Parallel%20TIme%20-%20Small%20Grids.png)


![alt text](plots/MPI%20W_O%20All_Reduce%20Parallel%20TIme%20-%20Big%20Grids.png)

![alt text](plots/MPI%20W_O%20All_Reduce%20Efficiency.png)

![alt text](plots/MPI%20W_O%20All_Reduce%20Speedup.png)

Regarding our CUDA implementation, during our runs using 1 and 2 GPUs, those are the results for the parallel run time:

![alt text](plots/CUDA%20Parallel%20Time.png)

__The full report is available [here](misc/Parallel_Systems_Project.pdf)__


## Compilation and running

- __Compile__ just by typing `make`
- __Run__ by passsing the correct parameters to the executable produced by compilation


## Project implemented by

[Nikos Galanis](https://github.com/nikosgalanis) \
[Pantelis Papageorgiou](https://github.com/PanPapag/)