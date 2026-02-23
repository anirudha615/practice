#include <iostream>

using namespace std;

struct Element {
    int _element;
    Element(int element): _element(element) {}

    const bool operator<(const Element otherElement) const {
        return _element > otherElement._element;
    }
};

struct ListNode {
    int val;
    ListNode* next;
    ListNode(int _val) : val(_val), next(nullptr) {}
};

struct ElementOptimized {
    ListNode* _node;
    ElementOptimized(ListNode* node): _node(node) {}

    const bool operator<(const ElementOptimized otherNode) const {
        return _node->val > otherNode._node->val;
    }
};

/**
 * To merge 2 sorted list, we take help of 2 pointers and a dummy head.
 * However, to merge K Sorted lists, we can't have K pointers compare among themselves.
 *    a. Hence, we put them in a min heap and construct the final linked list.
 */
ListNode* mergeKSortedListsBruteForce(std::vector<ListNode*> sortedLists) {
    std::priority_queue<Element> minHeap;

    //O(K) for building heap
    for (ListNode* head : sortedLists) {
        while (head) {
            minHeap.push(head->val);
            head = head->next;
        }
    }

    // Extracting all elements which is O(NLogN)
    ListNode* finalHead = new ListNode(INT16_MAX);
    ListNode* finalModifiedHead = finalHead;
    while (!minHeap.empty()) {
        finalModifiedHead->next = new ListNode(minHeap.top()._element);
        finalModifiedHead = finalModifiedHead->next;
        minHeap.pop();
    }

    return finalHead->next;
}

ListNode* mergeKSortedListsOptimized(std::vector<ListNode*> sortedLists) {
    std::priority_queue<ElementOptimized> minHeap;

    /**
     * Similar to two pointers used in merging 2 linked links, only compare the first element of all lists.
     * Hence, we will only put the head of the sorted lists.
     * 
     * This will be O(K) . Brute Force had O(N) as we are putting all the elements into minHeap.
     */
    for (ListNode* head : sortedLists) {
        if (head) {
            minHeap.push(head);
        }
    }

    /**
     * Extract the top element and put its value in the node.
     * The Top element will have a head, increment the head to its next pointer and push that into minHeap.
     * Similar to Two pointers, where if left wins, we do left++ and right remains on the same spot.
     * 
     * We will do Pop and Push in the same while loop.
     * 
     * Since the loop assess every element, the loop will run O(N) times
     * and we pop and push only the head of sorted list which is equal to K and hence the operation is O(LogK)
     * 
     * Total: O(N * LogK)
     */
    ListNode* finalHead = new ListNode(INT16_MAX);
    ListNode* finalModifiedHead = finalHead;
    while (!minHeap.empty()) {
        ListNode* minListHead = minHeap.top()._node;
        minHeap.pop(); // O(Log K) since we only added the head of all sorted lists and not individual elements

        // Add to dummy node
        finalModifiedHead->next = new ListNode(minListHead->val);
        finalModifiedHead = finalModifiedHead->next;

        // increment the head extracted
        if (minListHead->next) {
            minHeap.push(minListHead->next); // O(LogK) pushing the next pointer which will be equal to the number of sorted lists and not individual elements.
        }
    }

    return finalHead->next;
}

struct ListElement {
    int value;
    int currentIndex;
    int listIndex;
};

struct Cmp {
    bool operator()(const ListElement a, const ListElement b) {
        return a.value > b.value; // Ascending
    }
};

std::vector<int> mergeKSortedListWithoutPointer(std::vector<std::vector<int>> sortedLists) {
    std::priority_queue<ListElement, std::vector<ListElement>, Cmp> minHeap;
    std::vector<int> result;

    for (int index = 0; index < sortedLists.size(); index++) {
        minHeap.push({sortedLists.at(index).at(0), 0, index});
    }

    while (!minHeap.empty()) {
        ListElement frontElement = minHeap.top();
        minHeap.pop();
        result.push_back(frontElement.value);

        std::vector<int> sortedList = sortedLists.at(frontElement.listIndex);
        if (frontElement.currentIndex + 1 < sortedList.size()) {
            minHeap.push({sortedList.at(frontElement.currentIndex + 1), frontElement.currentIndex + 1, frontElement.listIndex});
        }
    }
    return result;
}

int main() {
    ListNode* head = new ListNode(1);
    head->next = new ListNode(4);
    head->next->next = new ListNode(5);

    ListNode* headTwo = new ListNode(1);
    headTwo->next = new ListNode(3);
    headTwo->next->next = new ListNode(4);

    ListNode* headThree = new ListNode(2);
    headThree->next = new ListNode(6);

    std::vector<ListNode*> sortedLists = {head, headTwo, headThree};
    head = mergeKSortedListsOptimized(sortedLists);
    while (head) {
        std::cout << head->val << std::endl;
        head = head->next;
    }

    std::vector<int> result = mergeKSortedListWithoutPointer({{3,4,6},{2,3,5},{-1,6}});
    for (auto coordinate : result) {
        std::cout << coordinate << ", " << std::endl;
    }
}