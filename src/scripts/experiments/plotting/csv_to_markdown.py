import csv
import argparse
import sys

def csv_to_markdown(csv_path, ignore_columns):
    with open(csv_path, newline='') as csvfile:
        reader = csv.DictReader(csvfile)
        headers = [h for h in reader.fieldnames if h not in ignore_columns]

        # Print Markdown table header
        markdown = "| " + " | ".join(headers) + " |\n"
        markdown += "| " + " | ".join(['---'] * len(headers)) + " |\n"

        for row in reader:
            markdown += "| " + " | ".join([row[h] for h in headers]) + " |\n"

    return markdown

def main():
    parser = argparse.ArgumentParser(description="Convert CSV to Markdown table.")
    parser.add_argument("--input", "-i", required=True, help="Path to the CSV file")
    parser.add_argument("--ignore", "-x", nargs="*", default=[], help="Column headers to ignore")

    args = parser.parse_args()

    try:
        md_table = csv_to_markdown(args.input, args.ignore)
        print(md_table)
    except FileNotFoundError:
        sys.stderr.write(f"Error: File '{args.input}' not found.\n")
        sys.exit(1)
    except Exception as e:
        sys.stderr.write(f"Error: {e}\n")
        sys.exit(1)

if __name__ == "__main__":
    main()
