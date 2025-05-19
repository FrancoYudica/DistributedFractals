#!/bin/bash

for np in 2 4 8 16 32; do
    echo "Running with $np processes..."
    python3 profile.py --csv parallel_execution_timesb2.csv mpirun -np $np -hostfile ./../../../build/hostfile  ./../../../build/fractal_mpi  -i 512 -s 16 --output_disabled -w 1920  -h 1080 -b 2

done

for np in 2 4 8 16 32; do
    echo "Running with $np processes..."
    python3 profile.py --csv parallel_execution_timesb4.csv mpirun -np $np -hostfile ./../../../build/hostfile  ./../../../build/fractal_mpi  -i 512 -s 16 --output_disabled -w 1920  -h 1080 -b 4

done

for np in 2 4 8 16 32; do
    echo "Running with $np processes..."
    python3 profile.py --csv parallel_execution_timesb8.csv mpirun -np $np -hostfile ./../../../build/hostfile  ./../../../build/fractal_mpi  -i 512 -s 16 --output_disabled -w 1920  -h 1080 -b 8

done


for np in 2 4 8 16 32; do
    echo "Running with $np processes..."
    python3 profile.py --csv parallel_execution_times16.csv mpirun -np $np -hostfile ./../../../build/hostfile  ./../../../build/fractal_mpi  -i 512 -s 16 --output_disabled -w 1920  -h 1080 -b 16

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

echo "Sequentia"
python3 profile.py --csv sequential_execution_timesb2.csv  ./../../../build/sequential -i 512 -s 16 --output_disabled -w 1920 -h 1080

echo "Run Finalized"
