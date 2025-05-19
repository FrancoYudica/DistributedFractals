
## Reproducing the experiments

1. Pick a fixed set of settings `--output_disabled -i 512 -w 1080 -h 1080 -s 16`
2. Profile the results of both implementations:
    - Sequential
    ```bash
    python3 profile.py --csv execution_times.csv <program and args>
    ```
    This will recollect all the required data

3. Generate summaries of data

    ```bash
    python3 profile.py --analyze --csv execution_times.csv --out summary.csv
    ```

4. Read average sequential time from `summary_sequential.csv`
5. Analyze the speedup and efficiency of the parallel version with:
    ```bash
    python analyze_speedup.py --seq_time <seq_time> --csv summary_parallel.csv --out speedup_summary.csv
    ```

Now, all that remains is plotting. It's achieved with the scripts located in `src/scripts/experiments/plotting`. Note that these use mathplotlib, so setting a python environment might be necessary.