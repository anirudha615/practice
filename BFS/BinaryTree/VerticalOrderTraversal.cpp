#include <iostream>
#include <algorithm>
#include <queue>
#include <map>

using namespace std;

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

/**
 * Puzzle Logic - 
 *   1. Keep column index as 0 for root. Do BFS with left's column Index = root - 1 and right's column Index = root + 1
 *   2. Group by ordered column index and return the result
 * 
 * https://www.hellointerview.com/community/questions/binary-tree-vertical-order/cm5eh7nrh04vq838orghkmycr?level=SENIOR
 */
vector<vector<int>> verticalOrder(TreeNode* root) {
    if (!root) {
        return {};
    }
    std::map<int, std::vector<int>> m_state; // Need ordered column index
    std::queue<std::pair<TreeNode*, int>> bfs_queue; // <Node, ColumnIndex>
    bfs_queue.push({root, 0});

    while (!bfs_queue.empty()) {
        TreeNode* node = bfs_queue.front().first;
        int columnIndex = bfs_queue.front().second;
        bfs_queue.pop();
        if (node->left) {
            bfs_queue.push({node->left, columnIndex-1});
        }
        if (node->right) {
            bfs_queue.push({node->right, columnIndex+1});
        }
        m_state[columnIndex].push_back(node->val);
    }

    vector<vector<int>> results;
    for (std::pair<int, std::vector<int>> groupByColumn : m_state) {
        results.push_back(groupByColumn.second);
    }

    return results;
}

int main() {
    TreeNode* head = new TreeNode(3);
    head->left = new TreeNode(9);
    head->right = new TreeNode(20);
    head->right->left = new TreeNode(15);
    head->right->right = new TreeNode(7);

    std::vector<std::vector<int>> results = verticalOrder(head);
    for (std::vector<int> res : results) {
        for (int element : res) {
            std::cout << element << ", ";
        }
        std::cout << std::endl;
    }
}