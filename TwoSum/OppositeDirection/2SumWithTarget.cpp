#include <iostream>
#include <map>
using namespace std;

/**
 * 1. Sort the array
 * 2. Two pointers at two ends of the sorted array. 
 * 3. Calculate the sum of first and second pointer
 *   a. if it lesser than target, we need a bigger number, hence we will increment the first pointer.
 *   b. If it is greater than target, we need a lesser number, hence we will decrement the second pointer.
 *   c. If it is equal, collect the pair and increment the first pointer or decrement the second pointer
 * 
 * Optimization: Check for both the pointers if the next value is a duplicate of previous values and if yes, skip.
 */
std::vector<std::pair<int, int>>  twoSum(vector<int>& arr, int target) {
    sort(arr.begin(), arr.end());
    int leftPointer = 0;
    int rightPointer = arr.size() - 1;
    std::vector<std::pair<int, int>> results;
    while (leftPointer < rightPointer) 
    {
        int sum = arr.at(leftPointer) + arr.at(rightPointer);
        if (sum == target)
        {
            results.push_back({arr.at(leftPointer), arr.at(rightPointer)});
            ++leftPointer;
        } else if (sum < target)
        {
            ++leftPointer;
        } else 
        {
            --rightPointer;
        }
    }
    return results;
}

int main() 
{
    vector<int> arr {0,3,4,6,9,10,13};
    std::vector<std::pair<int, int>> results = twoSum(arr, 13);
    for (auto result : results) 
    {
        std::cout << result.first << ", " << result.second << std::endl;
    }
}