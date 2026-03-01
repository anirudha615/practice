#include <iostream>
using namespace std;

 struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

void DFSHelper(TreeNode* node, vector<int>& results, string pattern) {
    //base condition to exit
    if (!node) {
        return;
    }

    if (pattern == "Inorder") {
        DFSHelper(node->left, results, pattern); // LEFT
        results.push_back(node->val); // capture the node
        DFSHelper(node->right, results, pattern); // RIGHT
    } else if (pattern == "Preorder") {
        results.push_back(node->val); // capture the node
        DFSHelper(node->left, results, pattern); // LEFT
        DFSHelper(node->right, results, pattern); // RIGHT
    } else if (pattern == "Postorder") {
        DFSHelper(node->left, results, pattern); // LEFT
        DFSHelper(node->right, results, pattern); // RIGHT
        results.push_back(node->val); // capture the node
    }
    
}

vector<int> inorderTraversal(TreeNode* root) {
    std::vector<int> results;
    DFSHelper(root, results, "Inorder");
    return results;
}

vector<int> preorderTraversal(TreeNode* root) {
    std::vector<int> results;
    DFSHelper(root, results, "Preorder");
    return results;
}

vector<int> postrderTraversal(TreeNode* root) {
    std::vector<int> results;
    DFSHelper(root, results, "Postorder");
    return results;
}

int main() {
    // Test tree:     1
    //              /   \
    //             2     3
    //            / \
    //           4   5
    TreeNode* root = new TreeNode(1);
    root->left = new TreeNode(2);
    root->right = new TreeNode(3);
    root->left->left = new TreeNode(4);
    root->left->right = new TreeNode(5);
    
    // Test 1: Inorder → [4,2,5,1,3]
    vector<int> inorder = inorderTraversal(root);
    cout << "Inorder: ";
    for (int val : inorder) cout << val << " ";
    cout << endl;
    
    // Test 2: Preorder → [1,2,4,5,3] (FIX: add results.push_back(node->val) in Preorder!)
    vector<int> preorder = preorderTraversal(root);
    cout << "Preorder: ";
    for (int val : preorder) cout << val << " ";
    cout << endl;
    
    // Test 3: Postorder → [4,5,2,3,1]
    vector<int> postorder = postrderTraversal(root);  // Note: typo in function name
    cout << "Postorder: ";
    for (int val : postorder) cout << val << " ";
    cout << endl;
    
    return 0;
}