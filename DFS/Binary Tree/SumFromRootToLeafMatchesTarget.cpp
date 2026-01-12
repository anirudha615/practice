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


bool pathSum(TreeNode* head, int target, std::vector<int>& currentPath, std::vector<std::vector<int>>& allPath) {
    if (!head) {
        // When we encounter a nullptr (child of leaf node) and target sent to it is 0, we have found a path.
        // when you return that target is found, the parent stack can take the responsibility of adding the current path to allPath
        return target == 0;
    }

    currentPath.push_back(head->val);
    int targetToBePassed = target - head->val;
    bool leftPathFound = pathSum(head->left, targetToBePassed, currentPath, allPath);
    bool rightPathFound = pathSum(head->right, targetToBePassed, currentPath, allPath);
    /**
     * When the node's recursion stack is over, 
     *   1. Revaluate if either of left or right subtree found a path.
     *   2. If either of subtree found a path and target passed from 'LEAF' node to 'nullptr' children is 0, path is found
     *   3. Add the currentPath to allPaths.
     *   4. Since the data is already recorded, lets remove current node from currentPath 
     *      so that we can evaluate others siblings or other section of the tree which could potentially be a new path.
     */
    bool eitherPathFound = leftPathFound || rightPathFound;
    if (eitherPathFound && !targetToBePassed && !head->left && !head->right) {
         allPath.push_back(currentPath);
    }
    if (!currentPath.empty()) {
        currentPath.pop_back();
    }
    return eitherPathFound;
}

std::vector<std::vector<int>> collectPaths(TreeNode* head, int target) {
    std::vector<int> currentPath;
    std::vector<std::vector<int>> allPaths;
    pathSum(head, target, currentPath, allPaths);
    return allPaths;
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

    auto allPaths = collectPaths(head, 22);

    for (auto path : allPaths) {
        for (auto node_path : path) {
            std::cout << node_path << ", " << std::endl;
        }
        std::cout << std::endl;
    }
}