#include <iostream>
#include <algorithm>
#include <queue>

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

int maximumWidthBinaryTree(TreeNode* head) {
    int maxWidth = 0;
    if (!head) {
        return maxWidth;
    }

    // Step 1: Push the first node to the queue
    std::queue<std::pair<TreeNode*, int>> bfs_queue;
    bfs_queue.push({head, 0});
    
    // Step 4: Do while the queue is empty
    while (!bfs_queue.empty()) {
        int levelSize = bfs_queue.size();
        int indexFirstNodeOfLevel = 0;
        int indexLastNodeOfLevel = 0;
        int minIndexAtCurrentLevel = bfs_queue.front().second;
        
        // Step 2: Extract all the nodes from the current level and assess it. 
        for (int i = 0; i < levelSize; i++) {
            std::pair<TreeNode*, int> node = bfs_queue.front();
            bfs_queue.pop();

            // We need to normalize the index by substracting the index at the current level - minimum index of the current level
            int normalizedIndex = node.second - minIndexAtCurrentLevel;
            
            // Step 3: Push your children to the queue
            // Increment the index even if the children does not exist. This will help in maintaining the index of every node in the tree
            if (node.first->left) {
                int indexOfLeftChild = (2 * normalizedIndex) + 1;
                bfs_queue.push({node.first->left, indexOfLeftChild});
            }
            if (node.first->right) {
                int indexOfRightChild = (2 * normalizedIndex) + 2;
                bfs_queue.push({node.first->right, indexOfRightChild});
            }

            // Finding the first node of the current level
            if (i == 0) {
                indexFirstNodeOfLevel = normalizedIndex;
            }
            // Finding the last node of the current level
            if (i == levelSize - 1) {
                indexLastNodeOfLevel = normalizedIndex;
            }
        }

        // calculating the width at the current level
        int widthAtEachLevel = indexLastNodeOfLevel - indexFirstNodeOfLevel + 1;
        maxWidth = std::max(maxWidth, widthAtEachLevel);
    }
    return maxWidth;
}

int main() {
    TreeNode* head = new TreeNode(1);
    head->left = new TreeNode(3);
    head->left->left = new TreeNode(5);
    head->left->left->left = new TreeNode(6);
    head->right = new TreeNode(2);
    head->right->right = new TreeNode(9);
    head->right->right->left = new TreeNode(7);

    std::cout << maximumWidthBinaryTree(head) << std::endl;
}
