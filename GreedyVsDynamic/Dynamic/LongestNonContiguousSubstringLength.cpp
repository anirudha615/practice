#include <iostream>
#include <unordered_map>
#include <cmath>
using namespace std;

/**
 * Since it is not contiguous, multiple branches of subarray can be formed.
 * So, we need to remember the number of branches that formed at every index.
 * 
 * At a current index, check all the previous indexes and see if previous index < current index,
 *  1. If it is, add 1 (current index) to the number of branches collected in previous index.
 *  2. Remember, while we are adding 1 to the number of branches collected in previous index, we need the maxium length. 
 *     Hence, we will add 1 to the max length of the branch - DP[current Index] = max(DP[previous indexes]) +1
 */
int longestSubArrayNotContiguous(vector<int>& nums) {
    std::vector<int> dp (nums.size(), 0);
    int maxPreviousBranchLength = 1;

    // Base case
    dp[0] = 1;
    for (int i = 1; i < nums.size(); i++) {
        int maxPreviousBranchLengthForCurrentIndex = -1;
        for (int j = 0; j < i; j++) {
            // Found a previous index which is stricly less than current index
            // find the max length of the branch from the previous indexes
            if (nums[j] < nums[i]) {
                maxPreviousBranchLengthForCurrentIndex = std::max(maxPreviousBranchLengthForCurrentIndex, dp[j]);
            }
        }
        // If we found the maxPreviousBranchLengthForCurrentIndex 
        if (maxPreviousBranchLengthForCurrentIndex != -1) {
            dp[i] = maxPreviousBranchLengthForCurrentIndex + 1;
            maxPreviousBranchLength = std::max(dp[i], maxPreviousBranchLength);
        } else {
            // If we didn't find maxPreviousBranchLengthForCurrentIndex that means current index is smaller or equal to previous index, 
            // in that case, just have 1 as branch length
            dp[i] = 1;
        }
    }

    return maxPreviousBranchLength;
}


int main() {
    vector<int> houses {1,3,6,7,9,4,10,5,6};
    std::cout << longestSubArrayNotContiguous(houses) << std::endl;
}

