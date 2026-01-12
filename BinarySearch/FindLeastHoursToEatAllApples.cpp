#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
using namespace std;

/**
 * Story: A marathon consists of N tracks, each with a different number of loops. The runner must complete all tracks within H hours. 
 *        To conserve energy while still finishing on time, we need to determine the minimum possible pace (in loops per hour) 
 *        that allows them to complete the entire marathon within the allotted time.
 * 
 * A marathon has [3,6,7,11] loops in each track respectively.  The runner needs to complete the entire track in 8 hours.
 * we need to find the slowest possible pace loops/hour. (4 loops per hour)
 * 
 * Find the minimum rate at which a monkey can eat all the apple in h hours.
 * 
 * 1. Sort the apples in the room in an increasing order.
 * 2. Figure out the max range by selecting the largest element in the array
 * 3. Perform binary search by selecting the mid from (left+right/2)
 * 4. Calculate the total hours from the middle rate.
 * 5. If the total hours is less than required hours, we will thrive to do better by analyzing the left of mid (as we need to go lesser).
 *    Hence, right will be mid-1 and left = left.
 * 6. If the total hours is more than required hours, we will have to get a bigger rate by analyzing the right of mid (as we need to go higher).
 *    Hence, right will be right and left = mid + 1.
 */
int minEatingSpeed(std::vector<int> appleList, int targetHour) {
    int rightBoundary = *(std::max_element(appleList.begin(), appleList.end()));
    int leftBoundary = 1;
    int minimumRate = -1;

    if (appleList.size() == targetHour) {
        return rightBoundary;
    }

    while (leftBoundary <= rightBoundary) {
        int mid = (leftBoundary + rightBoundary)/2;

        // calculate hour
        uint64_t hour = 0;
        for (int i = 0; i < appleList.size(); i++) {
            hour += (uint64_t)std::ceil((double)appleList.at(i)/mid);
        }

        if (hour <= targetHour) {
            // Everytime it comes over here, we have find a new min.
            minimumRate = mid;
            // There is a chance to find low rate and still match the target hour, hence decrease the rate.
            rightBoundary = mid - 1;
        } else {
            // Current mid's total hour is greater then intended, go to the right side to increase the rate.
            leftBoundary = mid + 1;
        }
    }
    return minimumRate;
}


int main()
{
    //  vector<int> applesInEachRoom = {805306368,805306368,805306368};
    vector<int> applesInEachRoom = {805306368,805306368,805306368};
    std::cout << minEatingSpeed(applesInEachRoom, 1000000000) << std::endl;
}


/**
 * 1. Input:
        loops = [10]
        H = 5

        Expected Output: 2

    2. Input:
        loops = [100, 200, 300, 400, 500]
        H = 7

        Expected Output: 300 

    3. Input:
        piles = [50, 100, 150, 200, 250]
        H = 5

        Expected Output: 250

    4. Input:
        loops =  [3,6,7,11]
        H = 8

        Expected Output: 4
   

 */