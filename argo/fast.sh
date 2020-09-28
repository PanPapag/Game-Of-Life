#!/bin/bash

cd ..
make
cd argo
qsub run.sh
