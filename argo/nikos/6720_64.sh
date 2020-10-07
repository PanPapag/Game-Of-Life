#!/bin/bash

# Max VM size #
#PBS -l pvmem=2G

# Max Wall time, Example 1 Minute #
#PBS -l walltime=01:00:00

# How many nodes and tasks per node, Example 2 nodes with 8 tasks each => 16 tasks #
#PBS -l select=8:ncpus=8:mpiprocs=8

#PBS -l place=excl

# Which Queue to use, DO NOT CHANGE #
#PBS -q workq

# Stdout Output File, if not provided a <JobName>.o<JobID> will be created #
#PBS -o 6720_64.out

# Stderr Output File, if not provided a <JobName>.e<JobID> will be created #
#PBS -e 6720_64.err

# JobName #
#PBS -N myJob

#Change Working directory to SUBMIT directory
cd $PBS_O_WORKDIR

#OpenMP Threads #
# OMP_NUM_THREADS * ppn should be max 8 (the total number of node cores= 8).
# To use OpenMPI remember to include -fopenmp in compiler flags in order to activate OpenMP directives.
export OMP_NUM_THREADS=1

# Run executable #
mpirun ../../build/main.x -l 50 -n 6720 -i "/home/pool/argo079/Game-of-Life/misc/grids/6720x6720_grid" -r 1
