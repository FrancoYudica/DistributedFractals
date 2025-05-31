"""
Uses matplotlib to plot speedup and efficiency graphs and compares with different parameters
"""
import csv
import matplotlib.pyplot as plt
import argparse

def read_data(csv_file):
    processors, speedups, efficiencies = [], [], []
    with open(csv_file, newline='') as file:
        reader = csv.DictReader(file)
        for row in reader:
            processors.append(int(row["Processors"]))
            speedups.append(float(row["Speedup"]))
            efficiencies.append(float(row["Efficiency"]))
    return processors, speedups, efficiencies

def plot_speedup_all(all_data, output_file):
    plt.figure(figsize=(10, 7))
    
    max_proc = 0
    for label, (procs, speedups) in all_data.items():
        plt.plot(procs, speedups, marker='o', label=label)
        max_proc = max(max_proc, max(procs))
    
    # Plot ideal linear speedup
    ideal = list(range(1, max_proc + 1))
    plt.plot(ideal, ideal, 'k--', label="Ideal Linear Speedup")
    
    plt.title("Speedup vs Processors")

    # Use actual processor values from the first dataset for x-ticks
    first_procs = next(iter(all_data.values()))[0]
    plt.xticks(first_procs)
    plt.xlabel("Processors")

    plt.ylabel("Speedup")
    plt.legend()
    plt.grid(True)
    plt.savefig(output_file)
    print(f"[✓] Speedup plot saved to: {output_file}")
    plt.close()

def plot_efficiency_all(all_data, output_file):
    plt.figure(figsize=(10, 7))
    
    for label, (procs, _, efficiencies) in all_data.items():
        plt.plot(procs, efficiencies, marker='o', label=label)
    
    plt.title("Efficiency vs Processors")

    # Use actual processor values from the first dataset for x-ticks
    first_procs = next(iter(all_data.values()))[0]
    plt.xticks(first_procs)
    plt.xlabel("Processors")

    plt.ylabel("Efficiency")
    plt.ylim(0, 1.1)
    plt.legend()
    plt.grid(True)
    plt.savefig(output_file)
    print(f"[✓] Efficiency plot saved to: {output_file}")
    plt.close()

def main():
    parser = argparse.ArgumentParser(description="Plot speedup and efficiency for multiple CSV datasets")
    parser.add_argument("--csv", nargs='+', required=True, help="Input CSV files")
    parser.add_argument("--label", nargs='+', required=True, help="Labels for each CSV file")
    parser.add_argument("--speedup_plot", default="combined_speedup.png", help="Output speedup plot image")
    parser.add_argument("--efficiency_plot", default="combined_efficiency.png", help="Output efficiency plot image")
    args = parser.parse_args()

    if len(args.csv) != len(args.label):
        raise ValueError("You must provide the same number of --csv and --label arguments")

    all_data = {}
    for csv_file, label in zip(args.csv, args.label):
        processors, speedups, efficiencies = read_data(csv_file)
        all_data[label] = (processors, speedups, efficiencies)

    plot_speedup_all({k: (v[0], v[1]) for k, v in all_data.items()}, args.speedup_plot)
    plot_efficiency_all(all_data, args.efficiency_plot)

if __name__ == "__main__":
    main()
