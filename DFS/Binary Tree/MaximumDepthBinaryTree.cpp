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

int maxDepth(TreeNode* head) {
    if (!head) {
        return 0; // if the node does not exist, height will be 0;
    }

    int leftHeight = maxDepth(head->left);
    int rightHeight = maxDepth(head->right);
    // return value of when the recursion stack of that node is over is max of (left, right) + height of the node itself (1)
    int totalHeightOfCurrentNode = std::max(leftHeight, rightHeight) + 1; 
    return totalHeightOfCurrentNode;
}

int main() {
    TreeNode* head = new TreeNode(4);
    head->left = new TreeNode(2);
    head->right = new TreeNode(7);
    head->left->left = new TreeNode(1);
    head->left->left->right = new TreeNode(8);
    head->right->left = new TreeNode(6);
    head->right->right = new TreeNode(9);

    std::cout << maxDepth(head) << std::endl;
}