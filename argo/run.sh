#!/bin/bash

# Max VM size #
#PBS -l pvmem=2G

# Max Wall time, Example 1 Minute #
#PBS -l walltime=01:00:00

# How many nodes and tasks per node, Example 2 nodes with 8 tasks each => 16 tasks #
#PBS -l select=1:ncpus=8:mpiprocs=1

#PBS -l place=excl

# Which Queue to use, DO NOT CHANGE #
#PBS -q workq

# Stdout Output File, if not provided a <JobName>.o<JobID> will be created #
#PBS -o MPI+AllReduce_1_840x840.out

# Stderr Output File, if not provided a <JobName>.e<JobID> will be created #
#PBS -e MPI+AllReduce_1_840x840.err

# JobName #
#PBS -N MPI+AR_1_840x840

#Change Working directory to SUBMIT directory
cd $PBS_O_WORKDIR

#OpenMP Threads #
# OMP_NUM_THREADS * ppn should be max 8 (the total number of node cores= 8).
# To use OpenMPI remember to include -fopenmp in compiler flags in order to activate OpenMP directives.
export OMP_NUM_THREADS=1

# Run executable #
<<<<<<< HEAD
mpirun ../build/main.x -l 50 -n 840 -i "/home/pool/argo081/Game-Of-Life/misc/grids/840x840_grid" -r 1
=======
mpirun ../build/main.x -l 50 -n 840 -i "/home/pool/argo079/Game-of-Life/misc/grids/840x840_grid"
>>>>>>> dd7bc03a82f1704307880f4f5f0b23689c8bb9bb
