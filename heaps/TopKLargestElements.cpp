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
 * Find the Top K largest elements
 * 
 * 1. Push all the elemenets in a max heap.
 * 2. Pop out K times from the max heap and return those elements.
 */
std::vector<int> topKLargestElements(vector<int>& nums, int k) {
    std::vector<int> results;
    std::priority_queue<Element> myQueue; // maxHeap

    for (auto num : nums) {
        myQueue.push(Element(num)); // O(N) heapify an array
    }

    for (int i = 0; i < k; i++) {
        results.push_back(myQueue.top()._element);
        myQueue.pop(); // bubble-down max heapify O(Log K)
    }

    return results;
        
}

int main() {
    std::vector<int> num = {9, 3, 7, 1, -2, 6, 8};
    auto result = topKLargestElements(num, 3);
    for (auto res: result) {
        std::cout << res << std::endl;
    }
}