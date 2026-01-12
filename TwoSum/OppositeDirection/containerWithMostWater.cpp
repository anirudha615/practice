#include <iostream>
using namespace std;

/**
 * Calculate the container with the most water
 * 
 * 1. Have a left and right pointer at the 2 ends.
 * 2. Calculate the breadth as right pointer - left pointer and the height as min (rightPointer, leftPointer). Calculate the area and keep track of max area.
 * 3. If the left pointer is lesser than the right pointer, lets increment the left pointer to get a bigger wall.
 * 4. If the left pointer is greater than the right pointer, lets decrement the right pointer to get a bigger wall.
 */
int maxArea(const std::vector<int>& heights)
{
    int maxArea = 0;
    int leftPointer = 0;
    int rightPointer = heights.size() - 1;
    while (leftPointer <= rightPointer)
    {
        int area = min(heights.at(leftPointer), heights.at(rightPointer)) * (rightPointer - leftPointer); // height * breadth
        maxArea = max(maxArea, area);
        // If leftPointer <= rightPointer, lets increment leftPointer in hoping to get a bigger wall
        if (heights.at(leftPointer) <= heights.at(rightPointer))
        {
            ++leftPointer;
        } 
        // If leftPointer > rightPointer, lets decrement rightPointer in hoping to get a bigger wall
        else if (heights.at(leftPointer) > heights.at(rightPointer))
        {
            --rightPointer;
        }
    }
    return maxArea;
}

int main()
{
    std::vector<int> heights {1,8,6,2,5,4,8,3,7};
    std::cout << maxArea(heights) << std::endl;
}