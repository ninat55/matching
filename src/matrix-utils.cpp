#include <vector>
#include <algorithm>
#include <random>
#include <unordered_set>
#include <iostream>
#include <map>

using std::vector, std::remove, std::cerr, std::endl, std::map;

// proposer_preference[p][r] gives proposer p's r'th preference
//      will be NUM_PROPOSERS X NUM_CHOICES
// receiver_ranking[r][p] gives receiver r's ranking for proposer p
//      will be NUM_RECEIVERS X NUM_PROPOSERS

namespace matching {

void printMatrix(const std::vector<std::vector<int>>& matrix) {
    for (const auto& row : matrix) {
        for (int i = 0; i < row.size(); ++i) {
            std::cout << row[i];
            if (i + 1 < row.size()) std::cout << " ";
        }
        std::cout << "\n";
    }
}

vector<vector<int>> preference_to_ranking(vector<vector<int>> proposer_preference, 
                                          int num_proposers, int num_receivers, int num_choices) {
    vector<vector<int>> proposer_ranking;

    // Initialize ranking matrix, fill with default -1's
    proposer_ranking.resize(num_proposers);
    for (int i = 0; i < num_proposers; i++) {
        proposer_ranking[i].assign(num_receivers, -1);
    }

    int proposer_idx = 0;
    for (auto proposer : proposer_preference) {
        int num_choices = 0;
        for (auto choice : proposer) {
            if (!(choice == -1)) {
                proposer_ranking[proposer_idx][choice] = num_choices;
                num_choices++;
            }
        }
        proposer_idx++;
    }

    return proposer_ranking;
}

vector<vector<int>> ranking_to_preference(vector<vector<int>> receiver_ranking, 
                                          int num_proposers, int num_receivers, int num_choices) {
    vector<vector<int>> receiver_preference;

    // Initialize preference matrix
    receiver_preference.resize(num_receivers);
    for (int i = 0; i < num_receivers; i++) {
        receiver_preference[i].resize(num_proposers);
    }

    int receiver_idx = 0;
    for (auto receiver : receiver_ranking) {
        int proposer_idx = 0;
        for (auto proposer : receiver) {
            receiver_preference[receiver_idx][receiver_ranking[receiver_idx][proposer_idx]] = proposer_idx;
            proposer_idx++;
        }
        receiver_idx++;
    }

    // Remove -1's
    for (auto receiver : receiver_preference) {
        receiver.erase(remove(receiver.begin(), receiver.end(), -1), receiver.end());
        // Ensure size is as expected
        if (receiver.size() != num_choices) {
            cerr << "Could not invert ranking matrix" << endl;
            return vector<vector<int>>();
        }
    }
    return receiver_preference;
}

// Moves previously matched entries to the end of each stated preference list.
// to_last[i] lists column indices that should be moved to last for row i.
vector<vector<int>> move_prior_matches_to_last(vector<vector<int>> preference,
                                               const map<int, vector<int>>& to_last) {
    for (const auto& [i, last_indices] : to_last) {
        if (i >= (int)preference.size()) continue;
        std::unordered_set<int> last_set(last_indices.begin(), last_indices.end());
        vector<int> keep, last;
        for (int x : preference[i]) {
            (last_set.count(x) ? last : keep).push_back(x);
        }
        keep.insert(keep.end(), last.begin(), last.end());
        preference[i] = keep;
    }
    return preference;
}

// deprioritize[i] lists column indices for row i that should be placed last in the random fill
// (used to penalize re-matching previously paired people via randomization)
vector<vector<int>> fill_preference(vector<vector<int>> preference, int num_rows, int num_cols,
                                    const map<int, vector<int>>& deprioritize = {}) {
    vector<int> default_row(num_cols);
    for (int i = 0; i < num_cols; i++) {
        default_row[i] = i;
    }

    std::random_device rd;
    std::mt19937 r(rd());

    for (int i = 0; i < num_rows; i++) {
        bool new_row = (i >= (int)preference.size());
        if (!new_row && (int)preference[i].size() >= num_cols) continue;

        vector<int> to_shuffle;
        if (new_row) {
            to_shuffle = default_row;
        } else {
            std::unordered_set<int> existing(preference[i].begin(), preference[i].end());
            for (int num : default_row) {
                if (!existing.count(num)) to_shuffle.push_back(num);
            }
        }

        // Partition into normal and deprioritized; shuffle each; deprioritized appended last
        vector<int> normal, deprio;
        const vector<int>* prev = deprioritize.count(i) ? &deprioritize.at(i) : nullptr;
        for (int x : to_shuffle) {
            bool is_deprio = prev && std::find(prev->begin(), prev->end(), x) != prev->end();
            (is_deprio ? deprio : normal).push_back(x);
        }
        std::shuffle(normal.begin(), normal.end(), r);
        std::shuffle(deprio.begin(), deprio.end(), r);
        normal.insert(normal.end(), deprio.begin(), deprio.end());

        if (new_row) {
            preference.push_back(normal);
        } else {
            preference[i].insert(preference[i].end(), normal.begin(), normal.end());
        }
    }

    return preference;
}

}