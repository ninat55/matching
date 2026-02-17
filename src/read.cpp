#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <vector>

using std::string, std::ifstream, std::map, std::vector, std::cout, std::cerr, std::endl, std::stringstream;

namespace matching {

// input - proposers' & receivers' preferences
// output - proposer_names dictionary, 2d vector proposer_rankings
int read(string proposer_prefs, string receiver_prefs) {
    ifstream proposer_stream(proposer_prefs);
    ifstream receiver_stream(receiver_prefs);

    map<int, string> proposer_names, receiver_names;
    vector<vector<int>> proposer_rankings, receiver_rankings;

    int proposer_idx = 0, receiver_idx = 0;

    string next_line, next_word, name;

    // Start with proposers!

    if (!proposer_stream) {
        cerr << "Unable to read file " << proposer_prefs << endl;
    }

    // Read header
    getline(proposer_stream, next_line);
    stringstream ss(next_line);

    while (getline(ss, next_word, ',')) {
        // Add receiver to map
        receiver_names[receiver_idx] = name;
        receiver_idx++;
    }

    // Now, iterate through the table!
    while (getline(proposer_stream, next_line)) {
        ss.clear(); // reset flags
        ss.str(""); // empty internal buffer
    
        getline(ss, next_word, ',');
    }
}

}