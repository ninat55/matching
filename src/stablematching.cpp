#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <queue>

using std::string, std::ifstream, std::cout, std::cerr, std::endl, std::map, std::vector, std::queue;

// TO-DO: fix this
#define NUM_PROPOSERS 5
#define NUM_RECEIVERS 5

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

// int read(string filename) {
//     ifstream contents(filename);
//     string name;
//     map<string, vector<string>> rankings;

//     if (!contents) {
//         cerr << "Unable to read file" << endl;

//     }

//     while (contents >> name) {

//         if (name == "/") {
//             continue; // finished with one line
//         }
//     }

//     contents.close();
// }

void algorithm(vector<vector<int>> proposer_rankings, vector<vector<int>> receiver_rankings) {
    // TO-DO: INITIALIZE TO -1
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
        
        cout << "\nStaging proposer " << next_proposer << endl;

        int next_proposer_pref = proposer_rankings[next_proposer][next_proposal[next_proposer]];

        cout << "Proposer " << next_proposer << "'s next preference is receiver " << next_proposer_pref << endl;
        next_proposal[next_proposer]++;

        if (receiver_pairings[next_proposer_pref] == -1) { // tentative receiver is unmatched
            cout << "Receiver " << next_proposer_pref << " is unmatched; matching with proposer " << next_proposer << endl;
            receiver_pairings[next_proposer_pref] = next_proposer;
            proposer_pairings[next_proposer] = next_proposer_pref;
        } else { // note the strictly greater than! revisit later
            if (receiver_rankings[next_proposer_pref][next_proposer] < 
                receiver_rankings[next_proposer_pref][receiver_pairings[next_proposer_pref]]) {
                
                cout << "Switching receiver " << next_proposer_pref << " from proposer " << receiver_pairings[next_proposer_pref];
                cout << " to proposer " << next_proposer << endl;

                // unmatch prior proposer, add to free proposers queue
                cout << "Adding proposer " << receiver_pairings[next_proposer_pref] << " to queue" << endl;

                free_proposers.push(receiver_pairings[next_proposer_pref]);
                proposer_pairings[receiver_pairings[next_proposer_pref]] = -1;

                // replace w/ current proposer
                receiver_pairings[next_proposer_pref] = next_proposer;
                proposer_pairings[next_proposer] = next_proposer_pref;
            } else {
                // next_proposer is rejected :( requeue
                // TO-DO: repeat until next_proposer is assigned!
                cout << "Couldn't find a match for proposer " << next_proposer << " - adding back to queue" << endl;

                free_proposers.push(next_proposer);
            }
        }
    }
}