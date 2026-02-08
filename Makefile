CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
PYTHON = venv/bin/python

all: stablematching

# Python preprocessing
data/clean.csv: data/Untitledform.csv src/preprocessing.py
	$(PYTHON) src/preprocessing.py data/Untitledform.csv data/clean.csv

# C++ build
stablematching: src/stablematching.cpp data/clean.csv
	$(CXX) $(CXXFLAGS) -o src/stablematching src/stablematching.cpp

clean:	make --debug=b
	rm -f src/stablematching data/clean.csv

.PHONY: all clean