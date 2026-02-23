#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
using namespace std;

struct NodeDistance {
    int nodeToBePopped;
    int distance;
};

struct Cmp {
    bool operator()(const NodeDistance a, const NodeDistance b) {
        return a.distance > b.distance; // Ascending;
    }
};

int networkDelayTime(vector<vector<int>> connections, int startingServer, int lastServer) {
    std::unordered_map<int, std::vector<std::pair<int, int>>> adjacencyGraph;
    for (vector<int> connection : connections) {
        adjacencyGraph[connection.at(0)].push_back({connection.at(1), connection.at(2)});
    }

    std::vector<int> distance (lastServer+1, INT_MAX);
    distance[startingServer] = 0;
    std::unordered_set<int> visitedNodes;
    std::priority_queue<NodeDistance, std::vector<NodeDistance>, Cmp> minHeap;
    minHeap.push({startingServer, 0});

    while (!minHeap.empty()) {
        NodeDistance frontElement = minHeap.top();
        minHeap.pop();

        // Mark the node as processed
        visitedNodes.insert(frontElement.nodeToBePopped);

        // Assess every neighbor and calculate if the distance to the neighbor from the current node is 
        // less than the current distance of that neighbor (distance[neighbor]) from any other node. If yes, update it.
        for (std::pair<int, int> neighborWithWeight : adjacencyGraph[frontElement.nodeToBePopped]) {
            // Don't explore the neighbor which is already processed.
            if (visitedNodes.count(neighborWithWeight.first)) {
                continue;
            }
            int distanceToNeighbor = distance[frontElement.nodeToBePopped] + neighborWithWeight.second;
            if (distanceToNeighbor < distance[neighborWithWeight.first]) {
                distance[neighborWithWeight.first] = distanceToNeighbor;
                minHeap.push({neighborWithWeight.first, distanceToNeighbor});
            }
        }
    }

    // Compute maxTime to reach all the servers. If the distance to any server is still INT_MAX, then return -1.
    int maxTime = 0;
    for (int i = 1; i <= lastServer; ++i) {
        if (distance[i] == INT_MAX) return -1;
        maxTime = std::max(maxTime, distance[i]);
    }
    return maxTime;
}

int main() {
    vector<vector<int>> connections = {{1,2,10}, {1,3,1}, {3,2,1}};
    std::cout << networkDelayTime(connections, 1, 3) << std::endl;
}