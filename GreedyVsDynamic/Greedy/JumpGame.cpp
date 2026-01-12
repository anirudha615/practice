#include <iostream>
#include <unordered_map>
#include <cmath>
using namespace std;

/**
 * The trick is to keep a track of furthestIndexReached.
 * 
 * 1. Loop through the array and if the maximum jump at the current index exceeds the array size, return true.
 * 2. If the maximum jump is greater than furthestIndexReached, update furthestIndexReached.
 * 
 * 3. If the maximum jump is equal to the index where you started but there is still room to proceed to furthestIndexReached, skip processing the index.
 * 4. However, the current index is equal to furthestIndexReached, we are cooked and return false
 */
bool canJump(vector<int>& nums) {

    // Let's keep a track of furthestIndexReached
    int furthestIndexReached = 0;

    for (int index = 0; index < nums.size(); index++) {
        int maximumJumpToIndex = nums.at(index) + index;
        
        // If the jump is greater or equal to the last index of the array, 
        // we are good. We can always jump less than our max potential.
        if (maximumJumpToIndex >= nums.size() - 1) {
            return true;
        }

        // If the maximumJumpToIndex is greater than furthestIndexReached but not exceeding the array size, 
        // lets store the furthestIndexReached.
        if (maximumJumpToIndex > furthestIndexReached) {
            furthestIndexReached = maximumJumpToIndex;
        }

        // If the maximumJumpToIndex equates to where you started, which is equal to furthestIndexReached
        // We cannot go further.
        if (maximumJumpToIndex == index && maximumJumpToIndex == furthestIndexReached) {
            return false;
        }
    }

    return false;
}

int main() {
    std::vector<int> num {2,0,0,1,4};
    std::cout << canJump(num) << std::endl;
}

