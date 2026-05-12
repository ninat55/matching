import sys
import csv

def preprocessing(input_path, output_path):
    with open(input_path, newline="", encoding="utf-8-sig") as in_file, \
         open(output_path, "w", newline="") as out_file:

        reader = csv.reader(in_file)
        writer = csv.writer(out_file)
        header = next(reader)

        # Find the name column
        name_idx = next(
            (i for i, col in enumerate(header) if "name" in col.strip().lower()),
            None
        )
        if name_idx is None:
            raise ValueError("Name column not found in CSV header")

        # Find choice columns: anything with "choice" in the header after the name column
        choice_indices = [
            i for i, col in enumerate(header)
            if i > name_idx and "choice" in col.strip().lower()
        ]
        if not choice_indices:
            raise ValueError("No choice columns found in CSV header")

        for row in reader:
            name = row[name_idx].strip().lower()
            if not name:
                continue
            choices = [row[i].strip().lower() for i in choice_indices if i < len(row) and row[i].strip()]
            writer.writerow([name] + choices)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <input_csv> <output_csv>")
        sys.exit(1)
    preprocessing(sys.argv[1], sys.argv[2])
