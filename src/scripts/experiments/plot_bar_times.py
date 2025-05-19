import csv
import argparse
import matplotlib.pyplot as plt

def read_average_time(csv_file):
    with open(csv_file, newline='') as f:
        reader = csv.DictReader(f)
        for row in reader:
            return float(row["Average Time (s)"])
    raise ValueError(f"No data found in {csv_file}")

def plot_bar_chart(labels, times, output_file):
    plt.figure(figsize=(10, 6))
    plt.bar(labels, times, color='skyblue')
    plt.title("Execution Time Comparison")
    plt.ylabel("Execution Time (s)")
    plt.grid(axis='y')
    plt.tight_layout()
    plt.savefig(output_file)
    print(f"[✓] Bar chart saved to: {output_file}")
    plt.close()

def main():
    parser = argparse.ArgumentParser(description="Plot execution times from multiple CSV files into a bar chart")
    parser.add_argument("--csv", nargs='+', required=True, help="List of CSV files")
    parser.add_argument("--label", nargs='+', required=True, help="Labels for each CSV")
    parser.add_argument("--out", type=str, default="bar_chart.png", help="Output image file")
    args = parser.parse_args()

    if len(args.csv) != len(args.label):
        parser.error("Number of --csv files must match number of --label entries")

    times = [read_average_time(csv_path) for csv_path in args.csv]
    plot_bar_chart(args.label, times, args.out)

if __name__ == "__main__":
    main()
