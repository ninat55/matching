CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
PYTHON = venv/bin/python

all: stablematching

# Python preprocessing
data/clean_p.csv: data/Untitledform.csv src/preprocessing.py
	$(PYTHON) src/preprocessing.py data/Untitledform.csv data/clean_p.csv data/capacities.csv

# data/clean_r.csv: data/Untitledform.csv src/preprocessing.py
# 	$(PYTHON) src/preprocessing.py data/Untitledform.csv data/clean_r.csv

stablematching: src/algorithm.cpp data/clean_p.csv
	$(CXX) $(CXXFLAGS) -o stablematching src/algorithm.cpp

clean:
	rm -f stablematching data/clean_p.csv

.PHONY: all clean