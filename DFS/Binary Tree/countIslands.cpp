#include <iostream>
#include <vector>

using namespace std;

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int _data) : val(_data), left(nullptr), right(nullptr) {}
};

void dfsHelper(TreeNode* node, int& numberOfIslands, int parentValue) {
    if (!node) {
        return; //termination case
    }

    // evaluate islands - if the child value changes , we are starting off with a new island
    if (parentValue != node->val) {
        ++numberOfIslands;
    }

    // explore children but need no return value from them as we passing the counter by reference.
    dfsHelper(node->left, numberOfIslands, node->val);
    dfsHelper(node->right, numberOfIslands, node->val);
}

int countIslands(TreeNode* rootNode) {
    if (!rootNode) {
        return 0;
    }
    if (!rootNode->left && !rootNode->right) {
        return 1;
    }
    int numberOfIslands = 1; // root exists and one or both of their children exists
    dfsHelper(rootNode, numberOfIslands, rootNode->val);
    return numberOfIslands;
}

int main() {
    TreeNode* head = new TreeNode(1);
    head->left = new TreeNode(1);
    head->right = new TreeNode(0);
    head->left->left = new TreeNode(1);
    head->left->right = new TreeNode(1);
    head->right->right = new TreeNode(0);

    std::cout << countIslands(head) << std::endl;
}