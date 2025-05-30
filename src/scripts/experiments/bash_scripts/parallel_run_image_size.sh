#!/bin/bash

# Usage help
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
BLOCK_SIZE=32

IMAGE_SIZES=(32 64 128 512 1080 1920)
NP_VALUES=(2 4 8 16 32)

# Parallel runs
for size in "${IMAGE_SIZES[@]}"; do
    for np in "${NP_VALUES[@]}"; do
        echo "Parallel: Running with $np processes for IMG $size..."
        python3 ../profile.py --csv "parallel_execution_times_image_size{size}.csv" \
            mpirun -np "$np" -hostfile "$HOSTFILE" "$PARALLEL_BIN" \
            $COMMON_PARAMS -w "$size" -h "$size" -b "$BLOCK_SIZE"
    done
done

# Sequential runs
for size in "${IMAGE_SIZES[@]}"; do
    echo "Sequential: Running for IMG $size..."
    python3 ../profile.py --csv "sequential_execution_times_image_size{size}.csv" \
        "$SEQUENTIAL_BIN" $COMMON_PARAMS -w "$size" -h "$size" -b "$BLOCK_SIZE"
done

echo "Run Finalized"
