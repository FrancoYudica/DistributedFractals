"""
Uses matplotlib to plot a boxplot of times and a graph that compares
the sequential and parallel times.
"""
import csv
import argparse
import matplotlib.pyplot as plt
import numpy as np

def parse_data(csv_file):
    commands = []
    procs = []
    avg_times = []
    stddevs = []

    with open(csv_file, newline='') as f:
        reader = csv.DictReader(f)
        for row in reader:
            command = row['Command']
            commands.append(command)
            nparts = [int(part) for part in command.split() if part.isdigit()]
            nproc = nparts[0] if nparts else -1
            procs.append(nproc)
            avg_times.append(float(row['Average Time (s)']))
            stddevs.append(float(row['Std Dev (s)']))
    
    return procs, avg_times, stddevs

def plot_boxplot(procs, avg_times, stddevs, out_path):
    plt.figure(figsize=(10, 6))
    data = [np.random.normal(loc=avg, scale=std, size=10) for avg, std in zip(avg_times, stddevs)]
    plt.boxplot(data, labels=procs)
    plt.title("Boxplot of Execution Times per Processor Count")
    plt.xlabel("Processors")
    plt.ylabel("Execution Time (s)")
    plt.grid(True)
    plt.savefig(out_path)
    print(f"[✓] Saved boxplot to: {out_path}")
    plt.close()

def plot_comparison(procs, avg_times, seq_time, out_path):
    plt.figure(figsize=(10, 6))
    plt.plot(procs, avg_times, marker='o', label='Parallel Time')
    plt.hlines(seq_time, xmin=min(procs), xmax=max(procs), colors='r', linestyles='--', label=f'Sequential Time ({seq_time:.2f}s)')
    plt.title("Parallel vs Sequential Execution Times")
    plt.xlabel("Processors")
    plt.ylabel("Execution Time (s)")
    plt.legend()
    plt.grid(True)
    plt.savefig(out_path)
    print(f"[✓] Saved comparison plot to: {out_path}")
    plt.close()

def main():
    parser = argparse.ArgumentParser(description="Analyze parallel execution times and plot boxplot and comparison")
    parser.add_argument("--csv", required=True, help="Input CSV summary of parallel runs")
    parser.add_argument("--seq_time", type=float, required=True, help="Average sequential execution time (s)")
    parser.add_argument("--boxplot", default="boxplot_times.png", help="Output path for boxplot image")
    parser.add_argument("--compare", default="compare_with_seq.png", help="Output path for comparison image")
    args = parser.parse_args()

    procs, avg_times, stddevs = parse_data(args.csv)
    sorted_data = sorted(zip(procs, avg_times, stddevs))
    procs, avg_times, stddevs = zip(*sorted_data)

    plot_boxplot(procs, avg_times, stddevs, args.boxplot)
    plot_comparison(procs, avg_times, args.seq_time, args.compare)

if __name__ == "__main__":
    main()
