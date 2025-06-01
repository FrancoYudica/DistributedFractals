import csv
import re
import matplotlib.pyplot as plt
import sys

def extract_iterations(command):
    match = re.search(r'-i\s*(\d+)', command)
    if match:
        return int(match.group(1))
    else:
        return None

def read_data(filename):
    iterations = []
    avg_times = []
    std_devs = []
    with open(filename, 'r') as f:
        reader = csv.DictReader(f)
        for row in reader:
            i = extract_iterations(row['Command'])
            if i is not None:
                iterations.append(i)
                avg_times.append(float(row['Average Time (s)']))
                std_devs.append(float(row['Std Dev (s)']))
    return iterations, avg_times, std_devs

def plot_graph(iterations, avg_times, std_devs, output_file):
    data = sorted(zip(iterations, avg_times, std_devs), key=lambda x: x[0])
    iterations, avg_times, std_devs = zip(*data)

    plt.errorbar(iterations, avg_times, yerr=std_devs, fmt='-o', capsize=5)
    plt.xlabel('Number of Iterations (-i)')
    plt.ylabel('Average Time (s)')
    plt.title('Average Time vs Number of Iterations')
    plt.grid(True)

    plt.savefig(output_file)
    print(f"Plot saved as {output_file}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python plot_times.py data.csv output.png")
        sys.exit(1)

    filename = sys.argv[1]
    output_file = sys.argv[2]
    iterations, avg_times, std_devs = read_data(filename)
    plot_graph(iterations, avg_times, std_devs, output_file)
