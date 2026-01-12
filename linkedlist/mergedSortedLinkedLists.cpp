#include <iostream>
#include <vector>

using namespace std;

struct ListNode
{
    int data;
    ListNode* next;
    ListNode(int _data) : data(_data), next(nullptr) {}
};

ListNode* mergeSortedLinkedList(ListNode* headOne, ListNode* headTwo) {
    ListNode* finalHead = new ListNode(-1);
    ListNode* finalModifiedHead = finalHead;
    while (headOne || headTwo) {
        if ((headOne && !headTwo) || (headOne->data < headTwo->data)) {
            finalModifiedHead->next = headOne;
            headOne = headOne->next;
       } else if ((!headOne && headTwo) || (headOne->data >= headTwo->data)) {
            finalModifiedHead->next = headTwo;
            headTwo = headTwo->next;
       }
       finalModifiedHead = finalModifiedHead->next;
    }
    return finalHead->next;
}

int main()
{
    ListNode* headOne = new ListNode(1);
    headOne->next = new ListNode(4);
    headOne->next->next = new ListNode(7);

    ListNode* headTwo = new ListNode(2);
    headTwo->next = new ListNode(5);
    headTwo->next->next = new ListNode(6);

    ListNode* head = mergeSortedLinkedList(headOne, headTwo);
    while (head) {
        std::cout << head->data << std::endl;
        head = head->next;
    }
}