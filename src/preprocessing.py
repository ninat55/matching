import sys
import csv
from number_parser import parse_ordinal

def preprocessing(input_path, output_path, capacity_path):
    with open(input_path, "r") as in_file, \
         open(output_path, "w") as out_file, \
         open(capacity_path, "w") as capacity_file:
        
        reader = csv.reader(in_file)
        writer = csv.writer(out_file)
        c_writer = csv.writer(capacity_file)

        header = next(reader)

        try:
            start_idx = next(i
                for i, col in enumerate(header)
                if col.strip().lower() == "name")
        except StopIteration:
            raise ValueError("Name column not found in CSV header")
        
        capacity_idx = None
        new_header = ["name"]
        
        for i, col in enumerate(header[start_idx + 1:]):
            col_name = extract_name(col)
            if col_name == "capacity":
                capacity_idx = i
                c_writer.writerow(["name", "capacity"])
            else:
                new_header.append(col_name)
        writer.writerow(new_header)
        
        for row in reader:
            new_row = [row[start_idx]]
            for i, col in enumerate(row[start_idx + 1:]):
                if i == capacity_idx:
                    c_writer.writerow([row[start_idx], True if col == "yes" else False])
                else:
                    new_row.append(parse_ordinal(col))
            writer.writerow(new_row)

def extract_name(col):
    col = col.strip()

    if "[" not in col or "]" not in col:
        # we assume this must be 'capacity' column
        return "capacity"

    left = col.index("[")
    right = col.index("]", left)

    name = col[left + 1 : right].strip()
    if not name:
        raise ValueError(f"Empty name in brackets: '{col}'")

    return name

if __name__ == "__main__":
    preprocessing(sys.argv[1], sys.argv[2], sys.argv[3])