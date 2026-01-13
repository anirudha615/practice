#include <iostream>
#include <map>
#include <vector>
#include <unordered_map>
#include <algorithm>

using namespace std;

int getMinimumLengthOfContiguousSubArrayWithTargetSum(std::vector<int>arr, int targetSum) {
    int sum = 0; // state
    int leftBoundary = 0;
    int minLength = INT_MAX;
    for (int rightBoundary = 0; rightBoundary < arr.size(); rightBoundary++) {
        // Updating the state
        sum = sum + arr.at(rightBoundary);

        // Window is valid
        while (sum >= targetSum) {
            // Process the window
            int length = rightBoundary - leftBoundary + 1;
            minLength = std::min(minLength, length);
            
            // Shorten the window after processing it 
            sum -= arr.at(leftBoundary);
            leftBoundary++;
        }
    }
    return minLength;
}

int main() {
    std::vector<int> arr {2,3,1,2,4,3};
    std::cout << "result is : " << getMinimumLengthOfContiguousSubArrayWithTargetSum(arr, 7) << std::endl;
}
