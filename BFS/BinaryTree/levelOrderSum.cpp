#include <iostream>
#include <algorithm>
#include <queue>

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

std::vector<int> calculateLevelOrderSum(TreeNode* head) {
    std::vector<int> levelOrderSum;
    if (!head) {
        return levelOrderSum;
    }
    // Step 1: Push the first node to the queue
    std::queue<TreeNode*> bfs_queue;
    bfs_queue.push(head);
    
    // Step 4: Do while the queue is empty
    while (!bfs_queue.empty()) {
        int levelSum = 0;
        int levelSize = bfs_queue.size();
        
        // Step 2: Extract all the nodes from the current level and assess it. 
        for (int level = 0; level < levelSize; level++) {
            TreeNode* node = bfs_queue.front();
            bfs_queue.pop();
            levelSum += node->val;

            // Step 3: Push your children to the queue
            if (node->left) {
                bfs_queue.push(node->left);
            }
            if (node->right) {
                bfs_queue.push(node->right);
            }
        }

        // For the question, collect the level sum once the assessment of all nodes in the current level is completed.
        levelOrderSum.push_back(levelSum);
    }
    return levelOrderSum;
}

int main() {
    TreeNode* head = new TreeNode(1);
    head->left = new TreeNode(3);
    head->right = new TreeNode(4);
    head->left->right = new TreeNode(2);
    head->left->right->left = new TreeNode(8);
    head->right->left = new TreeNode(7);

    std::vector<int> levelOrderSum = calculateLevelOrderSum(head);
    for (int level : levelOrderSum) {
        std::cout << level << std::endl;
    }

}
