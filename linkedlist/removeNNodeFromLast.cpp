#include <iostream>
#include <vector>

using namespace std;

struct ListNode
{
    int data;
    ListNode* next;
    ListNode(int _data) : data(_data), next(nullptr) {}
};

ListNode* removeNThNodeFromLast(ListNode* head, int n) {
    // Find the length of the linked list
    ListNode* curr = head;
    int length = 0;
    while (curr) {
        curr = curr->next;
        length++;
    }

    // Determine the index of the node to be removed
    if (length < n) {
        return head;
    }
    int indexToBeRemoved = length - n;
    
    // Remove the node
    ListNode* prev = nullptr;
    curr = head;
    int currentIndex = 0;
    while (curr) {
        if (currentIndex == indexToBeRemoved) {
            if (indexToBeRemoved) {
                prev->next = curr->next;
            } else {
                head = curr->next;
            }
            break;
        }
        currentIndex++;
        prev = curr;
        curr = curr->next;
    }
    return head;
}

int main() {
    ListNode* head = new ListNode(5);
    head->next = new ListNode(4);
    head->next->next = new ListNode(3);
    head->next->next->next = new ListNode(2);
    head->next->next->next->next = new ListNode(1);

    head = removeNThNodeFromLast(head, 2);
    while (head) {
        std::cout << head->data << std::endl;
        head = head->next;
    }
}