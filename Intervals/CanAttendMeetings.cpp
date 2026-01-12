#include <iostream>
#include <map>
#include <vector>
#include <algorithm>

using namespace std;

/**
 * Find if there is any overlapping interval
 */
bool canAttendMeetings(std::vector<std::vector<int>>& intervals) {
    std::sort(intervals.begin(), intervals.end(), [](const std::vector<int>& a, const std::vector<int>& b) {
        return a[0] < b[0];
    });

    for (int i = 1; i < intervals.size(); i++) {
        // If the current interval start time is less than previous interval's end time, then the person can't attend meeting.
        if (intervals.at(i-1).at(1) > intervals.at(i).at(0)) {
            return false;
        }
    }

    return true;
}

int main() 
{
    std::vector<std::vector<int>> intervals {{10,12},{6,9},{13,15}};
    std::cout << canAttendMeetings(intervals) << std::endl;
}
