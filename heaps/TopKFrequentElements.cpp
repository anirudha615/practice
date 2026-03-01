#include <iostream>
using namespace std;

struct Element {
    int value;
    int frequency;
};

struct Cmp {
    bool operator()(const Element a, const Element b) {
        return a.frequency > b.frequency; // Ascending
    }
};

/**
 * Calculate the frequency and then use minHeap of size K
 */
vector<int> topKFrequent(vector<int>& nums, int k) {
    std::unordered_map<int, int> m_frequency;
    std::priority_queue<Element, std::vector<Element>, Cmp> minHeap;

    // O(N) - worst case-  map is O(N) with frequency 1
    for (auto num: nums) {
        m_frequency[num]++;
    }

    // O(N * LogK)
    for (std::pair<int, int> keyValue : m_frequency) {
        if (minHeap.size() < k) {
            minHeap.push({keyValue.first, keyValue.second});
        } else if (minHeap.top().frequency < keyValue.second) {
            minHeap.pop();
            minHeap.push({keyValue.first, keyValue.second});
        }
    }

    std::vector<int> results;
    while (!minHeap.empty()) {
        results.push_back(minHeap.top().value);
        minHeap.pop();
    }
    return results;
}

int main() {
    std::vector<int> nums {3, 1,1,1,2,2,3};
    int k = 2;
    vector<int> results = topKFrequent(nums, k);
    for (auto res : results) {
        std::cout << res << ", ";
    }
}