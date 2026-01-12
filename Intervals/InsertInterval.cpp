#include <iostream>
#include <map>
#include <vector>
#include <algorithm>

using namespace std;

std::vector<std::vector<int>> insertIntervals(std::vector<std::vector<int>>& intervals, vector<int> newInterval) {
    // sort by start time
    intervals.push_back(newInterval);
    std::sort(intervals.begin(), intervals.end(), [](const std::vector<int>& a, const std::vector<int>& b) {
        return a[0] < b[0];
    });

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
    return merged_interval;
}

int main() 
{
    std::vector<std::vector<int>> intervals {{1,2}, {3,5}, {6,7}, {8,10}, {12,16}};
    std::vector<std::vector<int>> results = insertIntervals(intervals, {4,8});
    for (auto result : results) {
        for (auto res: result){
            std::cout << res << ", ";
        }
        std::cout<<endl;
    }
}
