#include <iostream>
#include <map>
#include <vector>
#include <unordered_map>
#include <algorithm>

using namespace std;

/**
 * We need to prepare a window of size K in which any of the element is not repeated.
 * 
 * 1. Proactively find if the map has repeated elements without the window to have size K and keep on shortening the window.
 * 2. If there is no repeated elements in the map and window is of size K, process it and shorten the window.
 * 
 * Proactively shortening the window is taken from longest substring (sliding window)
 */
int getMaxSumOfSubArrayWithWindowK(std::vector<int>& arr, int k) {
    int maxSum = 0;
    int left = 0;
    int sum = 0;
    std::unordered_map<int, int> map;
    for (int right = 0; right < arr.size(); right++) {

        // Update the window state
        sum+= arr.at(right);
        map[arr.at(right)]++;

        // Proactively find if the window state including repeated elements and shorten the widow
        while (map[arr.at(right)] > 1) {
            sum-= arr.at(left);
            map[arr.at(left)]--;
            left++;
        }

        // When the window rules match, process the window
        if ((right - left + 1) == k) {
            maxSum = max(maxSum, sum);
            
            // shorten the window by updating the state
            sum-= arr.at(left); 
            map[arr.at(left)]--;
            left++;
        }
    }
    return maxSum;
}

int main() 
{
    std::vector<int> arr {9,18,10,13,17,9,19,2,1,18};
    std::cout << getMaxSumOfSubArrayWithWindowK(arr, 5) << std::endl;
}
