import sys
import csv
from number_parser import parse_ordinal

def preprocessing(input_path, output_path):
    with open(input_path, "r") as in_file, \
         open(output_path, "w") as out_file:
        
        reader = csv.reader(in_file)
        writer = csv.writer(out_file)

        header = next(reader)

        try:
            start_idx = start_idx = next(
                i for i, col in enumerate(header)
                if col.strip().lower() == "name".lower())
        except ValueError:
            raise ValueError("Name column not found in CSV header")
        
        writer.writerow(["name"] + [extract_name(col) for col in header[start_idx + 1:]])

        for row in reader:
            writer.writerow([row[start_idx]] + [parse_ordinal(col) for col in row[start_idx + 1:]])

def extract_name(col):
    col = col.strip()

    if "[" not in col or "]" not in col:
        raise ValueError(f"Invalid column format (missing brackets): '{col}'")

    left = col.index("[")
    right = col.index("]", left)

    name = col[left + 1 : right].strip()
    if not name:
        raise ValueError(f"Empty name in brackets: '{col}'")

    return name

# def ordinal_to_int(col):
#     col = col.strip().lower()
#     if not col:
#         return ""

#     cardinal = i.number_to_words(i.ordinal(i.word_to_num(col)))

#     return i.word_to_num(cardinal)

if __name__ == "__main__":
    preprocessing(sys.argv[1], sys.argv[2])