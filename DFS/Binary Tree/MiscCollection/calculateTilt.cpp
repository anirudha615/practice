#include <iostream>
#include <vector>

using namespace std;

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int _data) : val(_data), left(nullptr), right(nullptr) {}
};

int calculateTilt(TreeNode* head, int& tilt) {
    if (!head) {
        return 0; // When we encounter the child nullptr of a leaf node, return 0 as sum.
    }

    int leftSum = calculateTilt(head->left, tilt);
    int rightSum = calculateTilt(head->right, tilt);
    int tiltOfNode = std::abs(rightSum - leftSum);
    // When the recursion stack of the node is over, we will calculate tilt by substracting sum of left - sum of right.
    // Additionally, we will also calculate the sum of that node (which contains the sum of left and right subtre)
    // so that the parent of the current node can use the sum to deduce their tilt.
    tilt += tiltOfNode;
    int sumOfNode = leftSum + rightSum + head->val;
    return sumOfNode;
}

int calculateTilt(TreeNode* head) {
    int tilt = 0;
    calculateTilt(head, tilt);
    return tilt;
}

int main() {
    TreeNode* head = new TreeNode(4);
    head->left = new TreeNode(2);
    head->right = new TreeNode(7);
    head->left->left = new TreeNode(1);
    head->left->right = new TreeNode(3);
    head->right->left = new TreeNode(6);
    head->right->right = new TreeNode(9);

    std::cout << calculateTilt(head) << std::endl;
}