#include <iostream>
#include <unordered_set>
using namespace std;

vector<vector<int>> graph;

bool knows(int a, int b) {
    return graph[a][b] == 1;
}


/**
 * https://www.hellointerview.com/community/questions/leetcode-277-celebrity/cm5eh7nrh04up838ocrhvbicf?level=STAFF
 * 
 * 1. Calculate the Innode to indicate that everybody knows celebrity.
 * 2. Maintain an outnode (not a celebrity list) if a person knows somebody.
 * 
 * Logic -> Innode of celebrity == n-1 but outnode[celebrity] == 0
 */
int findCelebrity(int n) {        
    int totalRows = n;
    int totalColumns = n;
    std::unordered_map<int, int> in_node;
    std::unordered_set<int> notCelebrity;
    for (int row = 0; row < totalRows; row++) {
        for (int column = 0; column < totalColumns; column++) {
            if (row == column) {
                continue;
            }
            if (knows(row, column)) {
                // person 'row' knows person 'column' but person 'column' has known anybody yet
                in_node[column]++; 
                notCelebrity.insert(row);
            }
        }
    }

    std::vector<int> celebrity;
    for (std::pair<int, int> keyValue : in_node) {
        // Everybody knows celebrity but celebrity knows nobody
        if (keyValue.second == (n - 1) && !notCelebrity.count(keyValue.first)) {
            celebrity.push_back(keyValue.first);
        }
    }

    return celebrity.size() == 1 ? celebrity.at(0) : -1;
}

int main() {
    // Person 2 is the celebrity — nobody known by 2, everyone knows 2
    graph = {
        {0, 1, 1},  // person 0 knows person 1 and 2
        {0, 0, 1},  // person 1 knows person 2
        {0, 0, 0}   // person 2 knows nobody ← celebrity
    };

    int n = graph.size();
    cout << "Celebrity: " << findCelebrity(n) << endl;  // Expected: 2

    // No celebrity case
    graph = {
        {0, 1, 0},
        {0, 0, 1},
        {0, 1, 0}
    };
    n = graph.size();
    cout << "Celebrity: " << findCelebrity(n) << endl;  // Expected: -1

    return 0;
}