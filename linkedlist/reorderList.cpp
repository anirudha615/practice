#include <iostream>
#include <vector>

using namespace std;

struct ListNode
{
    int data;
    ListNode* next;
    ListNode(int _data) : data(_data), next(nullptr) {}
};

/**
 * Used a O(N) space complexity to convert linked list into vector
 * and then used the approach of two pointer to reorder the list
 */
void reorderList(ListNode*& head) {
    std::vector<int> list;
    ListNode* curr = head;
    while (curr) {
        list.push_back(curr->data);
        curr = curr->next;
    }

    ListNode* finalHead = new ListNode(-1);
    int left = 0; int right = list.size() - 1;
    ListNode* finalModifiedHead = finalHead;
    while (left < right) {
        finalModifiedHead->next = new ListNode(list.at(left));
        finalModifiedHead->next->next = new ListNode(list.at(right));
        ++left;
        --right;
        finalModifiedHead = finalModifiedHead->next->next;
    }
    if (left == right) {
        finalModifiedHead->next = new ListNode(list.at(left));
    }
    head = finalHead->next;
}

int main() {
    ListNode* head = new ListNode(5);
    head->next = new ListNode(4);
    head->next->next = new ListNode(3);
    head->next->next->next = new ListNode(2);
    head->next->next->next->next = new ListNode(1);
    head->next->next->next->next->next = new ListNode(7);

    reorderList(head);
    while (head) {
        std::cout << head->data << std::endl;
        head = head->next;
    }
}