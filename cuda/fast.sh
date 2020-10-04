#!/bin/bash

rm gol
rm myGPUJob.*
nvcc gol.cu -o gol
qsub run.sh
