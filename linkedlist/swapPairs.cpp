#include <iostream>
#include <vector>

using namespace std;

struct ListNode
{
    int data;
    ListNode* next;
    ListNode(int _data) : data(_data), next(nullptr) {}
};

ListNode* swapPairs(ListNode* head) {
    if (!head || !head->next) {
        return head;
    }
    ListNode* prevPrev = nullptr;
    ListNode* prev = head;
    ListNode* curr = head->next;
    ListNode* finalHead = curr;
    // You would require to check prev and prev.next (curr) because without prev and curr, there can't be any swap. 
    while (prev && prev->next) {
        
        // Assign current
        curr = prev->next;

        // Assign next of current
        ListNode* next = curr->next; // Save the next of current in a temp
        
        /** Perform Swapping */
        /**  When you swap prev and curr, you need to current prev's prev to curr */
        if (prevPrev) {
            prevPrev->next = curr;
        }
        prev->next = next; // previous to current's next
        curr->next = prev; // current to previous

        /** Increment Pointers */
        prevPrev = prev;
        // since prev is now curr (after swapping), this is basically next of curr. It is okay to go forward with it as we are checking prev as null in while loop
        prev = prev->next; 
    }
    return finalHead;
}

int main() {
    ListNode* head = new ListNode(5);
    head->next = new ListNode(4);
    head->next->next = new ListNode(3);
    head->next->next->next = new ListNode(2);
    //head->next->next->next->next = new ListNode(1);

    head = swapPairs(head);
    while (head) {
        std::cout << head->data << std::endl;
        head = head->next;
    }
}