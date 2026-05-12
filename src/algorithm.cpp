#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <queue>
#include <cmath>
#include <utility> // for pair, move
#include <numeric>  // for iota

#include "matrix-utils.cpp"
#include "read.cpp"

using std::string, std::cout, std::cerr, std::endl, std::map, std::vector, std::queue, std::priority_queue;
using std::stringstream, std::ceil, std::pair, std::get, std::ostringstream, std::move;

map<int, string> PROPOSER_NAMES, RECEIVER_NAMES;


// TO-DO: need to ensure all bigs get at least one little!
// Maybe run many times on possible permutations of capacities and choose based on total compatibility?
// Add utils file with compatibility-check that computes for a full matching, as well as for a single matching

namespace matching {

class Match {

    public:
        Match(int r, priority_queue<pair<int, int>> p) 
            : receiver(r), proposers(move(p)) {}
        
        // default
        Match(int r) 
            : receiver(r) {}

        // TO BE REVISED
        string toString(const vector<vector<int>>& proposer_preference, const vector<vector<int>>& receiver_preference) const {
            ostringstream oss;
            oss << "Receiver: " << RECEIVER_NAMES.at(receiver) << "\n";

            int num_receiver_choices = (receiver < (int)receiver_preference.size())
                                       ? (int)receiver_preference[receiver].size() : 0;

            priority_queue<pair<int, int>> copy = proposers;
            while (!copy.empty()) {
                int receiver_rank = copy.top().first;
                int proposer_idx = copy.top().second;

                int proposer_rank = -1;
                if (proposer_idx < (int)proposer_preference.size()) {
                    const auto& prefs = proposer_preference[proposer_idx];
                    for (int i = 0; i < (int)prefs.size(); i++) {
                        if (prefs[i] == receiver) { proposer_rank = i; break; }
                    }
                }

                int display_p_rank = (proposer_rank == -1) ? -1 : proposer_rank + 1;
                int display_r_rank = (receiver_rank >= num_receiver_choices) ? -1 : receiver_rank + 1;

                oss << "  " << PROPOSER_NAMES.at(proposer_idx)
                    << " (proposer rank: " << display_p_rank
                    << ", receiver rank: " << display_r_rank << ")\n";
                copy.pop();
            }
            return oss.str();
        }

        // Pass original (unfilled) proposer_preference so proposer_rank == -1 means truly unranked.
        // original_receiver_choices: number of choices in the receiver's original (unfilled) list;
        // receiver_rank >= this value means the proposer wasn't originally ranked by the receiver.
        // proposer_prior_matches[p] lists receivers proposer p was matched with previously.
        // receiver_prior_matches[r] lists proposers receiver r was matched with previously;
        // used to prefer giving twin slots to receivers who have met fewer proposers overall.
        int measureCompatibility(const vector<vector<int>>& proposer_preference,
                                 const vector<vector<int>>& receiver_preference,
                                 const map<int, vector<int>>& proposer_prior_matches = {},
                                 const map<int, vector<int>>& receiver_prior_matches = {}) const {
            const int kMutualUnrankedPenalty = 500;
            const int kPreviouslyMatchedPenalty = 10000;
            const int kTwinPriorMatchPenalty = 50;

            int num_receiver_choices = (receiver < (int)receiver_preference.size())
                                       ? (int)receiver_preference[receiver].size() : 0;
            int total = 0;
            priority_queue<pair<int, int>> copy = proposers;
            while (!copy.empty()) {
                int receiver_rank = copy.top().first;
                int proposer_idx = copy.top().second;
                int proposer_rank = -1;
                if (proposer_idx < (int)proposer_preference.size()) {
                    const auto& prefs = proposer_preference[proposer_idx];
                    for (int i = 0; i < (int)prefs.size(); i++) {
                        if (prefs[i] == receiver) { proposer_rank = i + 1; break; }
                    }
                }

                if (proposer_prior_matches.count(proposer_idx)) {
                    const auto& prev = proposer_prior_matches.at(proposer_idx);
                    if (std::find(prev.begin(), prev.end(), receiver) != prev.end()) {
                        total += kPreviouslyMatchedPenalty;
                    }
                }

                bool proposer_unranked = (proposer_rank == -1);
                bool receiver_unranked = (receiver_rank >= num_receiver_choices);
                bool proposer_no_list = (proposer_idx >= (int)proposer_preference.size() ||
                                         proposer_preference[proposer_idx].empty());
                bool receiver_no_list = (receiver >= (int)receiver_preference.size() ||
                                         receiver_preference[receiver].empty());

                if (proposer_unranked && receiver_unranked) {
                    if (!proposer_no_list && !receiver_no_list) total += kMutualUnrankedPenalty;
                    else if (!proposer_no_list) total += kMutualUnrankedPenalty/2;
                    else if (!receiver_no_list) total += kMutualUnrankedPenalty/4;
                } else if (proposer_unranked) {
                    if (!proposer_no_list) total += kMutualUnrankedPenalty/2;
                    total += (receiver_rank * 2);
                } else if (receiver_unranked) {
                    if (!receiver_no_list) total += kMutualUnrankedPenalty/4;
                    total += (proposer_rank * 3);
                } else {
                    total += (proposer_rank * 3) + (receiver_rank * 2);
                }
                copy.pop();
            }

            // Penalize twin assignments for receivers who have already met more proposers
            if (proposers.size() == 2) {
                int prior_count = receiver_prior_matches.count(receiver)
                                  ? (int)receiver_prior_matches.at(receiver).size() : 0;
                total += prior_count * kTwinPriorMatchPenalty;
            }

            return total;
        }

        // Fields - public for now, to be revisited
        int receiver;
        priority_queue<pair<int, int>> proposers;
        // .first gives receiver's ranking for proposer,
        // .second gives proposer index,
};

// Returns all vectors of length `total` containing exactly `num_twos` 2s and the rest 1s.
vector<vector<int>> capacityCombinations(int total, int num_twos) {
    vector<vector<int>> result;
    vector<int> indices(num_twos);
    std::iota(indices.begin(), indices.end(), 0);

    while (true) {
        vector<int> combo(total, 1);
        for (int idx : indices) combo[idx] = 2;
        result.push_back(combo);

        int i = num_twos - 1;
        while (i >= 0 && indices[i] == total - num_twos + i) i--;
        if (i < 0) break;
        indices[i]++;
        for (int j = i + 1; j < num_twos; j++) indices[j] = indices[j - 1] + 1;
    }
    return result;
}

int highestProposerRank(const vector<Match>& matching, const vector<vector<int>>& proposer_preference,
                        int num_original_proposers) {
    int highest = -1;
    for (const auto& match : matching) {
        priority_queue<pair<int, int>> copy = match.proposers;
        while (!copy.empty()) {
            int proposer_idx = copy.top().second;
            if (proposer_idx < num_original_proposers) {
                const auto& prefs = proposer_preference[proposer_idx];
                bool found = false;
                for (int i = 0; i < (int)prefs.size(); i++) {
                    if (prefs[i] == match.receiver) { highest = std::max(highest, i + 1); found = true; break; }
                }
                if (!found) highest = std::max(highest, (int)prefs.size() + 1);
            }
            copy.pop();
        }
    }
    return highest;
}

// IMPORTANT:
// proposer_preference[p][r] gives proposer p's r'th preference
// receiver_ranking[r][p] gives receiver r's ranking for proposer p
vector<Match> algorithm(vector<vector<int>> proposer_preference, vector<vector<int>> receiver_ranking, vector<int> receiver_capacity) {
    const int kNumProposers = proposer_preference.size();
    const int kNumReceivers = receiver_ranking.size();
    const int KNumChoices = proposer_preference[0].size();

    queue<int> free_proposers;
    vector<int> next_proposal; // next_proposal[k] gives index in proposer_preference for 
                               // proposer k's highest not-yet-proposed-to preference
    vector<Match> matching; // matching[i] gives receiver i's tentative proposer(s)
    for (int i = 0; i < kNumReceivers; i++) {
        matching.push_back(Match(i));
    }

    for (int i = 0; i < kNumProposers; i++) {
        free_proposers.push(i);
        next_proposal.push_back(0);
    }

    int next_proposer;
    bool next = true;
    while (!free_proposers.empty() || !next) {
        if (next) {
            next_proposer = free_proposers.front();
            free_proposers.pop();
        }
        
        // cout << "\nStaging proposer " << PROPOSER_NAMES.at(next_proposer) << endl;

        if (next_proposal[next_proposer] >= KNumChoices) {
            cerr << "Warning! Unable to match proposer " << PROPOSER_NAMES.at(next_proposer) << endl;
            next = true;
            continue;
        }

        int next_proposer_pref = proposer_preference[next_proposer][next_proposal[next_proposer]];

        if (next_proposer_pref == -1) {
            cerr << "Warning! Unable to match proposer " << PROPOSER_NAMES.at(next_proposer) << endl;
            next = true;
            continue;
        }

        // cout << "Proposer " << PROPOSER_NAMES.at(next_proposer) << "'s next preference is receiver ";
        // cout << RECEIVER_NAMES.at(next_proposer_pref) << endl;

        next_proposal[next_proposer]++;

        if (matching[next_proposer_pref].proposers.size() < receiver_capacity[next_proposer_pref]) { // tentative receiver is unmatched
            // cout << "Receiver " << RECEIVER_NAMES.at(next_proposer_pref) << " has space; matching with proposer ";
            // cout << PROPOSER_NAMES.at(next_proposer) << endl;

            matching[next_proposer_pref].proposers.push(std::make_pair(receiver_ranking[next_proposer_pref][next_proposer], next_proposer));
            next = true;
        } else { // note the strictly greater than! revisit later
                 // I HARD CODED RECEIVER RANKS FOR UNRANKED PROPOSERS TO -1
            if (receiver_ranking[next_proposer_pref][next_proposer] < matching[next_proposer_pref].proposers.top().first &&
                receiver_ranking[next_proposer_pref][next_proposer] != -1) {
                
                // cout << "Switching receiver " << RECEIVER_NAMES.at(next_proposer_pref) << " from proposer ";
                // cout << PROPOSER_NAMES.at(matching[next_proposer_pref].proposers.top().second) << " to proposer ";
                // cout << PROPOSER_NAMES.at(next_proposer) << endl;

                // unmatch prior proposer, add to free proposers queue
                free_proposers.push(matching[next_proposer_pref].proposers.top().second);
                matching[next_proposer_pref].proposers.pop();

                // replace w/ current proposer
                matching[next_proposer_pref].proposers.push(std::make_pair(receiver_ranking[next_proposer_pref][next_proposer], next_proposer));
                next = true;
            } else {
                // next_proposer is rejected :( repeat process
                // cout << "Couldn't find a match for proposer " << PROPOSER_NAMES.at(next_proposer) << endl;
                next = false;
            }
        }
    }
    return matching;
}

}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <littles_csv> <bigs_csv> [prior_results.csv ...] [--twins big_name ...]" << endl;
        return 1;
    }

    vector<vector<int>> proposer_preference, receiver_preference;
    matching::read(argv[1], argv[2], PROPOSER_NAMES, RECEIVER_NAMES, proposer_preference, receiver_preference);

    vector<string> prior_csvs;
    vector<string> twins;
    bool parsing_twins = false;
    for (int i = 3; i < argc; i++) {
        if (string(argv[i]) == "--twins") { parsing_twins = true; continue; }
        (parsing_twins ? twins : prior_csvs).push_back(argv[i]);
    }

    map<int, vector<int>> prior_matches = matching::read_prior_matches(prior_csvs, PROPOSER_NAMES, RECEIVER_NAMES);

    // Build proposer-keyed inverse for fill_preference on the proposer side and compatibility scoring
    map<int, vector<int>> proposer_deprioritize;
    for (const auto& [r, ps] : prior_matches) {
        for (int p : ps) proposer_deprioritize[p].push_back(r);
    }

    // Move prior matches to the end of each person's stated preference list
    proposer_preference = matching::move_prior_matches_to_last(proposer_preference, proposer_deprioritize);
    receiver_preference = matching::move_prior_matches_to_last(receiver_preference, prior_matches);

    vector<matching::Match> best_match;
    int best_compatibility = -1;

    int num_receivers = (int)RECEIVER_NAMES.size();
    int num_proposers = (int)PROPOSER_NAMES.size();

    // Build base capacities from forced twins
    vector<int> base_capacities(num_receivers, 1);
    map<string, int> receiver_idx;
    for (const auto& [i, name] : RECEIVER_NAMES) receiver_idx[name] = i;
    int forced_twin_count = 0;
    for (const auto& name : twins) {
        if (receiver_idx.count(name)) { base_capacities[receiver_idx.at(name)] = 2; forced_twin_count++; }
        else cerr << "Warning: unknown big '" << name << "' in --twins" << endl;
    }

    // Indices of receivers not already forced to capacity 2
    vector<int> non_twin_indices;
    for (int i = 0; i < num_receivers; i++) {
        if (base_capacities[i] == 1) non_twin_indices.push_back(i);
    }

    // How many more twins are needed to seat all proposers
    int additional_twins_needed = num_proposers - num_receivers - forced_twin_count;

    vector<vector<int>> capacity_combos;
    if (additional_twins_needed <= 0) {
        capacity_combos = {base_capacities};
    } else {
        for (const auto& combo : matching::capacityCombinations(non_twin_indices.size(), additional_twins_needed)) {
            vector<int> caps = base_capacities;
            for (int i = 0; i < (int)non_twin_indices.size(); i++) caps[non_twin_indices[i]] = combo[i];
            capacity_combos.push_back(caps);
        }
    }

    for (const auto& capacities : capacity_combos) {
        int combo_best_compatibility = -1;
        vector<matching::Match> combo_best_match;
        for (int num_iterations = 0; num_iterations < 100; num_iterations++) {
            vector<vector<int>> filled_receiver_preference = matching::fill_preference(receiver_preference, num_receivers, num_proposers, prior_matches);
            vector<vector<int>> filled_receiver_ranking = matching::preference_to_ranking(filled_receiver_preference, num_receivers, num_proposers, 5);
            vector<vector<int>> filled_proposer_preference = matching::fill_preference(proposer_preference, num_proposers, num_receivers, proposer_deprioritize);

            vector<matching::Match> matching = matching::algorithm(filled_proposer_preference, filled_receiver_ranking, capacities);

            if (matching::highestProposerRank(matching, proposer_preference, (int)proposer_preference.size()) > (int)proposer_preference[0].size())
                continue;

            int total_compatibility = 0;
            for (const auto& match : matching) {
                total_compatibility += match.measureCompatibility(proposer_preference, receiver_preference, proposer_deprioritize, prior_matches);
            }

            if (combo_best_compatibility == -1 || total_compatibility < combo_best_compatibility) {
                combo_best_compatibility = total_compatibility;
                combo_best_match = matching;
            }
        }

        if (combo_best_compatibility != -1 &&
            (best_compatibility == -1 || combo_best_compatibility < best_compatibility)) {
            best_compatibility = combo_best_compatibility;
            best_match = combo_best_match;
        }
    }

    for (const auto& match : best_match) {
        cout << match.toString(proposer_preference, receiver_preference) << endl;
    }
    cout << "Compatibility score: " << best_compatibility << endl;

    return 0;
}