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

// const map<int, string> PROPOSER_NAMES = {{0, "lauren"}, {1, "chloe"}, {2, "kevin"}, {3, "kylie"}, {4, "varsha"}, {5, "cadence"}, {6, "katherine"}, {7, "trisha"}, {8, "hibah"}, {9, "katharina"}};
// const map<int, string> RECEIVER_NAMES = {{0, "harika"}, {1, "quinn s"}, {2, "lexi"}, {3, "jasmine"}, {4, "eliana"}, {5, "jordan"}, {6, "tshiring"}, {7, "jiayi"}, {8, "quinn a"}, {9, "stephanie"}};

// const map<int, string> PROPOSER_NAMES = {{0, "cailin"}, {1, "carson"}, {2, "eireann"}, {3, "eunice"}, {4, "karman"}, {5, "kisha"},
//                                          {6, "kruthi"}, {7, "lily"}, {8, "lindsay"}, {9, "manya"}, {10, "marie"}, {11, "namoir"},
//                                          {12, "natania"}, {13, "queenee"}, {14, "surya"}, {15, "tida"}, {16, "tiffany"},
//                                          {17, "vienna"}, {18, "will"}};
// const map<int, string> RECEIVER_NAMES = {{0, "cadence"}, {1, "clayton"}, {2, "cory"}, {3, "fernanda"}, {4, "irene"}, {5, "jada"},
//                                          {6, "katherine"}, {7, "naseem"}, {8, "raagini"}, {9, "shree"}, {10, "spencer"}, 
//                                          {11, "marisa"}, {12, "tania"}, {13, "yuna"}, {14, "varsha"}};

const map<int, string> PROPOSER_NAMES = {{0, "maddie"}, {1, "aarna"}, {2, "shirley"}, {3, "taryn"}, {4, "pareesha"}, {5, "abby"},
                                         {6, "audrey"}, {7, "electra"}, {8, "sadi"}, {9, "joyce"}, {10, "megan pereira"}, 
                                         {11, "megan ly"}, {12, "natalie"}, {13, "ingrid"}, {14, "sophia"}, {15, "inderjeet"},
                                         {16, "elise"}, {17, "maren"}, {18, "sara"}, {19, "naema"}, {20, "christine"},
                                         {21, "aabritty"}, {22, "haimkiry"}, {23, "leonie"}};
const map<int, string> RECEIVER_NAMES = {{0, "raagini"}, {1, "marie"}, {2, "tania"}, {3, "vienna"}, {4, "sam"}, {5, "kruthi"},
                                         {6, "fernanda"}, {7, "marisa"}, {8, "spencer"}, {9, "nina"}, {10, "namoir"}, 
                                         {11, "lindsay"}, {12, "natania"}, {13, "tiffany"}, {14, "emma"}, {15, "eunice"},
                                         {16, "jasmine"}, {17, "naseem"}, {18, "katherine"}, {19, "lily"}};

// TO-DO: need to ensure all bigs get at least one little!
// Maybe run many times on possible permutations of capacities and choose based on total compatibility?
// Add utils file with compatibility-check that computes for a full matching, as well as for a single matching

namespace matching {

class Match {

    public:
        Match(int r, int n, priority_queue<tuple<int, int, bool>> p) 
            : receiver(r), num_hard_proposers(n), proposers(move(p)) {}
        
        // default
        Match(int r) 
            : receiver(r), num_hard_proposers(0) {}

        string toString() const {
            ostringstream oss;
            oss << "------------" << endl;
            oss << "Receiver " << RECEIVER_NAMES.at(receiver) << endl;
            priority_queue<tuple<int, int, bool>> copy = proposers;
            while (!copy.empty()) {
                oss << PROPOSER_NAMES.at(get<1>(copy.top())) << endl;
                copy.pop();
            }
            return oss.str();
        }

        // Fields - public for now, to be revisited
        int receiver;
        int num_hard_proposers;
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
    // const int kNumProposers = proposer_preference.size();
    // const int kNumReceivers = receiver_ranking.size();
    // const int KNumChoices = proposer_preference[0].size();

    const int kNumProposers = 19;
    const int kNumReceivers = 15;
    const int KNumChoices = 5;
    // TO-DO: ADD CHECKS SOMEWHERE TO MAKE SURE ALL INPUTS ARE VALID
    // Had to hard code these for testing. Figure out why and fix

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
        
        cout << "\nStaging proposer " << PROPOSER_NAMES.at(next_proposer) << endl;

        if (next_proposal[next_proposer] >= KNumChoices) {
            // should never happen! throw some sort of warning message
            cerr << "Warning! Unable to match proposer " << PROPOSER_NAMES.at(next_proposer) << endl;

            tuple<int, int, bool, int> hard_match_res = hard_match(next_proposer, proposer_preference, receiver_ranking);
            int hard_match_receiver = get<3>(hard_match_res);
            tuple<int, int, bool> hard_match_tuple = std::make_tuple(get<0>(hard_match_res), get<1>(hard_match_res), get<2>(hard_match_res));
            
            if (matching[hard_match_receiver].proposers.size() >= receiver_capacity[hard_match_receiver]) {
                cout << "Adding proposer " << PROPOSER_NAMES.at(get<1>(matching[hard_match_receiver].proposers.top()));
                cout << " back to free proposers queue" << endl;
                free_proposers.push(get<1>(matching[hard_match_receiver].proposers.top()));
                matching[hard_match_receiver].proposers.pop();
            }
            matching[hard_match_receiver].proposers.push(hard_match_tuple);
            next = true;
            continue;
        }

        int next_proposer_pref = proposer_preference[next_proposer][next_proposal[next_proposer]];

        // THIS SHOULD NEVER HAPPEN! So I'll be commenting it out.
        // But before finalizing this should be fixed to throw an unrecoverable error.
        if (next_proposer_pref == -1) {  // may happen before the above if proposer didn't have full ranking
            cerr << "Warning! Unable to match proposer " << PROPOSER_NAMES.at(next_proposer) << endl;

            // ADDED THE FOLLOWING TO TEST ON DATA, where some proposers don't have full rankings.
            tuple<int, int, bool, int> hard_match_res = hard_match(next_proposer, proposer_preference, receiver_ranking);
            int hard_match_receiver = get<3>(hard_match_res);
            tuple<int, int, bool> hard_match_tuple = std::make_tuple(get<0>(hard_match_res), get<1>(hard_match_res), get<2>(hard_match_res));
            
            if (matching[hard_match_receiver].proposers.size() >= receiver_capacity[hard_match_receiver]) {
                cout << "Adding proposer " << PROPOSER_NAMES.at(get<1>(matching[hard_match_receiver].proposers.top()));
                cout << " back to free proposers queue" << endl;
                free_proposers.push(get<1>(matching[hard_match_receiver].proposers.top()));
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
// (Revisit this; this isn't proposer-optimal, and I think that's giving me a weird result.
//  Maybe I should consider the potential drop in compatibility that comes from the switch as well?)
//
// TO-DO: we should add a caveat that now, the match isn't stable! (i.e. no stable matching exists)
tuple<int, int, bool, int> hard_match(int proposer, vector<vector<int>> proposer_preference, vector<vector<int>> receiver_ranking) {
    vector<int> proposer_choices = proposer_preference[proposer];

    int best_receiver = -1;
    int best_compatibility = INT_MAX;

    for (int i = 0; i < proposer_choices.size(); i++) {
        int receiver = proposer_choices[i];
        if (receiver == -1) continue;
        if (receiver_ranking[receiver][proposer] == -1) continue;

        int compatibility = i + receiver_ranking[receiver][proposer];
        if (compatibility < best_compatibility) {
            best_compatibility = compatibility;
            best_receiver = receiver;
        }
    }

    if (best_receiver == -1) {
        cout << "hard_match: no valid receiver found for proposer "  << PROPOSER_NAMES.at(proposer) << endl;
    }

    // TO-DO: add a check that chosen receiver has non-fixed space. If not, we need to use the next-best option.
    cout << "Hard-matching proposer " << PROPOSER_NAMES.at(proposer) << " with receiver " << RECEIVER_NAMES.at(best_receiver) << endl;
    return std::make_tuple(receiver_ranking[best_receiver][proposer], proposer, true, best_receiver);
}

}

// const map<int, string> PROPOSER_NAMES = {{0, "lauren"}, {1, "chloe"}, {2, "kevin"}, {3, "kylie"}, {4, "varsha"}, {5, "cadence"}, {6, "katherine"}, {7, "trisha"}, {8, "hibah"}, {9, "katharina"}};
// const map<int, string> RECEIVER_NAMES = {{0, "harika"}, {1, "quinn s"}, {2, "lexi"}, {3, "jasmine"}, {4, "eliana"}, {5, "jordan"}, {6, "tshiring"}, {7, "jiayi"}, {8, "quinn a"}, {9, "stephanie"}};

// const map<int, string> PROPOSER_NAMES = {{0, "cailin"}, {1, "carson"}, {2, "eireann"}, {3, "eunice"}, {4, "karman"}, {5, "kisha"},
//                                          {6, "kruthi"}, {7, "lily"}, {8, "lindsay"}, {9, "manya"}, {10, "marie"}, {11, "namoir"},
//                                          {12, "natania"}, {13, "queenee"}, {14, "surya"}, {15, "tida"}, {16, "tiffany"},
//                                          {17, "vienna"}, {18, "will"}};
// const map<int, string> RECEIVER_NAMES = {{0, "cadence"}, {1, "clayton"}, {2, "cory"}, {3, "fernanda"}, {4, "irene"}, {5, "jada"},
//                                          {6, "katherine"}, {7, "naseem"}, {8, "raagini"}, {9, "shree"}, {10, "spencer"}, 
//                                          {11, "marisa"}, {12, "tania"}, {13, "yuna"}, {14, "varsha"}};

// const map<int, string> PROPOSER_NAMES = {{0, "maddie"}, {1, "aarna"}, {2, "shirley"}, {3, "taryn"}, {4, "pareesha"}, {5, "abby"},
//                                          {6, "audrey"}, {7, "electra"}, {8, "sadi"}, {9, "joyce"}, {10, "megan pereira"}, 
//                                          {11, "megan ly"}, {12, "natalie"}, {13, "ingrid"}, {14, "sophia"}, {15, "inderjeet"},
//                                          {16, "elise"}, {17, "maren"}, {18, "sara"}, {19, "naema"}, {20, "christine"},
//                                          {21, "aabritty"}, {22, "haimkiry"}, {23, "leonie"}};
// const map<int, string> RECEIVER_NAMES = {{0, "raagini"}, {1, "marie"}, {2, "tania"}, {3, "vienna"}, {4, "sam"}, {5, "kruthi"},
//                                          {6, "fernanda"}, {7, "marisa"}, {8, "spencer"}, {9, "nina"}, {10, "namoir"}, 
//                                          {11, "lindsay"}, {12, "natania"}, {13, "tiffany"}, {14, "emma"}, {15, "eunice"},
//                                          {16, "jasmine"}, {17, "naseem"}, {18, "katherine"}, {19, "lily"}};

int main(int argc, char* argv[]) {
    // TEST ALGORITHM
    vector<vector<int>> proposer_preference = {{7, 3, 13, 4, 11}, {11, 9, 13, 2, 1}, {6, 3, 13, 14, 15}, {15, 17, 18, 5, 2},
                                               {7, 18, 4, 5, 9}, {2, 14, 6, 4, 16}, {0, 13, 11, 17, 9}, {11, 9, 3, 12, 2},
                                               {14, 8, 13, 10, 9}, {3, 11, 13, 1, 17}, {2, 0, 14, 17, 18}, {15, 0, 16, 1, 12},
                                               {11, 4, 16, 15, 18}, {2, 4, 7, 0, 13}, {16, 15, 18, 7, 0}, {18, 10, 8, 16, 13},
                                               {6, 13, 7, 19, 0}, {0, 13, 1, 4, 16}, {13, 9, 3, 5, 16}, {9, 2, 7, 16, 18},
                                               {7, 13, 4, 0, 16}};
    
    vector<vector<int>> receiver_preference = {{6, 11, 17, 15, 7}, {3, 6, 9, 7, 2}, {13, 19, 21, 3, 5}, {13, 7, 22, 19, 2},
                                               {15, 19, 14, 22, 17}, {21, 3, 23, 4, 7}, {2, 21, 1, 19, 4}, {19, 0, 3, 9, 22},
                                               {23, 15, 8, 7, 3}, {12, 14, 22, 7, 6}, {8, 14, 11, 12, 1}, {18, 2, 7, 9, 12},
                                               {9, 13, 22, 7, 20}};

    vector<int> receiver_capacity = {2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

    vector<vector<int>> receiver_ranking = matching::preference_to_ranking(receiver_preference, 19, 15, 5);

    vector<matching::Match> matching = matching::algorithm(proposer_preference, receiver_ranking, receiver_capacity);

    for (auto m : matching) {
        cout << m.toString() << endl;
    }
}