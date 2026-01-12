#include <iostream>
#include <algorithm>
#include <queue>

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

/**
 * 1. Use a dequeue for inserting and popping from the back and front.
 * 2. Simple Algorithm - use a flag to maintain left to right at each level
 *      a. If it is left to right, pop from the front and push back the children to the back in the order left to right
 *      b. If it is right to left, pop from the back and push the children to the front in the order right to left
 *      c. Once the level is over, revert the flag.
 */
std::vector<std::vector<int>> zizZagTraversal(TreeNode* head) {
    std::vector<std::vector<int>> zizZagTraversalNodeList;
    if (!head) {
        return zizZagTraversalNodeList;
    }
    // Step 1: Push the first node to the queue
    std::deque<TreeNode*> bfs_queue;
    bfs_queue.push_front(head);
    bool leftToRightForCurrentLevel = true;
    
    // Step 4: Do while the queue is empty
    while (!bfs_queue.empty()) {
        int levelSize = bfs_queue.size();
        std::vector<int> zizZagTraversalNodes;
        
        // Step 2: Extract all the nodes from the current level and assess it. 
        for (int level = 0; level < levelSize; level++) {
            TreeNode* node;
            if (leftToRightForCurrentLevel) {
                node = bfs_queue.front();
                bfs_queue.pop_front();
            } else {
                node = bfs_queue.back();
                bfs_queue.pop_back();
            }
            zizZagTraversalNodes.push_back(node->val);

            // Step 3: Push children to Queue
            if (leftToRightForCurrentLevel) {
                // If it is left to right, push the children to the back of the queue in the order of left to right
                if (node->left) {
                    bfs_queue.push_back(node->left);
                }
                if (node->right) {
                    bfs_queue.push_back(node->right);
                }
            } else {
                // If it is right to left, push the children to the front of the queue in the order of right to left
                if (node->right) {
                    bfs_queue.push_front(node->right);
                }
                if (node->left) {
                    bfs_queue.push_front(node->left);
                }
                
            }
        }

        // Rever the flag
        leftToRightForCurrentLevel = !leftToRightForCurrentLevel;
        zizZagTraversalNodeList.push_back(zizZagTraversalNodes);
    }
    return zizZagTraversalNodeList;
}

int main() {
    TreeNode* head = new TreeNode(1);
    head->left = new TreeNode(3);
    head->right = new TreeNode(4);
    head->left->right = new TreeNode(2);
    head->left->right->left = new TreeNode(8);
    head->right->left = new TreeNode(7);

    std::vector<std::vector<int>> nodesList = zizZagTraversal(head);
    for (std::vector<int> nodes : nodesList) {
        for (int node : nodes) {
            std::cout << node << std::endl;
        }
    }
}
