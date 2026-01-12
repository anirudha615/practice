#include <iostream>
#include <map>
#include <vector>
#include <algorithm>

using namespace std;

/**
 * Since this is a problem of non-overlapping interval, sort by end timestamp.
 * Also, the trick is if you encounter overlapping intervals, you need to remove them by not changing the previous interval value.
 * so that we can compare the current iterated interval with previous interval value.
 */
int removeToFormNonOverlapping(std::vector<std::vector<int>>& intervals) {
    // sort by end time
    std::sort(intervals.begin(), intervals.end(), [](const std::vector<int>& a, const std::vector<int>& b) {
        return a[1] < b[1];
    });
    
    int overlappingInterval = 0;
    std::vector<int> previousInterval = intervals.at(0);
    for (int i = 1; i < intervals.size(); i++) {
        if (intervals.at(i).at(0) < previousInterval.at(1)) {
            // count the overlapping interval
            overlappingInterval++;
            // remove the overlapping interval by not changing the previous interval
        } else {
            previousInterval = intervals.at(i);
        }
    }
    return overlappingInterval;
}

int main() 
{
    std::vector<std::vector<int>> intervals {{0,2}, {1,3}, {1,3}, {2,4}, {3,5}, {3,5}, {4,6}};
    std::cout << removeToFormNonOverlapping(intervals) << std::endl;
}
