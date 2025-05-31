import csv
import argparse
import matplotlib.pyplot as plt
import re

def extract_processors(command_str):
    match = re.search(r"-np\s+(\d+)", command_str)
    return int(match.group(1)) if match else None

def read_time_data(csv_file):
    processors, times, stddevs = [], [], []
    with open(csv_file, newline='') as file:
        reader = csv.DictReader(file)
        for row in reader:
            proc = extract_processors(row["Command"])
            if proc is not None:
                processors.append(proc)
                times.append(float(row["Average Time (s)"]))
                stddevs.append(float(row["Std Dev (s)"]))
    return processors, times, stddevs

def plot_times_all(all_data, output_file):
    plt.figure(figsize=(10, 7))
    
    for label, (processors, times, stddevs) in all_data.items():
        # Sort by processor count for proper plotting
        sorted_data = sorted(zip(processors, times, stddevs))
        processors, times, stddevs = zip(*sorted_data)

        plt.errorbar(processors, times, yerr=stddevs, marker='o', capsize=5, label=label)

    # Force X-ticks to match all unique processor counts
    all_procs = sorted(set(p for (procs, _, _) in all_data.values() for p in procs))
    plt.xticks(all_procs)

    plt.title("Average Execution Time vs Processors")
    plt.xlabel("Processors")
    plt.ylabel("Average Time (s)")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(output_file)
    print(f"[✓] Time plot saved to: {output_file}")
    plt.close()

def main():
    parser = argparse.ArgumentParser(description="Plot execution time vs processors for multiple CSV datasets")
    parser.add_argument("--csv", nargs='+', required=True, help="Input CSV files")
    parser.add_argument("--label", nargs='+', required=True, help="Labels for each CSV file")
    parser.add_argument("--output", default="execution_time.png", help="Output plot image")
    args = parser.parse_args()

    if len(args.csv) != len(args.label):
        raise ValueError("You must provide the same number of --csv and --label arguments")

    all_data = {}
    for csv_file, label in zip(args.csv, args.label):
        processors, times, stddevs = read_time_data(csv_file)
        all_data[label] = (processors, times, stddevs)

    plot_times_all(all_data, args.output)

if __name__ == "__main__":
    main()
