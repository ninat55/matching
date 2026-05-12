#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <vector>

using std::string, std::ifstream, std::map, std::vector, std::cerr, std::endl, std::stringstream;

namespace matching {

static vector<vector<string>> read_csv(const string& path) {
    ifstream f(path);
    if (!f) {
        cerr << "Unable to read file " << path << endl;
        return {};
    }
    vector<vector<string>> rows;
    string line;
    while (getline(f, line)) {
        vector<string> row;
        stringstream ss(line);
        string cell;
        while (getline(ss, cell, ',')) {
            row.push_back(cell);
        }
        if (!row.empty()) rows.push_back(row);
    }
    return rows;
}

// Reads two CSVs (littles and bigs) produced by preprocessing.py.
// Each row: name, choice1, choice2, ...
// Populates name maps and preference vectors (ordered indices, most-preferred first).
void read(const string& proposer_prefs_path, const string& receiver_prefs_path,
          map<int, string>& proposer_names, map<int, string>& receiver_names,
          vector<vector<int>>& proposer_preference, vector<vector<int>>& receiver_preference) {

    auto proposer_rows = read_csv(proposer_prefs_path);
    auto receiver_rows = read_csv(receiver_prefs_path);

    map<string, int> proposer_idx, receiver_idx;

    for (int i = 0; i < (int)proposer_rows.size(); i++) {
        if (proposer_rows[i].empty()) continue;
        proposer_names[i] = proposer_rows[i][0];
        proposer_idx[proposer_rows[i][0]] = i;
    }
    for (int i = 0; i < (int)receiver_rows.size(); i++) {
        if (receiver_rows[i].empty()) continue;
        receiver_names[i] = receiver_rows[i][0];
        receiver_idx[receiver_rows[i][0]] = i;
    }

    for (const auto& row : proposer_rows) {
        vector<int> prefs;
        for (int j = 1; j < (int)row.size(); j++) {
            if (receiver_idx.count(row[j])) {
                prefs.push_back(receiver_idx.at(row[j]));
            } else {
                cerr << "Warning: unknown big '" << row[j] << "' in little '" << row[0] << "'" << endl;
            }
        }
        proposer_preference.push_back(prefs);
    }

    for (const auto& row : receiver_rows) {
        vector<int> prefs;
        for (int j = 1; j < (int)row.size(); j++) {
            if (proposer_idx.count(row[j])) {
                prefs.push_back(proposer_idx.at(row[j]));
            } else {
                cerr << "Warning: unknown little '" << row[j] << "' in big '" << row[0] << "'" << endl;
            }
        }
        receiver_preference.push_back(prefs);
    }
}

// Reads one or more prior-round result CSVs (each row: big_name, little1_name, little2_name, ...).
// Accumulates into a map of receiver_idx -> list of proposer_idx matched across all rounds.
map<int, vector<int>> read_prior_matches(const vector<string>& result_csvs,
                                         const map<int, string>& proposer_names,
                                         const map<int, string>& receiver_names) {
    map<string, int> proposer_idx, receiver_idx;
    for (const auto& [i, name] : proposer_names) proposer_idx[name] = i;
    for (const auto& [i, name] : receiver_names) receiver_idx[name] = i;

    map<int, vector<int>> prior_matches;

    for (const auto& path : result_csvs) {
        for (const auto& row : read_csv(path)) {
            if (row.empty()) continue;
            if (!receiver_idx.count(row[0])) {
                cerr << "Warning: unknown big '" << row[0] << "' in prior matches file " << path << endl;
                continue;
            }
            int r = receiver_idx.at(row[0]);
            for (int j = 1; j < (int)row.size(); j++) {
                if (proposer_idx.count(row[j])) {
                    prior_matches[r].push_back(proposer_idx.at(row[j]));
                } else {
                    cerr << "Warning: unknown little '" << row[j] << "' in prior matches file " << path << endl;
                }
            }
        }
    }

    return prior_matches;
}

}
