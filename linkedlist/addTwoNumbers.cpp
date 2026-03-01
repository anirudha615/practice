#include <iostream>
#include <vector>

using namespace std;

struct ListNode
{
    int val;
    ListNode* next;
    ListNode(int _val) : val(_val), next(nullptr) {}
};

/**
 * https://leetcode.com/problems/add-two-numbers/description/
 */
ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
    ListNode* head = new ListNode(-1);
    ListNode* modifiedHead = head;

    int carryOver = 0;
    while (l1 && l2) {
        int sum = l1->val + l2->val + carryOver;
        modifiedHead->next = new ListNode(sum%10);
        modifiedHead = modifiedHead->next;
        carryOver = sum/10;
        l1 = l1->next;
        l2 = l2->next;
    }

    while (l2) {
        int sum = l2->val + carryOver;
        modifiedHead->next = new ListNode(sum%10);
        modifiedHead = modifiedHead->next;
        carryOver = sum/10;
        l2 = l2->next;
    }

    while (l1) {
        int sum = l1->val + carryOver;
        modifiedHead->next = new ListNode(sum%10);
        modifiedHead = modifiedHead->next;
        carryOver = sum/10;
        l1 = l1->next;
    }

    if (carryOver) {
        modifiedHead->next = new ListNode(carryOver);
    }

    return head->next;
}

int main() {
    ListNode* head1 = new ListNode(2);
    head1->next = new ListNode(4);
    head1->next->next = new ListNode(9);

    ListNode* head2 = new ListNode(5);
    head2->next = new ListNode(6);
    head2->next->next = new ListNode(4);
    head2->next->next->next = new ListNode(9);

    addTwoNumbers(head1, head2);

    ListNode* head = addTwoNumbers(head1, head2);
    while (head) {
        std::cout << head->val << ",";
        head = head->next;
    }
    
}