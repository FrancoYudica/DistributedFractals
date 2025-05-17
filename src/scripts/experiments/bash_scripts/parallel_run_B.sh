#!/bin/bash

for np in 2 4 8 16 32; do
    echo "Running with $np processes..."
    python3 profile.py --csv parallel_execution_timesb32.csv mpirun -np $np -hostfile ./../../../build/hostfile  ./../../../build/fractal_mpi  -i 512 -s 16 --output_disabled -w 1920  -h 1080 -b 16

done


for np in 2 4 8 16 32; do
    echo "Running with $np processes..."
    python3 profile.py --csv parallel_execution_timesb32.csv mpirun -np $np -hostfile ./../../../build/hostfile  ./../../../build/fractal_mpi  -i 512 -s 16 --output_disabled -w 1920  -h 1080 -b 32

done

for np in 2 4 8 16 32; do
    echo "Running with $np processes..."
    python3 profile.py --csv parallel_execution_timesb64.csv mpirun -np $np -hostfile ./../../../build/hostfile  ./../../../build/fractal_mpi -i 512 -s 16 --output_disabled -w 1920  -h 1080 -b 64


done

for np in 2 4 8 16 32; do
    echo "Running with $np processes..."
    python3 profile.py --csv parallel_execution_timesb128.csv mpirun -np $np -hostfile ./../../../build/hostfile  ./../../../build/fractal_mpi -i 512 -s 16 --output_disabled -w 1920  -h 1080 -b 128

done

echo "Sequential b 16"
python3 profile.py --csv sequential_execution_timesb16.csv  ./../../../build/sequential -i 512 -s 16 --output_disabled -w 1920 -h 1080 -b 16

echo "Sequential b 32"
python3 profile.py --csv sequential_execution_timesb32.csv  ./../../../build/sequential -i 512 -s 16 --output_disabled -w 1920 -h 1080 -b 32

echo "Sequential b 64"
python3 profile.py --csv sequential_execution_timesb64.csv  ./../../../build/sequential -i 512 -s 16 --output_disabled -w 1920 -h 1080 -b 64

echo "Sequential b 128"
python3 profile.py --csv sequential_execution_timesb128.csv  ./../../../build/sequential -i 512 -s 16 --output_disabled -w 1920 -h 1080 -b 128




echo "Run Finalized"
