#include <iostream>
#include <vector>
#include <unordered_set>

using namespace std;

/**
 * https://leetcode.com/problems/longest-mountain-in-array/
 * 
 * First identify the peak, switch the flag and capture violation.
 * Once violation capature (FIXED WINDOW), close the window and start a new window
 */
int longestMountainArray(vector<int>& arr) {
    if (arr.size() < 3) {
        return 0;
    }
    int maxLength = 0;
    int left = 0;
    int prev = arr[0];
    // For a mountain to exist, it should increase first and then decrease
    bool isRising = true;
    for (int right = 1; right < arr.size(); right++) {

        // Peak detection ONLY after confirmed uphill (at least 2 steps up)
        if (isRising && prev > arr[right] && right - left > 1) {
            isRising = false;
        }

        // Window is invalid if it is suppose to rise but prev is >= current value.
        // Window is invalid if it is suppose to decline but prev is <= current value
        if ((isRising && prev >= arr[right]) || (!isRising && prev <= arr[right])) {
            
            // Capture the previous window if mountain is declining where right-left > 1
            if (right - left >= 3 && !isRising) {
                maxLength = std::max(maxLength, right - left);
            }
            left = right;
            isRising = true;
        }

        prev = arr[right];
    }

    // If the loop ends but the mountain was declining, we should capture it.
    if (arr.size() - left >= 3 && !isRising) {
        maxLength = std::max(maxLength, (int)(arr.size()) - left);
    }

    return maxLength;
}

int main() {
    std::vector<int> arr {2,1,4,7,3,2,5};
    std::cout << longestMountainArray(arr) << std::endl;
}