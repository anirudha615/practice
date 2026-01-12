#include <iostream>
#include <map>
using namespace std;

/**
 * Contiguous Sub-arrays with target sum
 * 
 * 1. We will loop through the array and calculate the prefix sum and store the mapping of prefix sum to the index list.
 * 2. While looping through the array, we will calculate prefix-targetSum and check if the value exists in the map. 
 * 3. If present, we will get the contiguous sub array from [index + 1, current index of the loop]
 */
std::vector<std::vector<int>> getContiguousSubArraysWithTargetsum(const std::vector<int>& array, int targetSum)
{
    if (array.empty())
    {
        return {};
    }

    std::vector<std::vector<int>> results;
    // This will map prefixSum to the index
    std::unordered_map<int, std::vector<int>> prefixSumMap;
    int prefixSum = 0;
    prefixSumMap[prefixSum].push_back(-1); // prefix Sum protocol

    for (int i = 0; i < array.size(); i++)
    {
        prefixSum = prefixSum + array.at(i);
        // append the prefixSumMap as you loop
        prefixSumMap[prefixSum].push_back(i);

        // find whether (prefixSum - targetSum) ~== `prefixSum exists in the prefixSumMap 
        // Get the list of indexes from the `prefixSum and the sub-array would be the [indexList of `prefixSum + 1, currentIndex Of loop]
        // The basic math here is prefixSum - `prefixSum -  ==> prefixSum of the current loop - [(prefixSum - targetSum) of the index found in the map] ==> targetSum
        auto indexList = prefixSumMap[prefixSum - targetSum];
        for (auto index : indexList)
        {
            std::vector<int> result (array.begin() + index + 1, array.begin() + i + 1); //[inclusive, exclusive)
            results.push_back(result);
        }
    }
    return results;
}

std::vector<std::vector<int>> getContiguousSubArraysWithTargetsumRev2(const std::vector<int>& array, int targetSum) {
    std::vector<std::vector<int>> results;
    std::unordered_map<int, std::vector<int>> prefixToIndexMap;
    int prefixSum = 0;
    prefixToIndexMap[prefixSum].push_back(-1);

    // Create a map of prefix Sum to Index - You can do the same in the below loop. Readability
    for (int i = 0; i < array.size(); i++) {
        prefixSum += array.at(i);
        prefixToIndexMap[prefixSum].push_back(i);
    }

    prefixSum = 0;
    for (int rightWindowBoundaryIndex = 0; rightWindowBoundaryIndex < array.size(); rightWindowBoundaryIndex++) {
        prefixSum += array.at(rightWindowBoundaryIndex);

        // Now we have prefixToIndexMap, we will recalculate prefixSum at every index and subtract with targetSum. 
        // If the result is present in prefixToIndexMap, that result is literally left boundary of subarray window whose sum is equal to targetSum.
        // Add the left boundary + 1, current index (right boundary) to the list of windows.
        int leftWindowBoundary = prefixSum - targetSum;
        if (prefixToIndexMap.count(leftWindowBoundary)) {
            // A window has been found whose sum of the elements is equal to targetSum
            std::vector<int> indexOfLeftBoundaryList = prefixToIndexMap[leftWindowBoundary];
            for (auto index : indexOfLeftBoundaryList) {
                if (index + 1 <= rightWindowBoundaryIndex) {
                    results.push_back({index + 1, rightWindowBoundaryIndex});
                }
            }   
        } else {
            // No window was found continue
        }
    } 

    return results;
}


int main ()
{
    std::vector<int> array {-1,-1,1};
    auto results = getContiguousSubArraysWithTargetsumRev2(array, 0);
    int longestSubArraySize = 0;
    std::vector<int> longestSubArray;
    for (auto result : results)
    {
        longestSubArraySize = max(longestSubArraySize, static_cast<int>(result.size()));
        if (longestSubArraySize == result.size())
        {
            longestSubArray = result;
        }
        for (auto res: result)
        {
            std::cout << res << " to " ;
        }
        std::cout << std::endl;
    }
    
    std::cout << "longest subarray of size : " << longestSubArraySize << std::endl;
    for (auto res: longestSubArray)
    {
        std::cout << res << " to " ;
    }
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