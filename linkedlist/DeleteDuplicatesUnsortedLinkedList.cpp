#include <iostream>
#include <vector>
#include <map>

using namespace std;

struct ListNode
{
    int val;
    ListNode* next;
    ListNode(int _val) : val(_val), next(nullptr) {}
};

/**
 * Used map for frequency counter and vector for insertion order.
 * 
 * We can eliminate the use of vector by using a 2-pointer approach
 */
ListNode* deleteDuplicatesUnsorted(ListNode* head) {
    std::unordered_map<int, int> state; // frequency
    std::vector<int> state2; // insertion order
    while (head) {
        state[head->val]++;
        state2.push_back(head->val);
        head = head->next;
    }

    ListNode* newHead = new ListNode(-1);
    ListNode* modifiedHead = newHead;
    for (int value: state2) {
        if (state[value] > 1) {
            continue;
        }
        modifiedHead->next = new ListNode(value);
        modifiedHead = modifiedHead->next;
    }
    return newHead->next;
}

int main() {
    ListNode* head = new ListNode(1);
    head->next = new ListNode(2);
    head->next->next = new ListNode(3);
    head->next->next->next = new ListNode(2);
    head->next->next->next->next = new ListNode(3);
    head->next->next->next->next->next = new ListNode(4);
    
    ListNode* result = deleteDuplicatesUnsorted(head);    
    while (result) {
        std::cout << result->val << ", ";
        result = result->next;
    }
    cout << endl;
}