#include <iostream>
#include <unordered_map>
#include <cmath>
using namespace std;

struct Element {
    int _distance;
    int _index;
    Element(int distance, int index): _distance(distance) , _index(index) {}

    const bool operator<(const Element otherElement) const {
        return _distance > otherElement._distance;
    }
};

/**
 * Find the K closest element to a target value.
 * 
 * 1. Find the distance between the element to the target value using abs(element - targetValue)
 * 2. Push the distance and index to the min heap while maintaining a map of distance to the element or index of the element in the array.
 * 3. Pop the first K elements from the min heap and return the results
 */
std::vector<int> findClosestElements(vector<int> nums, int k, int target) {
    std::priority_queue<Element> minHeap;
    std::vector<int> results;

    // O(N) for building a minHeap
    for (int i = 0; i < nums.size(); i++) {
        int distance = abs(nums.at(i) - target);
        minHeap.push({distance, i});
    }

    // O(KLogN) for Heapify when popping
    for (int i = 0; i < k; i++) {
        results.push_back(nums.at(minHeap.top()._index));
        minHeap.pop();
    }

    return results;
}

int main() {
    vector<int> arr = {1,2,5,5,6,6,7,7,8,9};
    auto res = findClosestElements(arr, 7, 7);
    
    for (const auto coordinate : res) {
        std::cout << coordinate << std::endl;
    }
}

