#include <iostream>
#include <map>
#include <vector>
#include <unordered_map>
#include <algorithm>

using namespace std;

/**
 * 1. The window will be invalid if the window size exceeds K (Fixed window)
 */
int getMaxSumOfSubArrayWithWindowK(std::vector<int>& arr, int k) {
    int maxSum = 0;
    int left = 0;
    int sum = 0;
    for (int right = 0; right < arr.size(); right++)
    {
        sum+= arr.at(right);
        // Check if the window reached its size
        if ((right - left + 1) == k)
        {
            //process the window
            maxSum = max(maxSum, sum);
            // Deduct the element that you threw from the window and shorten the window
            sum-= arr.at(left);
            left++;
        }
    }
    return maxSum;
}

int main() 
{
    std::vector<int> arr {2, 1, 5, 1, 3, 2};
    std::cout << getMaxSumOfSubArrayWithWindowK(arr, 3) << std::endl;
}
