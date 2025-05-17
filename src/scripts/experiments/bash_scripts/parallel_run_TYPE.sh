#!/bin/bash

for np in  32; do
    echo "Running with $np processes..."
    python3 profile.py --csv parallel_execution_timesTYPE0.csv mpirun -np $np -hostfile ./../../../build/hostfile  ./../../../build/fractal_mpi  -i 512 -s 16 --output_disabled -w 1080  -h 1080 -b 32 -t 0

done

for np in  32; do
    echo "Running with $np processes..."
    python3 profile.py --csv parallel_execution_timesTYPE1.csv mpirun -np $np -hostfile ./../../../build/hostfile  ./../../../build/fractal_mpi -i 512 -s 16 --output_disabled -w 1080  -h 1080 -b 32 -t 1

done


echo "Run Finalized"
