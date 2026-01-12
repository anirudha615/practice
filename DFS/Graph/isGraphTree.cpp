#include <iostream>
#include <vector>

using namespace std;

bool dfs_helper(std::unordered_map<int, vector<int>>& adjacency_list, std::unordered_map<int, int>& visited_node, int node, int parent) {
    if (visited_node.count(node)) {
        // We have detected a cycle
        return false;
    }

    // Mark them as visited
    visited_node[node] = parent;

    // Traverse the connected components via recursion
    for (auto edge : adjacency_list[node]) {
        // Since you came from your parent. If your neighbour is also a parent (contract of bidrectional graph, don't explore)
        // Another Advantage - If you land in a visited node, you found a cycle
        if (edge != parent) {
            bool cycle_detection = dfs_helper(adjacency_list, visited_node, edge, node);
            if (!cycle_detection) {
                return false;
            }
        }
    }
    return true;
}

unordered_map<int, vector<int>> buildAdjList(int totalNodes, vector<vector<int>>& edges) {
    unordered_map<int, vector<int>> adjList;
    
    for (int i = 0; i < totalNodes; i++) {
        adjList[i] = vector<int>();
    }

    for (const auto& edge : edges) {
        int u = edge[0];
        int v = edge[1];
        adjList[u].push_back(v);
        adjList[v].push_back(u);
    }
        
    return adjList;
}

bool isGraphTree(int totalNodes, std::vector<std::vector<int>>& edges) {
    unordered_map<int, vector<int>> adjacency_list = buildAdjList(totalNodes, edges);
    std::unordered_map<int, int> visited_node;
    return dfs_helper(adjacency_list, visited_node, 0, -1) && visited_node.size() == totalNodes;
}

int main() {
    std::vector<std::vector<int>> edges = {{0, 1}, {1, 2}, {2, 0}};
    std::cout << isGraphTree(5, edges) << std::endl;
}