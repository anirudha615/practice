#include <iostream>
#include <map>
#include <vector>
#include <unordered_map>
#include <algorithm>
using namespace std;


/**
 * https://leetcode.com/problems/adjacent-increasing-subarrays-detection-ii/
 */
int maxIncreasingSubarrays(vector<int>& nums) {
    if (nums.size() <= 1) {
        return 0;
    }
    // collect adjacent contiguous subarray length
    std::vector<int> adjacentIncreasingContiguousSubArrayLength;
    int left = 0;
    int prev = 0;
    for (int right = 1; right < nums.size(); right++) {
        // window invalid
        if (nums.at(prev) >= nums.at(right)) {
            adjacentIncreasingContiguousSubArrayLength.push_back(prev - left + 1);
            left = right;
        }
        prev = right;
    }
    adjacentIncreasingContiguousSubArrayLength.push_back(prev - left + 1);

    // process adjacent contiguous subarray
    if (adjacentIncreasingContiguousSubArrayLength.size() == 1) {
        return adjacentIncreasingContiguousSubArrayLength.at(0)/2;
    }

    int maxLength = 0;
    for (int index = 1; index < adjacentIncreasingContiguousSubArrayLength.size(); index++) {
        // STEP 1: Since they are adjacent, compare the length between current and previous index using std::min
        // STEP 2: Since we require adjacent, we can also break the length into 2 of any of the index - So take the largest one and divide by 2.
        // STEP 3: Take the max of STEP 1 and STEP 2 --> length variable
        int length = std::max(
            std::min(adjacentIncreasingContiguousSubArrayLength.at(index-1), adjacentIncreasingContiguousSubArrayLength.at(index)), 
            std::max(adjacentIncreasingContiguousSubArrayLength.at(index-1), adjacentIncreasingContiguousSubArrayLength.at(index))/2
            );
        maxLength = std::max(maxLength, length);
    }

    return maxLength;
}

int main() {
    std::vector<int> input1 {2,5,7,8,9,2,3,4,3,1};
    std::cout << "Input1 Result is : " << maxIncreasingSubarrays(input1) << std::endl;
    std::vector<int> input2 {1,2,3,4,4,4,4,5,6,7};
    std::cout << "Input2 Result is : " << maxIncreasingSubarrays(input2) << std::endl;
}
