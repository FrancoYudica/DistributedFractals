"""
Uses matplotlib to plot speedup and efficiency graphs
"""
import csv
import matplotlib.pyplot as plt
import argparse

def read_data(csv_file):
    processors = []
    speedups = []
    efficiencies = []

    with open(csv_file, newline='') as file:
        reader = csv.DictReader(file)
        for row in reader:
            processors.append(int(row["Processors"]))
            speedups.append(float(row["Speedup"]))
            efficiencies.append(float(row["Efficiency"]))

    return processors, speedups, efficiencies

def plot_speedup(processors, speedups, output_file):
    plt.figure(figsize=(8, 6))
    plt.plot(processors, speedups, marker='o', linestyle='-', color='blue', label='Measured Speedup')
    plt.plot(processors, processors, linestyle='--', color='gray', label='Ideal Speedup (Linear)')
    plt.title("Speedup vs Processors")
    plt.xlabel("Processors")
    plt.ylabel("Speedup")
    plt.legend()
    plt.grid(True)
    plt.savefig(output_file)
    print(f"Speedup plot saved to {output_file}")
    plt.close()

def plot_efficiency(processors, efficiencies, output_file):
    plt.figure(figsize=(8, 6))
    plt.plot(processors, efficiencies, marker='o', linestyle='-', color='green')
    plt.title("Efficiency vs Processors")
    plt.xlabel("Processors")
    plt.ylabel("Efficiency")
    plt.ylim(0, 1.1)
    plt.grid(True)
    plt.savefig(output_file)
    print(f"Efficiency plot saved to {output_file}")
    plt.close()

def main():
    parser = argparse.ArgumentParser(description="Plot speedup and efficiency from CSV data")
    parser.add_argument("--csv", type=str, default="speedup_summary.csv", help="Input CSV file")
    parser.add_argument("--speedup_plot", type=str, default="speedup_plot.png", help="Output file for speedup plot")
    parser.add_argument("--efficiency_plot", type=str, default="efficiency_plot.png", help="Output file for efficiency plot")
    args = parser.parse_args()

    processors, speedups, efficiencies = read_data(args.csv)
    plot_speedup(processors, speedups, args.speedup_plot)
    plot_efficiency(processors, efficiencies, args.efficiency_plot)

if __name__ == "__main__":
    main()
