#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <queue>
#include <cmath>
#include <utility> // for pair

using std::string, std::cout, std::cerr, std::endl, std::map, std::vector, std::queue, std::priority_queue;
using std::stringstream, std::ceil, std::pair;

namespace matching {

const map<int, string> PROPOSER_NAMES = {{0, "nina"}, {1, "sam"}, {2, "fernanda"}, {3, "marisa"}, {4, "jasmine"}, {5, "shree"}, {6, "caleb"}};
const map<int, string> RECEIVER_NAMES = {{0, "jada"}, {1, "clayton"}, {2, "kylie"}, {3, "lexi"}, {4, "quinn"}};

// IMPORTANT:
// proposer_rankings[p][r] gives proposer p's r'th preference
// receiver_rankings[r][p] gives receiver r's ranking for proposer p
void algorithm(vector<vector<int>> proposer_ranking, vector<vector<int>> receiver_ranking, vector<int> receiver_capacity) {
    const int kNumProposers = proposer_ranking.size();
    const int kNumReceivers = receiver_ranking.size();
    const int KNumChoices = proposer_ranking[0].size();
    // ADD CHECKS SOMEWHERE TO MAKE SURE ALL INPUTS ARE VALID

    queue<int> free_proposers;
    vector<int> next_proposal; // next_proposal[k] gives index in proposer_rankings for 
                               // proposer k's highest not-yet-proposed-to preference
    vector<priority_queue<pair<int, int>>> matching; // matching[i] gives receiver i's tentative proposer(s)
                                                     // pair.first gives receiver's ranking for pair.second
    matching.resize(receiver_capacity.size());

    for (int i = 0; i < kNumProposers; i++) {
        free_proposers.push(i);
        next_proposal.push_back(0);
    }

    while (!free_proposers.empty()) {
        int next_proposer = free_proposers.front();
        free_proposers.pop();
        
        cout << "\nStaging proposer " << PROPOSER_NAMES.at(next_proposer) << endl;

        if (next_proposal[next_proposer] >= KNumChoices) {
            // should never happen! throw some sort of warning message
            cout << "Warning! Unable to match proposer " << PROPOSER_NAMES.at(next_proposer) << endl;
            continue;
        }

        int next_proposer_pref = proposer_ranking[next_proposer][next_proposal[next_proposer]];

        cout << "Proposer " << PROPOSER_NAMES.at(next_proposer) << "'s next preference is receiver ";
        cout << RECEIVER_NAMES.at(next_proposer_pref) << endl;

        next_proposal[next_proposer]++;

        if (matching[next_proposer_pref].size() < receiver_capacity[next_proposer_pref]) { // tentative receiver is unmatched
            cout << "Receiver " << RECEIVER_NAMES.at(next_proposer_pref) << " has space; matching with proposer ";
            cout << PROPOSER_NAMES.at(next_proposer) << endl;

            matching[next_proposer_pref].push(std::make_pair(receiver_ranking[next_proposer_pref][next_proposer], next_proposer));
        } else { // note the strictly greater than! revisit later
            if (receiver_ranking[next_proposer_pref][next_proposer] < 
                matching[next_proposer_pref].top().first) {
                
                cout << "Switching receiver " << RECEIVER_NAMES.at(next_proposer_pref) << " from proposer ";
                cout << PROPOSER_NAMES.at(matching[next_proposer_pref].top().second) << " to proposer ";
                cout << PROPOSER_NAMES.at(next_proposer) << endl;

                // unmatch prior proposer, add to free proposers queue
                free_proposers.push(matching[next_proposer_pref].top().second);
                matching[next_proposer_pref].pop();

                // replace w/ current proposer
                matching[next_proposer_pref].push(std::make_pair(receiver_ranking[next_proposer_pref][next_proposer], next_proposer));
            } else {
                // next_proposer is rejected :( requeue
                // TO-DO: fix - repeat until next_proposer is assigned!
                cout << "Couldn't find a match for proposer " << PROPOSER_NAMES.at(next_proposer);
                cout << " - adding back to queue" << endl;

                free_proposers.push(next_proposer);
            }
        }
    }
}

}

int main(int argc, char* argv[]) {
    // TEST ALGORITHM
    vector<vector<int>> proposer_ranking = {{1, 2, 0}, {2, 1, 0}, {2, 1, 0}, {2, 0, 1}, {2, 1, 0}};
    vector<vector<int>> receiver_ranking = {{4, 0, 3}, {2, 4, 1}, {1, 2, 3}};
    vector<int> receiver_capacity = {2, 1, 2};

    matching::algorithm(proposer_ranking, receiver_ranking, receiver_capacity);
}