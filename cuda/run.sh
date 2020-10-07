#!/bin/bash

# Which Queue to use, DO NOT CHANGE #
#PBS -q GPUq

# Max Wall time, Example 1 Minute #
#PBS -l walltime=00:01:00

# How many nodes and tasks per node, Example 2 nodes with 8 tasks each => 16 tasts #
#PBS -l select=1:ncpus=1:ngpus=1


# JobName #
#PBS -N myGPUJob

#Change Working directory to SUBMIT directory
cd $PBS_O_WORKDIR

# Run executable #
./gol -l 15 -n 840 -i "/home/pool/argo081/Game-Of-Life/misc/grids/840x840_grid" -o "/home/pool/argo081/Game-Of-Life/misc/outputs/small"

# profile executable #
#nvprof ./gol
