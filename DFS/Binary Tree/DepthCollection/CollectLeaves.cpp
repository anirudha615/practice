#include <iostream>
#include <vector>

using namespace std;

struct TreeNode
{
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int _data) : val(_data), left(nullptr), right(nullptr) {}
};

/**
 * Use the same concept as Depth of binary tree - pass height from leaves to the parents.
 *   1. We will use 'current node height' == max of left subtree and right subtree height as 
 *      the index to insert into the final result
 *   2. Once the recursion of the node is over, we will send height to be used by parents (current node height + 1)
 * 
 * https://www.hellointerview.com/community/questions/binary-tree-leaves/cm5eh7nri04x6838o1f4f8f5b?company=Oracle&level=MID_LEVEL
 */
int dfsHelper(TreeNode* node, std::vector<std::vector<int>>& results) {
    if (!node) {
        return 0;
    }

    int leftHeight = dfsHelper(node->left, results);
    int rightHeight = dfsHelper(node->right, results);

    int currentNodeHeight = std::max(leftHeight, rightHeight);
    if (results.size() < currentNodeHeight + 1) {
        results.resize(currentNodeHeight + 1);
    }
    results[currentNodeHeight].push_back(node->val);

    int heightReturnedToParents = currentNodeHeight + 1; // 1 for the current node
    return heightReturnedToParents;
}

vector<vector<int>> findLeaves(TreeNode* root) {
    std::vector<std::vector<int>> results;
    dfsHelper(root, results);
    return results;
}