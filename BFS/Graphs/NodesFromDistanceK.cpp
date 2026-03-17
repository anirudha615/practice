#include <iostream>
#include <unordered_set>
using namespace std;

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

void dfsTraverse(TreeNode* node, TreeNode* parent, std::unordered_map<int, TreeNode*>& parentState) {
    if (!node) {
        return;
    }
    if (parent) {
        parentState[node->val] = parent;
    }
    dfsTraverse(node->left, node, parentState);
    dfsTraverse(node->right, node, parentState);
}

/**
 * https://www.hellointerview.com/community/questions/binary-tree-distance-k/cm5eguhac02od838oslnlfl37?level=SENIOR
 * 
 * Since this is a distance problem, use BFS (This is a graph problem as compared to tree problem)
 * 1. Create a parent map first.
 * 2. Push the target node first and then explore left, right and parent.
 * 3. Once the level == k, all the nodes in the queue are correct answers.
 */
vector<int> distanceK(TreeNode* root, TreeNode* target, int k) {
    if (!root) {
        return {};
    }
    // state
    std::vector<int> results;
    std::unordered_map<int, TreeNode*> parentState;
    std::queue<TreeNode*> bfs;
    std::unordered_set<int> visited_node;
    int level = 0;

    // Collect Node->Parent mapping
    dfsTraverse(root, nullptr, parentState);
    
    // Push target node
    bfs.push(target);
    visited_node.insert(target->val);

    while (!bfs.empty()) {
        // Stopping at level K
        if (level == k) {
            results.push_back(bfs.front()->val);
            bfs.pop();
            continue;
        }

        // Process the level
        int levelSize = bfs.size();
        for (int index = 0; index < levelSize; index++) {
            TreeNode* node = bfs.front();
            bfs.pop();

            // Explore parent and children
            if (parentState.count(node->val) && !visited_node.count(parentState[node->val]->val)) {
                visited_node.insert(parentState[node->val]->val);
                bfs.push(parentState[node->val]);
            }

            if (node->left && !visited_node.count(node->left->val)) {
                visited_node.insert(node->left->val);
                bfs.push(node->left);
            }

            if (node->right && !visited_node.count(node->right->val)) {
                visited_node.insert(node->right->val);
                bfs.push(node->right);
            }
        }
        level++;
    }

    return results;
}

int main() {
    // Tree:        1
    //            /   \
    //           2     3
    //          / \   / \
    //         4   5 6   7
    // target = 5, k = 2
    // Expected output: 4, 1 (in any order)

    TreeNode* root = new TreeNode(1);
    root->left = new TreeNode(2);
    root->right = new TreeNode(3);
    root->left->left = new TreeNode(4);
    root->left->right = new TreeNode(5);
    root->right->left = new TreeNode(6);
    root->right->right = new TreeNode(7);

    TreeNode* target = root->left->right;  // node 5
    int k = 2;

    vector<int> result = distanceK(root, target, k);

    cout << "Nodes at distance " << k << " from node " << target->val << ": ";
    for (int val : result) cout << val << " ";
    cout << endl;  // Expected: 4 1

    return 0;
}