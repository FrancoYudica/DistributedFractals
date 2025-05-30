#!/bin/bash

# Check arguments
if [ "$#" -lt 4 ]; then
    echo "Usage: $0 <COMMON_PARAMS> <PARALLEL_BIN> <SEQUENTIAL_BIN> <HOSTFILE>"
    echo "Example:"
    echo "  $0 \"-i 512 -s 16 --output_disabled\" ./../../../build/fractal_mpi ./../../../build/sequential ./../../../build/hostfile"
    exit 1
fi

COMMON_PARAMS="$1"
PARALLEL_BIN="$2"
SEQUENTIAL_BIN="$3"
HOSTFILE="$4"

NP_VALUES=(2 4 8 16 32)
BLOCK_SIZES=(2 4 8 16 32 64 128)

WIDTH=1920
HEIGHT=1080

# Parallel runs by block size
for block in "${BLOCK_SIZES[@]}"; do
    for np in "${NP_VALUES[@]}"; do
        echo "Parallel: Running with $np processes and block size $block..."
        python3 ../profile.py --csv "parallel_execution_timesb${block}.csv" \
            mpirun -np "$np" -hostfile "$HOSTFILE" "$PARALLEL_BIN" \
            $COMMON_PARAMS -w "$WIDTH" -h "$HEIGHT" -b "$block"
    done
done

# Single sequential run
echo "Sequential: Running once..."
python3 ../profile.py --csv "sequential_execution_timesb2.csv" \
    "$SEQUENTIAL_BIN" $COMMON_PARAMS -w "$WIDTH" -h "$HEIGHT"

echo "Run Finalized"
