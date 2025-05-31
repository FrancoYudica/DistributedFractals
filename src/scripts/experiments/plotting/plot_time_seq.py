import re
import csv
import matplotlib.pyplot as plt
import argparse

def extract_processors(command_str):
    match = re.search(r"-np\s+(\d+)", command_str)
    return int(match.group(1)) if match else None

def read_time_data(csv_file):
    processors, times = [], []
    with open(csv_file, newline='') as file:
        reader = csv.DictReader(file)
        for row in reader:
            proc = extract_processors(row["Command"])
            if proc is not None:
                processors.append(proc)
                times.append(float(row["Average Time (s)"]))
    return processors, times

def plot_times_all(all_data, seq_times, output_file):
    plt.figure(figsize=(10, 7))

    all_proc_set = set()

    for i, (label, (processors, times)) in enumerate(all_data.items()):
        sorted_pairs = sorted(zip(processors, times))
        processors_sorted, times_sorted = zip(*sorted_pairs)

        all_proc_set.update(processors_sorted)

        line, = plt.plot(processors_sorted, times_sorted, marker='o', label=label)

        # Plot sequential time as horizontal line
        seq_time = seq_times[i]
        plt.axhline(y=seq_time, linestyle='--', color=line.get_color(),
                    label=f"{label} (Sequential)")

    # Set X-ticks to match all unique processor counts
    xticks_sorted = sorted(all_proc_set)
    plt.xticks(xticks_sorted)

    plt.title("Execution Time vs Processors")
    plt.xlabel("Processors")
    plt.ylabel("Average Time (s)")
    plt.legend()
    plt.grid(True)
    plt.savefig(output_file)
    print(f"[✓] Time plot saved to: {output_file}")
    plt.close()

def main():
    parser = argparse.ArgumentParser(description="Plot execution time for multiple CSV datasets")
    parser.add_argument("--csv", nargs='+', required=True, help="Input CSV files")
    parser.add_argument("--label", nargs='+', required=True, help="Labels for each CSV file")
    parser.add_argument("--seq_time", nargs='+', type=float, required=True, help="Sequential times (same order as CSVs)")
    parser.add_argument("--output", default="combined_time.png", help="Output plot image")
    args = parser.parse_args()

    if len(args.csv) != len(args.label) or len(args.csv) != len(args.seq_time):
        raise ValueError("Number of --csv, --label, and --seq_time entries must match")

    all_data = {}
    for csv_file, label in zip(args.csv, args.label):
        processors, times = read_time_data(csv_file)
        all_data[label] = (processors, times)

    plot_times_all(all_data, args.seq_time, args.output)

if __name__ == "__main__":
    main()
