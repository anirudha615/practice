#include <iostream>
#include <map>
#include <vector>
#include <algorithm>

using namespace std;

std::vector<std::vector<int>> getFreeCalendarSlotForAllEmployees(std::vector<std::vector<std::vector<int>>>& schedules) {
    // Collect intervals from all the employees schedule
    std::vector<std::vector<int>> intervals;
    for (std::vector<std::vector<int>> schedule : schedules) {
        for (std::vector<int> interval: schedule) {
            intervals.push_back(interval);
        }
    }

    // sort the intervals by start time
    std::sort(intervals.begin(), intervals.end(), [](const std::vector<int>& a, const std::vector<int>& b) {
        return a[0] < b[0];
    });

    // Merge the intervals
    std::vector<std::vector<int>> merged_interval;
    for (int i = 0; i < intervals.size(); i++) {
        // If there is no entry in merged interval or the current evaluated interval's start time > last entry in merged interval's end time (they don't overlap), 
        // add the interval to the merged interval set as it is already sorted.
        if (merged_interval.empty() || intervals.at(i).at(0) > merged_interval.back().at(1)) {
            merged_interval.push_back(intervals.at(i));
        } else {
            // If the new interval start time is in between any interval's start and end time, merge them by checking the max of end time of 
            // last merged item and currently evaluated interval
            merged_interval.back()[1] = max(merged_interval.back()[1], intervals.at(i).at(1));
        }
    }

    // Once the merged interval list is ready, we can prepare free_interval list by subtracting the next - previous
    std::vector<std::vector<int>> free_interval;
    if (merged_interval.size() < 2) {
        return free_interval;
    }
    for (int i = 1; i < merged_interval.size(); i++) {
        free_interval.push_back({merged_interval.at(i-1).at(1), merged_interval.at(i).at(0)});
    }
    
    return free_interval;
}

int main() 
{
    std::vector<std::vector<std::vector<int>>> schedule {{{2,4}, {7,10}}, {{6,9}}, {{1,5}}};
    std::vector<std::vector<int>> results = getFreeCalendarSlotForAllEmployees(schedule);
    for (auto result : results) {
        for (auto res: result){
            std::cout << res << ", ";
        }
        std::cout<<endl;
    }
}
