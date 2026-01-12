#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
using namespace std;

std::vector<int> canFinish(int numCourses, vector<vector<int>> prerequisites) {
    std::vector<int> ordered_list;
    std::queue<int> bfs_queue;
    std::vector<int> inboundEdges (numCourses, 0);
    std::unordered_map<int, std::vector<int>> adjacency_list;

    for (auto edge : prerequisites) {
        inboundEdges[edge.at(0)]++;
        adjacency_list[edge.at(1)].push_back(edge.at(0));
    }

    for (int index = 0; index < inboundEdges.size(); index++) {
        // This means that index is free and no one is dependent on it.
        if (!inboundEdges.at(index)) {
            bfs_queue.push(index);
        }
    }

    while (!bfs_queue.empty()) {
        int levelSize = bfs_queue.size();
        for (int i = 0; i < levelSize; i++) {
            int indexToBePopped = bfs_queue.front();
            bfs_queue.pop();

            // Add to topological ordered list
            ordered_list.push_back(indexToBePopped);

            // Explore the neighbors
            for (auto neighbor: adjacency_list[indexToBePopped]) {
                // Simulating the behavior of removing indexToBePopped from list so that it is not dependent on its neighbors
                inboundEdges[neighbor]--;
                // Check if the neighbor is free and no one is dependent on it.
                if (!inboundEdges.at(neighbor)) {
                    bfs_queue.push(neighbor);
                }
            }
        }
    }

    return ordered_list.size() == numCourses ? ordered_list : std::vector<int>();
}


int main() {
    vector<vector<int>> prerequisites = {{1, 0}, {2, 0}, {3, 1}, {3, 2}};
    std::vector<int> ordered_list = canFinish(4, prerequisites);
    for (auto course : ordered_list) {
        std::cout << course << std::endl;
    }
}