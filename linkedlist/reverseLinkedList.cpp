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
 * Using prev, curr and next to reverse the linked list.
 * 1. Keep next as placeholder for the next current for the next loop
 * 2. Keep curr as prev for the next loop
 */
ListNode* reverseLinkedList(ListNode* head) {
    ListNode* prev = nullptr;
    ListNode* curr = head;
    while (curr) {
        ListNode* next = curr->next; // store the next in a temp variable which will become next current.
        curr->next = prev;
        prev = curr;
        curr = next;
    }
    return prev;
}

int main()
{
    ListNode* head = new ListNode(1);
    head->next = new ListNode(2);
    head->next->next = new ListNode(3);
    head->next->next->next = new ListNode(4);
    head->next->next->next->next = new ListNode(5);

    head = reverseLinkedList(head);
    while (head) {
        std::cout << head->data << std::endl;
        head = head->next;
    }
}