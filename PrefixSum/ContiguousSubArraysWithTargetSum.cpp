#include <iostream>
#include <map>
using namespace std;

/**
 * Contiguous Sub-arrays with target sum
 * 
 * 1. We will create a Map of <PrefixSum>:<Index>
 * 2. Similar to Fixed/Sliding window concepty, at every index, we will calculate RunningSum and 
 *     then subtract RunningSum - targetSum and check if that value is present in the above map.
 * 3. If it is present, we found a left boundary window and then we will process the fixed window
 * 
 */
void getContiguousSubArraysWithTargetsum(const std::vector<int>& array, int targetSum) {
    std::vector<std::vector<int>> results;
    int maxSubArraySize = INT_MIN;
    std::unordered_map<int, std::vector<int>> prefixToIndexMap;
    int prefixSum = 0;
    prefixToIndexMap[prefixSum].push_back(-1);

    // Create a map of prefix Sum to Index - You can do the same in the below loop. Readability
    for (int i = 0; i < array.size(); i++) {
        prefixSum += array.at(i);
        prefixToIndexMap[prefixSum].push_back(i);
    }

    int runningSum = 0;
    for (int rightWindowBoundaryIndex = 0; rightWindowBoundaryIndex < array.size(); rightWindowBoundaryIndex++) {
        runningSum += array.at(rightWindowBoundaryIndex);

        // Now we have prefixToIndexMap, we will recalculate prefixSum at every index and subtract with targetSum. 
        // If the result is present in prefixToIndexMap, that result is literally left boundary of subarray window whose sum is equal to targetSum.
        // Add the left boundary + 1, current index (right boundary) to the list of windows.
        int leftWindowBoundary = runningSum - targetSum;
        if (prefixToIndexMap.count(leftWindowBoundary)) {
            // A window has been found whose sum of the elements is equal to targetSum
            std::vector<int> indexOfLeftBoundaryList = prefixToIndexMap[leftWindowBoundary];
            for (auto index : indexOfLeftBoundaryList) {
                if (index + 1 <= rightWindowBoundaryIndex) {
                    results.push_back({index + 1, rightWindowBoundaryIndex}); // this is capturing the range
                    maxSubArraySize = std::max(maxSubArraySize, rightWindowBoundaryIndex - index);
                }
            }   
        } else {
            // No window was found continue
        }
    }

    std::cout << "total subarray : " << results.size() << std::endl;
    for (std::vector<int> res : results) {
        for (int index = res.at(0); index <= res.at(1); index++) {
            std::cout << array.at(index) << ", ";
        }
        std::cout << std::endl;
    }

    std::cout << "longest subarray of size : " << maxSubArraySize << std::endl;
    
}


int main ()
{
    std::vector<int> array {-1,-1,1};
    getContiguousSubArraysWithTargetsum(array, 0);
}

/**
 * Story - 
 *  1. A coach has a season of 9 games in the NFL.
 *  2. Each game has 4 quarters with the following data - {6, 15, -24, 6}
 *  3. The coach wants to analyze the season and find out the set of adjacent games with a total combined net win of 7.alignas
 *  4. Below is the data for the season -
 *         [
 *              {6, 15, -24, 6},
 *              {4, 8, -8, 0},
 *              {13, 16, -9, -13},
 *              {14, -12, 14, -14},
 *              {13, 10, 5, -31},
 *              {15, 19, -15, -18},
 *              {14, 18, -15, -13},
 *              {14, -12, -1, 1},
 *              {1, 0, -1, 1}
 * 
 *         ]
 *         3, 4, 7, 2, -3, 1, 4, 2, 1
 */


 /**
  * Test data - 
  * 
  * 1. Input:
        nums = [0, -3, 1, 2, 5, -5, 3]
        target_sum = 3

        Expected Output:
        [
            [0, -3, 1, 2, 5, -5, 3],  # sums to 3
            [3],                      # single element match
            [1, 2],                   # sums to 3
            [5, -5, 3]                # sums to 3
        ]

    2. Input:
        nums = [4, -2, -1, 1, 6]
        game = [
                {2, 0, 2, 0},
                {-3, 1, 0, 1},
                {3, -4, 0, 0},
                {-3, 4, 0, 0},
                {2, 2, 2, 0}
                ]
        
        target_sum = 4

        Expected Output:
        [
            [4],                # single element match
            [4, -2, -1, 1, 6]   # whole array sums to 4
        ]
  *     
  */