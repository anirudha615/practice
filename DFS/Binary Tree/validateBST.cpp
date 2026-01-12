#include <iostream>
#include <vector>

using namespace std;

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int _data) : val(_data), left(nullptr), right(nullptr) {}
};

/**
 * For BST, we need to follow left < root < right.
 * 1. To assess every node, we need to make sure that node is smaller/greater than its parent node
 *    and smaller/greater than all of its ancestor.
 * 2. However, validating against all of the ancestors is cumbersome, hence we will use minVal and maxValue
 *    as 'tightest lower bound from ALL ancestors (including parent)' and 'tightest upper bound from ALL ancestors (including parent)'
 * 3. As a result, you would only need to assess if a node is > minVal and node is < maxVal.
 */
bool validateBST(TreeNode* head, long min, long max) {
    if (!head) {
        return true; // When we encounter the child nullptr of a leaf node, return true indicating it suffice BST.
    }

    // We check if the node falls within the range. 
    // If it does not fall within the range, there is no point in traversing the children as the current node does not
    // satisfy BST. If it does, traverse the children.
    if (head->val <= min || head->val >= max) {
        return false;
    }

    // Since the current node satisfied the BST, lets dive deeper into their children
    bool leftSatisfiedBST = validateBST(head->left, min, head->val);
    bool rightSatisfiedBST = validateBST(head->right, head->val, max);
    // When the recursion stack of the node is over, check whether the current node satisfied the BST property and so do its chilren.
    // We came till this section knowing that current node satisfied the BST property, hence we will only check their children satisfaction.
    bool bothLeftRightSatisfiedBST = leftSatisfiedBST && rightSatisfiedBST;
    return bothLeftRightSatisfiedBST;
}

bool validateBST(TreeNode* head) {
    return validateBST(head, LONG_MIN, LONG_MAX);
}

int main() {
    TreeNode* head = new TreeNode(4);
    head->left = new TreeNode(1);
    head->right = new TreeNode(5);
    head->right->left = new TreeNode(3);
    head->right->right = new TreeNode(6);

    std::cout << validateBST(head) << std::endl;
}