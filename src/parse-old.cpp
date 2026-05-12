#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <vector>

using std::string, std::ofstream, std::map, std::vector, std::cerr, std::endl;

// proposer_preference[p][r] gives proposer p's r'th preference (index into receivers)
// receiver_preference[r][p] gives receiver r's p'th preference (index into proposers)
// Both are ordered most-preferred first.

namespace matching {

void write_csv(string path, const map<int, string>& names, const map<int, string>& pref_names,
               const vector<vector<int>>& preference) {
    ofstream f(path);
    if (!f) {
        cerr << "Unable to write file " << path << endl;
        return;
    }

    for (int i = 0; i < (int)names.size(); i++) {
        f << names.at(i);
        if (i < (int)preference.size()) {
            for (int choice : preference[i]) {
                f << "," << pref_names.at(choice);
            }
        }
        f << "\n";
    }
}

void parse(const map<int, string>& proposer_names, const map<int, string>& receiver_names,
           const vector<vector<int>>& proposer_preference, const vector<vector<int>>& receiver_preference,
           string proposer_out, string receiver_out) {
    write_csv(proposer_out, proposer_names, receiver_names, proposer_preference);
    write_csv(receiver_out, receiver_names, proposer_names, receiver_preference);
}

} // namespace matching

int main() {
    const map<int, string> PROPOSER_NAMES = {{0, "taryn"}, {1, "ingrid"}, {2, "electra"}, {3, "maddie"}, {4, "megan pereira"},
                                         {5, "aarna"}, {6, "natalie"}, {7, "megan ly"}, {8, "sophia"}, {9, "audrey"}, 
                                         {10, "pareesha"}, {11, "haimkiry"}, {12, "maren"}, {13, "joyce"}, {14, "sadi"},
                                         {15, "elise"}, {16, "abby"}, {17, "shirley"}, {18, "christine"}, {19, "naema"}, 
                                         {20, "rudra"}, {21, "inderjeet"}};
    const map<int, string> RECEIVER_NAMES = {{0, "namoir"}, {1, "lily"}, {2, "marisa"}, {3, "raagini"}, {4, "kruthi"}, {5, "nina"},
                                         {6, "jasmine"}, {7, "fernanda"}, {8, "sam"}, {9, "natania"}, {10, "lindsay"},
                                         {11, "vienna"}, {12, "spencer"}, {13, "tiffany"}, {14, "tania"}, {15, "eunice"},
                                         {16, "katherine"}, {17, "naseem"}, {18, "marie"}, {19, "kisha"}, {20, "emma"}};

    vector<vector<int>> proposer_preference = {{15, 14, 3, 2, 4}, {13, 3, 6, 8, 2}, {11, 6, 16, 14, 13}, {5, 6, 8, 16, 3},
                                               {3, 15, 16, 17, 6}, {18, 10, 8, 17, 19}, {17, 18, 16, 14, 10}, {6, 9, 18, 20, 3},
                                               {8, 2, 16, 20, 3}, {18, 11, 5, 4, 15}, {8, 2, 7, 16, 5}, {11, 12, 8, 2, 0},
                                               {17, 13, 6, 7, 20}, {18, 8, 13, 17, 1}, {12, 17, 13, 18, 7}, {2, 13, 16, 7, 8},
                                               {17, 7, 20, 2, 11}, {11, 13, 20}, {8, 9, 18, 3, 17}};
    
    vector<vector<int>> receiver_preference = {{15, 8, 19, 9, 18}, {1, 0, 6, 9, 17}, {8, 10, 0, 6, 15}, {15, 13, 8, 1, 2},
                                               {9, 4, 18, 3, 6}, {8, 9, 15, 2, 5}, {3, 1, 19, 13, 2}, {10, 19, 8, 1, 9},
                                               {11, 10, 5, 16, 1}, {13, 18, 6, 19, 1}, {1, 10, 11, 6, 19}, {2, 11, 17, 1, 0},
                                               {14, 10, 15, 13, 19}, {1, 15, 12, 10, 17}, {19, 6, 10, 17, 11}};

    matching::parse(PROPOSER_NAMES, RECEIVER_NAMES, proposer_preference, receiver_preference,
                    "round_3_littles.csv", "round_3_bigs.csv");

    std::cout << "Wrote round_3_littles.csv and round_3_bigs.csv" << std::endl;
    return 0;
}