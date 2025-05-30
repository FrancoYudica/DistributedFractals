#!/bin/bash

# Check arguments
if [ "$#" -lt 3 ]; then
    echo "Usage: $0 <COMMON_PARAMS> <PARALLEL_BIN> <HOSTFILE>"
    echo "Example:"
    echo "  $0 \"-s 16 --output_disabled -w 1080 -h 1080 -b 32\" ./../../../build/fractal_mpi ./../../../build/hostfile"
    exit 1
fi

COMMON_PARAMS="$1"
PARALLEL_BIN="$2"
HOSTFILE="$3"

NP=32  # Only one value used for this run
ITERATIONS=(5000 10000 15000 20000)

# Parallel runs by iteration count
for it in "${ITERATIONS[@]}"; do
    echo "Parallel: Running with $NP processes and $it iterations..."
    python3 ../profile.py --csv "parallel_execution_times_iterations${it}.csv" \
        mpirun -np "$NP" -hostfile "$HOSTFILE" "$PARALLEL_BIN" \
        -i "$it" $COMMON_PARAMS
done

echo "Run Finalized"
