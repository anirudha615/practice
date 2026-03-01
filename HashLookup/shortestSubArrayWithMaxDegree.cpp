#include <iostream>
using namespace std;

int findShortestSubArray(vector<int>& nums) {
    // Maps to store: frequency, first occurrence index, last occurrence index
    unordered_map<int, int> count, first, last;

    // Single pass: populate all three maps
    for (int i = 0; i < nums.size(); i++) {
        int num = nums[i];
        count[num]++;
        if (first.find(num) == first.end())
            first[num] = i;   // Record first occurrence only once
        last[num] = i;        // Always update last occurrence
    }

    // Find the degree (max frequency)
    int degree = 0;
    for (auto& [num, freq] : count)
        degree = max(degree, freq);

    // Among elements matching the degree, find smallest subarray span
    int result = nums.size(); // Worst case: entire array
    for (auto& [num, freq] : count) {
        if (freq == degree) {
            // Subarray from first[num] to last[num] has the same degree
            result = min(result, last[num] - first[num] + 1);
        }
    }

    return result;
}

int main() {
    // Example 1
    vector<int> nums1 = {1, 2, 2, 3, 1};
    cout << findShortestSubArray(nums1) << endl; // Output: 2

    // Example 2
    vector<int> nums2 = {1, 2, 2, 3, 1, 4, 2};
    cout << findShortestSubArray(nums2) << endl; // Output: 6

    return 0;
}
