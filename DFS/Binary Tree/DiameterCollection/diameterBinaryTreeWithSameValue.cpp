#include <iostream>
#include <vector>

using namespace std;

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int _data) : val(_data), left(nullptr), right(nullptr) {}
};

int findDiameterForNodesWithSameValue(TreeNode* head, int& maxDiameter) {
    if (!head) {
        return 0; // When we encounter the child nullptr of a leaf node, return 0 as that node will have no edges.
    }

    int leftEdge = findDiameterForNodesWithSameValue(head->left, maxDiameter);
    int rightEdge = findDiameterForNodesWithSameValue(head->right, maxDiameter);
    /**
     * when the recursion of that node is over, we want to calculate the maximum edges it has including itself 
     * so that its parent can utilize this data in the form of 'total edges from child'.
     * 
     * Additionally, we need to calculate the diameter of a node (which is basically the distance between left and right edge)
     * 
     * Both of the above has a similar condition - they need to be calculated only when 'Any of the children value matches the node value'.
     * 
     *  a. If it does not or its a leaf node, maxEdgesOfNodeIncludingItself should be 1 (which is for that node itself) so that its parent can use.
     *  b. Similarly, for distanceBetweenLeftAndRightEdge - If none of the children matches the node, 
     *      there is no point in calculating the diameter and mark it as 0.
     */
    int maxEdgesOfNodeIncludingItself = 1;
    int distanceBetweenLeftAndRightEdge = 0;
    if (head->left && head->left->val == head->val) {
        distanceBetweenLeftAndRightEdge += leftEdge;
        maxEdgesOfNodeIncludingItself = leftEdge + 1;
    }
    if (head->right && head->right->val == head->val) {
        distanceBetweenLeftAndRightEdge += rightEdge;
        maxEdgesOfNodeIncludingItself = rightEdge + 1;
    }
    if (head->left && head->left->val == head->val && head->right && head->right->val == head->val) {
        maxEdgesOfNodeIncludingItself = std::max(leftEdge, rightEdge) + 1;
    }
    maxDiameter = std::max(maxDiameter, distanceBetweenLeftAndRightEdge);    
    return maxEdgesOfNodeIncludingItself;
}

int findDiameterForNodesWithSameValue(TreeNode* head) {
    int maxDiameter = 0;
    findDiameterForNodesWithSameValue(head, maxDiameter);
    return maxDiameter;
}

int main() {
    TreeNode* head = new TreeNode(5);
    head->left = new TreeNode(4);
    head->right = new TreeNode(5);
    head->left->left = new TreeNode(1);
    head->left->right = new TreeNode(1);
    head->right->right = new TreeNode(5);

    std::cout << findDiameterForNodesWithSameValue(head) << std::endl;
}