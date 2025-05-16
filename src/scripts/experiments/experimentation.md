
## Reproducing the experiments

1. Pick a fixed set of settings `--output_disabled -i 512 -w 1080 -h 1080 -s 16`
2. Profile the results of both implementations:
    - Parallel: Execute this command with varying amount of nodes
    ```bash
    for np in {2..8}; do
        echo "Running with $np processes..."
        python3 profile.py --csv parallel_execution_times.csv mpirun -np $np <program and args>
    done
    ```

    - Sequential
    ```bash
    python3 profile.py --csv sequential_execution_times.csv <program and args>
    ```

3. Generate summaries of data

    ```bash
    python3 profile.py --analyze --csv parallel_execution_times.csv --out summary_parallel.csv
    ```

    ```bash
    python3 profile.py --analyze --csv sequential_execution_times.csv --out summary_sequential.csv
    ```

4. Read average sequential time from `summary_sequential.csv`
5. Analyze the speedup and efficiency of the parallel version with:
    ```bash
    python analyze_speedup.py --seq_time <seq_time> --csv summary_parallel.csv --out speedup_summary.csv
    ```

6. Plot results with matplotlib

    Plotting speedup and efficiency
    ```bash
    python3 plot_statistics.py --csv speedup_summary.csv           
    ```

    Box plots and parallel vs sequential graph
    ```bash
    python3 plot_parallel_times.py --seq_time <seq_time> --csv summary_parallel.csv
    ```