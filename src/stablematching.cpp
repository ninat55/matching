#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <queue>
#include <sstream>

using std::string, std::ifstream, std::cout, std::cerr, std::endl, std::map, std::vector, std::queue, std::stringstream;

// TO-DO: fix this
#define NUM_PROPOSERS 5
#define NUM_RECEIVERS 5
#define NUM_CHOICES 5

const map<int, string> PROPOSER_NAMES = {{0, "nina"}, {1, "sam"}, {2, "fernanda"}, {3, "marisa"}, {4, "jasmine"}, {5, "shree"}, {6, "caleb"}};
const map<int, string> RECEIVER_NAMES = {{0, "jada"}, {1, "clayton"}, {2, "kylie"}, {3, "lexi"}, {4, "quinn"}};

void algorithm(vector<vector<int>> proposer_rankings, vector<vector<int>> receiver_rankings);

int main(int argc, char* argv[]) {
    // TEST ALGORITHM
    vector<vector<int>> proposer_rankings = {{1, 3, 0, 4, 2}, {0, 3, 4, 2, 1}, {2, 1, 4, 0, 3}, {4, 3, 1, 2, 0}, {2, 1, 0, 3, 4}};
    vector<vector<int>> receiver_rankings = {{4, 0, 3, 2, 1}, {3, 4, 0, 2, 1}, {1, 2, 0, 4, 3}, {4, 3, 0, 1, 2}, {2, 4, 3, 0, 1}};

    // IMPORTANT:
    // proposer_rankings[p][r] gives proposer p's r'th preference
    // receiver_rankings[r][p] gives receiver r's ranking for proposer p

    algorithm(proposer_rankings, receiver_rankings);
}

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

void algorithm(vector<vector<int>> proposer_rankings, vector<vector<int>> receiver_rankings) {
    vector<int> proposer_pairings; // proposer_pairings[i] gives proposer i's tentative receiver
    vector<int> receiver_pairings; // receiver_pairings[j] gives receiver j's tentative proposer

    queue<int> free_proposers;

    vector<int> next_proposal; // next_proposal[k] gives index in proposer_rankings for 
                               // proposer k's highest not-yet-proposed-to preference

    for (int i = 0; i < NUM_PROPOSERS; i++) {
        free_proposers.push(i);
        proposer_pairings.push_back(-1);
        next_proposal.push_back(0);
    }

    for (int i = 0; i < NUM_RECEIVERS; i++) {
        receiver_pairings.push_back(-1);
    }

    while (!free_proposers.empty()) {
        int next_proposer = free_proposers.front();
        free_proposers.pop();
        
        cout << "\nStaging proposer " << PROPOSER_NAMES.at(next_proposer) << endl;

        if (next_proposal[next_proposer] >= NUM_CHOICES) {
            // should never happen! throw some sort of warning message
            cout << "Warning! Unable to match proposer " << PROPOSER_NAMES.at(next_proposer) << endl;
            continue;
        }

        int next_proposer_pref = proposer_rankings[next_proposer][next_proposal[next_proposer]];

        cout << "Proposer " << PROPOSER_NAMES.at(next_proposer) << "'s next preference is receiver ";
        cout << RECEIVER_NAMES.at(next_proposer_pref) << endl;

        next_proposal[next_proposer]++;

        if (receiver_pairings[next_proposer_pref] == -1) { // tentative receiver is unmatched
            cout << "Receiver " << RECEIVER_NAMES.at(next_proposer_pref) << " is unmatched; matching with proposer ";
            cout << PROPOSER_NAMES.at(next_proposer) << endl;

            receiver_pairings[next_proposer_pref] = next_proposer;
            proposer_pairings[next_proposer] = next_proposer_pref;
        } else { // note the strictly greater than! revisit later
            if (receiver_rankings[next_proposer_pref][next_proposer] < 
                receiver_rankings[next_proposer_pref][receiver_pairings[next_proposer_pref]]) {
                
                cout << "Switching receiver " << RECEIVER_NAMES.at(next_proposer_pref) << " from proposer ";
                cout << PROPOSER_NAMES.at(receiver_pairings[next_proposer_pref]) << " to proposer ";
                cout << PROPOSER_NAMES.at(next_proposer) << endl;

                // unmatch prior proposer, add to free proposers queue
                cout << "Adding proposer " << PROPOSER_NAMES.at(receiver_pairings[next_proposer_pref]) << " to queue" << endl;

                free_proposers.push(receiver_pairings[next_proposer_pref]);
                proposer_pairings[receiver_pairings[next_proposer_pref]] = -1;

                // replace w/ current proposer
                receiver_pairings[next_proposer_pref] = next_proposer;
                proposer_pairings[next_proposer] = next_proposer_pref;
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