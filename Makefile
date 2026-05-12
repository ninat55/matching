CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
PYTHON = venv/bin/python

LITTLES_CSV  = data/round_x_littles.csv
BIGS_CSV     = data/round_x_bigs.csv

all: stablematching

$(LITTLES_CSV): $(LITTLES_FORM) src/preprocessing.py
	$(PYTHON) src/preprocessing.py "$(LITTLES_FORM)" $(LITTLES_CSV)

$(BIGS_CSV): $(BIGS_FORM) src/preprocessing.py
	$(PYTHON) src/preprocessing.py "$(BIGS_FORM)" $(BIGS_CSV)

stablematching: src/algorithm.cpp src/read.cpp src/matrix-utils.cpp
	$(CXX) $(CXXFLAGS) -o stablematching src/algorithm.cpp

preprocess: $(LITTLES_CSV) $(BIGS_CSV)

run: stablematching
	./stablematching $(LITTLES_CSV) $(BIGS_CSV)

clean:
	rm -f stablematching $(LITTLES_CSV) $(BIGS_CSV)

.PHONY: all run clean
