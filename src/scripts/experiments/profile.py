"""
This script executes the given program multiple times and serializes output data such as time into CSV format
Also, this can --analyze the output and provide a summary, showing the command, runs, avg time and std dev
"""
import subprocess
import sys
import time
import csv
import os
import statistics
import argparse

def measure_and_record(command, runs, csv_file):
    command_str = ' '.join(command)
    with open(csv_file, 'a', newline='') as csvfile:
        writer = csv.writer(csvfile)
        for i in range(runs):
            start = time.perf_counter()
            try:
                subprocess.run(command, check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            except subprocess.CalledProcessError as e:
                print(f"Run {i+1} failed with return code {e.returncode}")
                continue
            end = time.perf_counter()
            elapsed = end - start
            writer.writerow([command_str, elapsed])
            print(f"Run {i+1}: {elapsed:.6f} seconds")

def analyze_csv(input_csv, output_csv):
    if not os.path.exists(input_csv):
        print(f"CSV file '{input_csv}' not found.")
        return

    results = {}
    with open(input_csv, newline='') as csvfile:
        reader = csv.reader(csvfile)
        for row in reader:
            if len(row) != 2:
                continue
            command_str, time_str = row
            try:
                elapsed = float(time_str)
                results.setdefault(command_str, []).append(elapsed)
            except ValueError:
                continue

    if not results:
        print("No valid data found in CSV.")
        return

    with open(output_csv, 'w', newline='') as outcsv:
        writer = csv.writer(outcsv)
        writer.writerow(["Command", "Runs", "Average Time (s)", "Std Dev (s)"])

        for cmd, times in results.items():
            avg = statistics.mean(times)
            stddev = statistics.stdev(times) if len(times) > 1 else 0.0
            writer.writerow([cmd, len(times), avg, stddev])
            print(f"\nCommand: {cmd}")
            print(f"  Runs: {len(times)}")
            print(f"  Average time: {avg:.6f} seconds")
            print(f"  Std deviation: {stddev:.6f} seconds")

    print(f"\nSummary written to: {output_csv}")

def main():
    parser = argparse.ArgumentParser(description="Measure execution time of a command.")
    parser.add_argument("command", nargs=argparse.REMAINDER, help="Command and its arguments to execute")
    parser.add_argument("--analyze", action="store_true", help="Analyze CSV file instead of running the command")
    parser.add_argument("--runs", type=int, default=10, help="Number of times to run the command (default: 10)")
    parser.add_argument("--csv", type=str, default="execution_times.csv", help="CSV file to read/write execution times")
    parser.add_argument("--out", type=str, default="summary.csv", help="Output CSV for analysis summary (only with --analyze)")

    args = parser.parse_args()

    if args.analyze:
        analyze_csv(args.csv, args.out)
    elif args.command:
        measure_and_record(args.command, args.runs, args.csv)
    else:
        parser.print_help()

if __name__ == "__main__":
    main()
