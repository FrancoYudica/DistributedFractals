#!/bin/bash

for np in 2 4 8 16 32; do
    echo "Running with $np processes..."
    python3 profile.py --csv parallel_execution_timesIMG128.csv mpirun -np $np -hostfile ./../../../build/hostfile  ./../../../build/fractal_mpi  -i 512 -s 16 --output_disabled -w 128  -h 128 -b 32

done

for np in 2 4 8 16 32; do
    echo "Running with $np processes..."
    python3 profile.py --csv parallel_execution_timesIMG512.csv mpirun -np $np -hostfile ./../../../build/hostfile  ./../../../build/fractal_mpi -i 512 -s 16 --output_disabled -w 512  -h 512 -b 32


done

for np in 2 4 8 16 32; do
    echo "Running with $np processes..."
    python3 profile.py --csv parallel_execution_timesIMG1080.csv mpirun -np $np -hostfile ./../../../build/hostfile  ./../../../build/fractal_mpi -i 512 -s 16 --output_disabled -w 1080  -h 1080 -b 32

done
for np in 2 4 8 16 32; do
    echo "Running with $np processes..."
    python3 profile.py --csv parallel_execution_timesIMG1920.csv mpirun -np $np -hostfile ./../../../build/hostfile  ./../../../build/fractal_mpi -i 512 -s 16 --output_disabled -w  1920  -h 1920 -b 32

done

echo "Sequential IMG 128"
python3 profile.py --csv sequential_execution_timesIMG128.csv  ./../../../build/sequential -i 512 -s 16 --output_disabled -w 128 -h 128 -b 32

echo "Sequential IMG 512"
python3 profile.py --csv sequential_execution_timesIMG512.csv  ./../../../build/sequential -i 512 -s 16 --output_disabled -w 512 -h 512 -b 32

echo "Sequential IMG 1080"
python3 profile.py --csv sequential_execution_timesIMG1080.csv  ./../../../build/sequential -i 512 -s 16 --output_disabled -w 1080 -h 1080 -b 32

echo "Sequential IMG 1920"
python3 profile.py --csv sequential_execution_timesIMG1920.csv  ./../../../build/sequential -i 512 -s 16 --output_disabled -w 1920 -h 1920 -b 32


echo "Run Finalized"
