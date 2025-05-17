#!/bin/bash

for np in 32; do
    echo "Running with $np processes..."
    python3 profile.py --csv parallel_execution_timesITER100.csv mpirun -np $np -hostfile ./../../../build/hostfile  ./../../../build/fractal_mpi  -i 100 -s 16 --output_disabled -w 1080  -h 1080 -b 32

done

for np in 32; do
    echo "Running with $np processes..."
    python3 profile.py --csv parallel_execution_timesITER500.csv mpirun -np $np -hostfile ./../../../build/hostfile  ./../../../build/fractal_mpi -i 500 -s 16 --output_disabled -w 1080  -h 1080 -b 32


done

for np in 32; do
    echo "Running with $np processes..."
    python3 profile.py --csv parallel_execution_timesITER1000.csv mpirun -np $np -hostfile ./../../../build/hostfile  ./../../../build/fractal_mpi -i 100 -s 16 --output_disabled -w 1080  -h 1080 -b 32

done





echo "Run Finalized"
