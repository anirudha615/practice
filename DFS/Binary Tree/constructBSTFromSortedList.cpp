#include <iostream>
using namespace std;

struct ListNode {
    int val;
    ListNode *next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}
};

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
};

int getMedian(std::vector<int>& sortedList) {
    return (!sortedList.size() % 2) ? sortedList.size()/2 : (sortedList.size()-1)/2;
}

void constructBSTRecursively(TreeNode* root, std::vector<int> leftSubArray, std::vector<int> rightSubArray) {
    if (leftSubArray.empty() && rightSubArray.empty()) {
        return;
    }
    
    if (leftSubArray.size() == 1) {
        root->left = new TreeNode(leftSubArray.at(0));
    }

    if (rightSubArray.size() == 1) {
        root->right = new TreeNode(rightSubArray.at(0));
    }

    if (leftSubArray.size() > 1) {
        int leftSubArrayRootIndex = getMedian(leftSubArray);
        root->left = new TreeNode(leftSubArray.at(leftSubArrayRootIndex));
        std::vector<int> newleftSubArray(leftSubArray.begin(), leftSubArray.begin() + leftSubArrayRootIndex); 
        std::vector<int> newrightSubArray(leftSubArray.begin() + leftSubArrayRootIndex + 1, leftSubArray.end());
        constructBSTRecursively(root->left, newleftSubArray, newrightSubArray);
    }
    

    if (rightSubArray.size() > 1) {
        int rightSubArrayRootIndex = getMedian(rightSubArray);
        root->right = new TreeNode(rightSubArray.at(rightSubArrayRootIndex));
        std::vector<int> newleftSubArray(rightSubArray.begin(), rightSubArray.begin() + rightSubArrayRootIndex);
        std::vector<int> newrightSubArray(rightSubArray.begin() + rightSubArrayRootIndex + 1, rightSubArray.end()); 
        constructBSTRecursively(root->right, newleftSubArray, newrightSubArray);
    }
}

/**
 * Logic - 
 *   1. Convert linked list into vector list
 *   2. Find the median of the list which will become the root
 *   3. Create left/right subarray and pass to constructBSTRecursively
 *       a. Which will repeat step 2 for left/right subarray
 *       b. create another smaller left/right subarray
 * 
 * NOTE: Recursion ending stack is - if array is empty or have size == 1.
 * 
 * https://www.hellointerview.com/community/questions/convert-sorted-list/cm5eh7nrh04q1838o7r19jaed?level=STAFF
 */
TreeNode* sortedListToBST(ListNode* head) {
    if (!head) {
        return nullptr;
    }

    std::vector<int> sortedList;
    while (head) {
        sortedList.push_back(head->val);
        head = head->next;
    }

    int rootIndex = getMedian(sortedList);
    TreeNode* root = new TreeNode(sortedList.at(rootIndex));
    // inclusive of first param, exclusive of second param
    std::vector<int> leftSubArray(sortedList.begin(), sortedList.begin() + rootIndex); 
    std::vector<int> rightSubArray(sortedList.begin() + rootIndex + 1, sortedList.end());
    constructBSTRecursively(root, leftSubArray, rightSubArray);
    return root;
}

// Inorder traversal
void printBST(TreeNode* root) {
    if (!root) {
        return;
    }
    printBST(root->left);
    std::cout << root->val << std::endl;
    printBST(root->right);
}

int main() {
    ListNode* head = new ListNode(-10);
    head->next = new ListNode(-3);
    head->next->next = new ListNode(0);
    head->next->next->next = new ListNode(5);
    head->next->next->next->next = new ListNode(9);

    TreeNode* BSThead = sortedListToBST(head);
    printBST(BSThead);
}