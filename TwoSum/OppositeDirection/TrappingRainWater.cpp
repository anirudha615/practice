#include <iostream>
using namespace std;


/**
 * Calculate the rain water trapped between 2 blocks.
 * 
 * 1. Place 2 pointers at 2 extreme ends of the unsorted array.
 * 2. The approach is to calculate the water trapped by comparing the current left and right pointer with their respective max left/right pointer.
 *    Instead of comparing the left and right pointers.
 * 3. Water Trapped = max_left - height[left] or max_right - height[right] 
 * 4. However, we will move the pointers based on two pointer principle where if max_left < max_right, we will increment left pointer else decrement right pointer.
 */
int trap(vector<int>& height)
{
    int trappedWater = 0;
    int leftPointer = 0;
    int rightPointer = height.size() - 1;
    int maxLeft = height.at(leftPointer);
    int maxRight = height.at(rightPointer);

    while (leftPointer < rightPointer)
    {
        if (maxLeft < maxRight)
        {
            // increment the left pointer
            ++leftPointer;
            // compare the new and previous pointer to get maxLeft
            maxLeft = std::max(maxLeft, height.at(leftPointer));
            // calculate the water trapped
            trappedWater += maxLeft - height.at(leftPointer);
        } else {
            --rightPointer;
            // compare the new and previous pointer to get maxRight
            maxRight = std::max(maxRight, height.at(rightPointer));
            // calculate the water trapped
            trappedWater += maxRight - height.at(rightPointer);
        }
    }
    return trappedWater;
}

int main()
{
    std::vector<int> heights {2,8,5,5,6,1,7,4,5};
    std::cout << trap(heights) << std::endl;
}