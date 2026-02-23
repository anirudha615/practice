#include <iostream>

using namespace std;

/**
 * Find the Kth largest element - 
 *   1. Take a minHeap of size K and keep on pushing item till it reaches its size.
 *   2. Only pop the top if there is any integer > top as we want largest.
 * 
 * If we want smallest, take maxHeap
 */
int findKthLargest(vector<int> nums, int k) {
    std::priority_queue<int, std::vector<int>, std::greater<int>> minHeap;
    for (int data : nums) {
        if (minHeap.size() < k) {
            minHeap.push(data);
        } else if (data > minHeap.top()) {
            minHeap.pop();
            minHeap.push(data);
        }
    }
    return minHeap.top();

    }

int main() {
    std::vector<int> num = {3, 2, 1, 5, 6, 4};
    std::cout << findKthLargest(num, 2) << std::endl;
}