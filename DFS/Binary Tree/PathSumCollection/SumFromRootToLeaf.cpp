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
 * https://www.hellointerview.com/community/questions/sum-root-leaf-numbers/cm5eh7nrh04ql838o5ungj2h0?level=MID_LEVEL
 * 
 * 1. Update total sum when you encounter the leaves
 * 2. Keep updating the path (by value) so that when you recuse back, you don't have to remove old sibling
 */
void dfsHelper(TreeNode* node, int& totalSum, string path) {
    if (!node) {
        return;
    }
    path += std::to_string(node->val);
    dfsHelper(node->left, totalSum, path);
    dfsHelper(node->right, totalSum, path);

    // Exit strategy for leave node - update totalSum
    if (!node->left && !node->right) {
        totalSum += std::stoi(path);
    }
}

int sumNumbers(TreeNode* root) {
    int totalSum = 0;
    string path = "";
    dfsHelper(root, totalSum, path);
    return totalSum;
}

int main() {
    TreeNode* head = new TreeNode(5);
    head->left = new TreeNode(4);
    head->left->left = new TreeNode(11);
    head->left->left->left = new TreeNode(7);
    head->left->left->right = new TreeNode(2);
    head->right = new TreeNode(8);
    head->right->left = new TreeNode(9);
    head->right->right = new TreeNode(4);
    head->right->right->right = new TreeNode(1);

    std::cout << sumNumbers(head) << std::endl;
}