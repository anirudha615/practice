#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
using namespace std;

/**
 * Search in a sorted array which is rotated at a point.
 * 
 * 1. Find the pivot using left + right / 2
 * 2. Check if the left <= middle or middle > right
 * 3. If the either of the above is right, we will begin our search in that direction.
 * 4. The reason being that side is sorted and we can perform binary search.
 * 5. Since the side is sorted, we will find if the target value lies within it.
 * 6. If it doesn't move, to the other non-sorted side and agian follow steps 1-5 to make sure you start searching in the sorted path.
 */
int searchInRotatedArray(vector<int>& array, int targetValue) {
    int leftBoundary = 0;
    int rightBoundary = array.size() - 1;

    while (leftBoundary <= rightBoundary) {
        int mid = (leftBoundary + rightBoundary) / 2;

        if (array.at(mid) == targetValue) {
            return mid;
        }

        // Always search in the sorted side
        if (array.at(leftBoundary) <= array.at(mid)) {
            // left side is sorted
            // If the target value is in between the left and mid (sorted range), dive deeper
            if (targetValue >= array.at(leftBoundary) && targetValue <= array.at(mid)) {
                rightBoundary = mid - 1;
            } else {
                // Since the target value is not between mid and right, decision to search in sorted left side was wrong,
                // hence reverting to the right side (we will find the sorted range in the next loop) 
                leftBoundary = mid + 1;
            }

        } else {
            // right side is sorted
            // If the target value is in between the mid and right (sorted range), dive deeper
            if (targetValue >= array.at(mid) && targetValue <= array.at(rightBoundary)) {
                leftBoundary = mid + 1;
            } else {
                // Since the target value is not between mid and right, decision to search in sorted right side was wrong,
                // hence reverting to the left side (we will find the sorted range in the next loop) 
                rightBoundary = mid - 1;
            }
        }
    }
    return -1;
}



int main() {
    vector<int> array = {4,5,6,7,0,1,2};
    std::cout << searchInRotatedArray(array, 0) << std::endl;
}