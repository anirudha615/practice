#include <iostream>
#include <map>
#include <vector>
#include <unordered_map>
#include <algorithm>

using namespace std;

/**
 * We need to pick up cards from either the front or back of the array - You can pick only 1 card at a loop.
 * That means you can either pick all from front or all from back or a mix of both front and back.
 * 
 * If this was a problem of finding only from front or back, we can use K as the window size. However, mix of both front and back 
 * introduces a lot of combination which will difficult to evaluate in just 1 loop.
 * 
 * Hence, we need to think of a pattern that won't change. If we fix N - K as the window, that means we will never pick up from here
 * and we will pick from its surrounding.
 *      a. So, we will keep on shifting the window from which we will never pick and calculate the maxSum of its nearby surrounding
 * 
 */
int getMaxSumOfKCardsWithOptions(std::vector<int>& arr, int k) {
    // If K is larger than array size, return total
    int total = 0;
    for (int a : arr) {
        total += a;
 
    }
    if (arr.size() <= k) return total;
    
    
    int maxSum = 0;
    int left = 0;
    int sum = 0;
    
    for (int right = 0; right < arr.size(); right++)
    {
        sum+= arr.at(right);
        // Check if the window (from which we will never pick up) reached its size
        if ((right - left + 1) == (arr.size() - k))
        {
            //process the window for 4 options
            maxSum = max(maxSum, total - sum);
            // Deduct the element that you threw from the window and shorten the window
            sum-= arr.at(left);
            left++;
        }
    }
    return maxSum;
}

int main() 
{
    std::vector<int> arr {32,69,37,79,4,33,29,33,45,1,99,90,56,24,76};
    std::cout << getMaxSumOfKCardsWithOptions(arr, 10) << std::endl;
}
