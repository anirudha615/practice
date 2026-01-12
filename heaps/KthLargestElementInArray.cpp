#include <iostream>

using namespace std;

struct Element
{
    int _element;
    Element(int element): _element(element) {}

    const bool operator<(const Element otherElement) const
    {
        return _element < otherElement._element;
    }
};

/**
 * Find the Kth largest element 
 * 
 * 1. Push the elemenets in a max heap.
 * 2. Pop out K-1 times from the max heap and return the top element.
 */
int findKthLargest(vector<int>& nums, int k) {
    std::priority_queue<Element> myQueue; // maxHeap

    for (auto num : nums) {
        myQueue.push(Element(num)); // O(N) heapify an array
    }

    // Remove all index except k-1 index
    for (int i = 0; i < k - 1; i++) {
        myQueue.pop(); // bubble-down max heapify O(KLogN)
    }

    return myQueue.top()._element; //O(1)
        
}

int main() {
    std::vector<int> num = {3, 2, 1, 5, 6, 4};
    std::cout << findKthLargest(num, 2) << std::endl;
}