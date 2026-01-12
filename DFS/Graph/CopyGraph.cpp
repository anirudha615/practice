#include <iostream>
#include <vector>

using namespace std;

struct IntGraphNode {
    int value;
    vector<IntGraphNode*> neighbors;
    IntGraphNode(int _data) : value(_data) {}
};

void dfs_helper(std::unordered_map<int, vector<int>>& adjacency_list, std::unordered_map<IntGraphNode*, int>& visited_node, IntGraphNode* node) {
    if (!node || visited_node[node]) {
        return;
    }

    // Mark them as visited
    visited_node[node] = 1;

    // Put them into adjacency list
    std::vector<int> edges;
    for (auto edge : node->neighbors) {
        edges.push_back(edge->value);
    }
    adjacency_list[node->value] = edges;

    // Visit the edges via recursion
    for (auto edge : node->neighbors) {
        dfs_helper(adjacency_list, visited_node, edge);
    }
}

unordered_map<int, vector<int>> copy_graph(IntGraphNode* node) {
    std::unordered_map<int, vector<int>> adjacency_list;
    std::unordered_map<IntGraphNode*, int> visited_node;
    dfs_helper(adjacency_list, visited_node, node);

    for (auto nodes : adjacency_list) {
        vector<int> edges = nodes.second;
        for (auto edge : edges) {
            std::cout << nodes.first << " has an edge to " << edge << std::endl;
        }
    }
    return adjacency_list;
}

int main() {
    IntGraphNode* node1 = new IntGraphNode(1);
    IntGraphNode* node2 = new IntGraphNode(2);
    IntGraphNode* node3 = new IntGraphNode(3);
    IntGraphNode* node4 = new IntGraphNode(4);
    node1->neighbors.push_back(node2);
    node1->neighbors.push_back(node4);

    node2->neighbors.push_back(node1);
    node2->neighbors.push_back(node3);

    node3->neighbors.push_back(node2);
    node3->neighbors.push_back(node4);

    node4->neighbors.push_back(node1);
    node4->neighbors.push_back(node3);

    copy_graph(node1);
}