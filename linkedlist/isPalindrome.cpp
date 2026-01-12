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
 * Approach A/B:
 *   1. Loop through the linked list and store the data in a vector.
 *   2. Once you have a list, apply 2 pointer. IF both pointers are same, increment/decrement them. Else break and return false.
 *   3. (OR) Once you have a list, reverse the list (O(N)) and compare it with original list
 * 
 * Approach C:
 *   1. Find the middle node of the linked list and reverse the second half and then match with the first half.
 */
bool isPalindromeApproachB(ListNode* head)
{
    /** just 1 node */
    if (!head)
    {
        return true;
    }

    std::vector<int> myVector;
    while (head) {
        myVector.push_back(head->data);
        head = head->next;
    }
    auto myVectorReverse = myVector;
    std::reverse(myVectorReverse.begin(), myVectorReverse.end());
    return myVectorReverse == myVector;
}

bool isPalindromeApproachA(ListNode* head)
{
    /** just 1 node */
    if (!head) {
        return true;
    }
    std::vector<int> myVector;
    while (head) {
        myVector.push_back(head->data);
        head = head->next;
    }

    int left = 0; int right = myVector.size() - 1;
    while (left < right) {
        if (myVector.at(left) != myVector.at(right)) {
            return false;
        } else {
            ++left;
            --right;
        }
    }

    return true;
}

int main()
{
    ListNode* head = new ListNode(1);
    head->next = new ListNode(2);
    head->next->next = new ListNode(3);
    head->next->next->next = new ListNode(4);
    head->next->next->next->next = new ListNode(0);

    std::cout << "ani " << isPalindromeApproachA(head) << std::endl;
}