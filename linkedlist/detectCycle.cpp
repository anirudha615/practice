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
 * Fast moves twice. Slow moves once
 * 1. When Fast == slow, there is a cycle. This approach is simple to execute with a cost of 1 or 2 extra loops
 * 2. You can catch the cycle before where fast < slow but then you need to keep a visited node to make sure it has passed once.
 */
bool detectCycle(ListNode* head) {
    ListNode* fast = head;
    ListNode* slow = head;
    while (fast && fast->next) {
        fast = fast->next->next;
        slow = slow->next;
        if (fast == slow) {
            return true; // there is a cycle
        }
    }
    return false;
}

int main()
{
    ListNode* head = new ListNode(3);
    head->next = new ListNode(2);

    std::cout << detectCycle(head) << std::endl;
}