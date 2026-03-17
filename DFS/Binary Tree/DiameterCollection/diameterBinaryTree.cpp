#include <iostream>
#include <vector>

using namespace std;

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int _data) : val(_data), left(nullptr), right(nullptr) {}
};

int findDiameter(TreeNode* head, int& maxDiameter) {
    if (!head) {
        return 0; // When we encounter the child nullptr of a leaf node, return 0 as that node will have no edges.
    }

    int leftEdge = findDiameter(head->left, maxDiameter);
    int rightEdge = findDiameter(head->right, maxDiameter);
    // A diameter of a node is basically the distance between left and right edge.
    int distanceBetweenLeftAndRightEdge = leftEdge + rightEdge;
    maxDiameter = std::max(maxDiameter, distanceBetweenLeftAndRightEdge);
    // when the recursion of that node is over, we want to calculate the maximum edges it has including itself
    // so that its parent can utilize this data in the form of 'total edges from child'.
    int maxEdgesOfNodeIncludingItself = std::max(leftEdge, rightEdge) + 1;
    return maxEdgesOfNodeIncludingItself;
}

int findDiameter(TreeNode* head) {
    int maxDiameter = 0;
    findDiameter(head, maxDiameter);
    return maxDiameter;
}

int main() {
    TreeNode* head = new TreeNode(3);
    head->left = new TreeNode(9);
    head->right = new TreeNode(2);
    head->left->left = new TreeNode(1);
    head->left->right = new TreeNode(4);
    head->left->left->right = new TreeNode(5);

    std::cout << findDiameter(head) << std::endl;
}