#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <queue>
#include <cmath>
#include <utility> // for pair, move

#include "matrix-utils.cpp"

using std::string, std::cout, std::cerr, std::endl, std::map, std::vector, std::queue, std::priority_queue;
using std::stringstream, std::ceil, std::tuple, std::get, std::ostringstream, std::move;

const map<int, string> PROPOSER_NAMES = {{0, "lauren"}, {1, "chloe"}, {2, "kevin"}, {3, "kylie"}, {4, "varsha"}, {5, "cadence"}, {6, "katherine"}, {7, "trisha"}, {8, "hibah"}, {9, "katharina"}};
const map<int, string> RECEIVER_NAMES = {{0, "harika"}, {1, "quinn s"}, {2, "lexi"}, {3, "jasmine"}, {4, "eliana"}, {5, "jordan"}, {6, "tshiring"}, {7, "jiayi"}, {8, "quinn a"}, {9, "stephanie"}};

namespace matching {

class Match {

    public:
        Match(int r, priority_queue<tuple<int, int, bool>> p) 
            : receiver(r), proposers(move(p)) {}
        
        // default
        Match(int r) 
            : receiver(r) {}

        string toString() const {
            ostringstream oss;
            oss << "------------" << endl;
            oss << "Receiver " << RECEIVER_NAMES.at(receiver) << endl;
            priority_queue<tuple<int, int, bool>> copy = proposers;
            while (!copy.empty()) {
                oss << PROPOSER_NAMES.at(get<1>(copy.top())) << endl;
                copy.pop();
            }
            oss << "------------" << endl;
            return oss.str();
        }

        // Fields - public for now, to be revisited
        int receiver;
        priority_queue<tuple<int, int, bool>> proposers;
        // get<0> gives receiver's ranking for proposer,
        // get<1> gives proposer index,
        // get<2> gives whether the pairing is fixed
};

// THIS NEEDS TO BE MOVED, added here for now just so I can test the algorithm method
tuple<int, int, bool, int> hard_match(int proposer, vector<vector<int>> proposer_preference, vector<vector<int>> receiver_ranking);

// IMPORTANT:
// proposer_preference[p][r] gives proposer p's r'th preference
// receiver_ranking[r][p] gives receiver r's ranking for proposer p
vector<Match> algorithm(vector<vector<int>> proposer_preference, vector<vector<int>> receiver_ranking, vector<int> receiver_capacity) {
    const int kNumProposers = proposer_preference.size();
    const int kNumReceivers = receiver_ranking.size();
    const int KNumChoices = proposer_preference[0].size();
    // TO-DO: ADD CHECKS SOMEWHERE TO MAKE SURE ALL INPUTS ARE VALID

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
    while (!free_proposers.empty()) {
        if (next) {
            next_proposer = free_proposers.front();
            free_proposers.pop();
        }
        
        cout << "\nStaging proposer " << PROPOSER_NAMES.at(next_proposer) << endl;

        if (next_proposal[next_proposer] >= KNumChoices) {
            // should never happen! throw some sort of warning message
            cerr << "Warning! Unable to match proposer " << PROPOSER_NAMES.at(next_proposer) << endl;

            tuple<int, int, bool, int> hard_match_res = hard_match(next_proposer, proposer_preference, receiver_ranking);
            int hard_match_receiver = get<3>(hard_match_res);
            tuple<int, int, bool> hard_match_tuple = std::make_tuple(get<0>(hard_match_res), get<1>(hard_match_res), get<2>(hard_match_res));
            
            if (matching[hard_match_receiver].proposers.size() >= receiver_capacity[hard_match_receiver]) {
                matching[hard_match_receiver].proposers.pop();
            }
            matching[hard_match_receiver].proposers.push(hard_match_tuple);
            next = true;
            continue;
        }

        int next_proposer_pref = proposer_preference[next_proposer][next_proposal[next_proposer]];

        // THIS SHOULD NEVER HAPPEN! So I'm commenting it out (need to talk to Jada!)
        // But before finalizing this should be fixed to throw an unrecoverable error.
        if (next_proposer_pref == -1) {  // may happen before the above if proposer didn't have full ranking
            cerr << "Warning! Unable to match proposer " << PROPOSER_NAMES.at(next_proposer) << endl;

            // ADDED THE FOLLOWING TO TEST ON QUINN'S DATA, where some proposers don't have full rankings.
            tuple<int, int, bool, int> hard_match_res = hard_match(next_proposer, proposer_preference, receiver_ranking);
            int hard_match_receiver = get<3>(hard_match_res);
            tuple<int, int, bool> hard_match_tuple = std::make_tuple(get<0>(hard_match_res), get<1>(hard_match_res), get<2>(hard_match_res));
            
            if (matching[hard_match_receiver].proposers.size() >= receiver_capacity[hard_match_receiver]) {
                matching[hard_match_receiver].proposers.pop();
            }
            matching[hard_match_receiver].proposers.push(hard_match_tuple);
            next = true;
            continue;
        }

        cout << "Proposer " << PROPOSER_NAMES.at(next_proposer) << "'s next preference is receiver ";
        cout << RECEIVER_NAMES.at(next_proposer_pref) << endl;

        next_proposal[next_proposer]++;

        if (matching[next_proposer_pref].proposers.size() < receiver_capacity[next_proposer_pref]) { // tentative receiver is unmatched
            cout << "Receiver " << RECEIVER_NAMES.at(next_proposer_pref) << " has space; matching with proposer ";
            cout << PROPOSER_NAMES.at(next_proposer) << endl;

            matching[next_proposer_pref].proposers.push(std::make_tuple(receiver_ranking[next_proposer_pref][next_proposer], next_proposer, false));
            next = true;
        } else { // note the strictly greater than! revisit later
                 // I HARD CODED RECEIVER RANKS FOR UNRANKED PROPOSERS TO -1
            if (receiver_ranking[next_proposer_pref][next_proposer] < get<0>(matching[next_proposer_pref].proposers.top()) &&
                receiver_ranking[next_proposer_pref][next_proposer] != -1) {
                
                cout << "Switching receiver " << RECEIVER_NAMES.at(next_proposer_pref) << " from proposer ";
                cout << PROPOSER_NAMES.at(get<1>(matching[next_proposer_pref].proposers.top())) << " to proposer ";
                cout << PROPOSER_NAMES.at(next_proposer) << endl;

                // unmatch prior proposer, add to free proposers queue
                free_proposers.push(get<1>(matching[next_proposer_pref].proposers.top()));
                matching[next_proposer_pref].proposers.pop();

                // replace w/ current proposer
                matching[next_proposer_pref].proposers.push(std::make_tuple(receiver_ranking[next_proposer_pref][next_proposer], next_proposer, false));
                next = true;
            } else {
                // next_proposer is rejected :( repeat process
                cout << "Couldn't find a match for proposer " << PROPOSER_NAMES.at(next_proposer) << endl;
                next = false;
            }
        }
    }
    return matching;
}

// Helper method; if unable to find a stable match for a proposer (little),
// we hard-match them with their most compatible receiver, where higher compatibility
// is defined by a lower sum of proposer and receiver rankings for each other.
// This receiver MUST BE ON THEIR PREFERENCE LIST.
// (Revisit this; this isn't proposer-optimal, and I think that's giving me a weird result.)
//
// TO-DO: we should add a caveat that now, the match isn't stable! (i.e. no stable matching exists)
tuple<int, int, bool, int> hard_match(int proposer, vector<vector<int>> proposer_preference, vector<vector<int>> receiver_ranking) {
    vector<int> proposer_choices = proposer_preference[proposer];

    int best_receiver = -1;
    int best_compatibility = INT_MAX;

    for (int i = 0; i < proposer_choices.size(); i++) {
        int receiver = proposer_choices[i];
        if (receiver == -1) continue;

        int compatibility = i + receiver_ranking[receiver][proposer];
        if (compatibility < best_compatibility) {
            best_compatibility = compatibility;
            best_receiver = receiver;
        }
    }

    // TO-DO: add a check that chosen receiver has non-fixed space. If not, we need to use the next-best option.
    cout << "Hard-matching proposer " << PROPOSER_NAMES.at(proposer) << " with receiver " << RECEIVER_NAMES.at(best_receiver) << endl;
    return std::make_tuple(receiver_ranking[best_receiver][proposer], proposer, true, best_receiver);
}

}

// const map<int, string> PROPOSER_NAMES = {{0, "lauren"}, {1, "chloe"}, {2, "kevin"}, {3, "kylie"}, {4, "varsha"}, {5, "cadence"}, {6, "katherine"}, {7, "trisha"}, {8, "hibah"}, {9, "katharina"}};
// const map<int, string> RECEIVER_NAMES = {{0, "harika"}, {1, "quinn s"}, {2, "lexi"}, {3, "jasmine"}, {4, "eliana"}, {5, "jordan"}, {6, "tshiring"}, {7, "jiayi"}, {8, "quinn a"}, {9, "stephanie"}};

int main(int argc, char* argv[]) {
    // TEST ALGORITHM
    vector<vector<int>> proposer_preference = {{3, 7, 1, 2, 6}, {3, 2, 9, 8, 1}, {7, 9, 6, 0, 5}, {7, 2, 0, 4, 6}, {6, 9, 3, 0, -1}, {8, 2, 3, 0, 5}, {7, 1, 9, 5, -1}, {3, 0, 1, 2, -1}, {9, 2, 6, 7, 4}, {7, 9, 1, 8, 2}};
    vector<vector<int>> receiver_preference = {{5, 4, 3, 2, 7}, {0, 6, 9, 2, 8}, {3, 9, 8, 0, 5}, {0, 7, 1, 4, 5}, {3, 0, 8, -1, -1}, {2, 5, 6, 1, 0}, {0, 4, 8, 2, 3}, {9, 3, 0, 8, 2}, {1, 5, 3, 4, 9}, {9, 1, 4, 8, 2}};
    vector<int> receiver_capacity = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

    vector<vector<int>> receiver_ranking = matching::preference_to_ranking(receiver_preference, 10, 10, 5);

    vector<matching::Match> matching = matching::algorithm(proposer_preference, receiver_ranking, receiver_capacity);

    for (auto m : matching) {
        cout << m.toString() << endl;
    }
}