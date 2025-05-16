"""
Given the average sequential time and the average parallel time with different amount of
processors, this script computes the speedup and efficiency of each combination
"""
import csv
import argparse
import re

def extract_processors(command_str):
    match = re.search(r"-np\s+(\d+)", command_str)
    return int(match.group(1)) if match else None

def compute_speedup_efficiency(summary_csv, sequential_time, output_csv):
    rows = []
    with open(summary_csv, newline='') as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            cmd = row["Command"]
            avg_time = float(row["Average Time (s)"])
            p = extract_processors(cmd)
            if p is None:
                print(f"Skipping: could not extract processor count from command: {cmd}")
                continue
            speedup = sequential_time / avg_time
            efficiency = speedup / p
            rows.append({
                "Processors": p,
                "Command": cmd,
                "Parallel Time (s)": avg_time,
                "Speedup": speedup,
                "Efficiency": efficiency
            })

    rows.sort(key=lambda r: r["Processors"])  # sort results for readability

    with open(output_csv, 'w', newline='') as out:
        writer = csv.DictWriter(out, fieldnames=rows[0].keys())
        writer.writeheader()
        writer.writerows(rows)

    print(f"Results saved to: {output_csv}")

def main():
    parser = argparse.ArgumentParser(description="Compute speedup and efficiency from parallel summary.csv")
    parser.add_argument("--seq_time", type=float, required=True, help="Average sequential execution time")
    parser.add_argument("--csv", type=str, default="summary.csv", help="Input summary CSV file")
    parser.add_argument("--out", type=str, default="speedup_summary.csv", help="Output CSV for results")
    args = parser.parse_args()

    compute_speedup_efficiency(args.csv, args.seq_time, args.out)

if __name__ == "__main__":
    main()
