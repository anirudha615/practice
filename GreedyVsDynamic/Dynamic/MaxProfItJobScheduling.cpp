#include <iostream>
#include <unordered_set>
#include <cmath>
using namespace std;


/**
 * This is similar to non-contiguous longest substring.
 * 
 * 1. Since this is a problem of interval, sort the interval by start time.
 * 2. Loop through the interval and check if any of previous interval's end time <= current interval's start time - 
 *    At this moment, you know the previous interval got completed and you can pick up current interval job. Hence, add previous interval profit to the current interval profit.
 * 3. If you din't find any previous interval that got completed, think what if you discard all previous job and pick up the current job, then your profilt = current job's profit.
 * 4. Keep a track of the maxProfit as it is not necessary that last interval will always be non-overlapping.
 */
int jobScheduling(vector<int> startTime, vector<int> endTime, vector<int> profit) {
    int maxProfit = 0;
    
    // Prepare the job interval list
    std::vector<std::pair<int, std::vector<int>>> job_interval_list;
    for (int job = 0; job < startTime.size(); job++) {
        job_interval_list.push_back({profit.at(job), {startTime.at(job), endTime.at(job)}});
    }

    // sort them by start time
    std::sort(job_interval_list.begin(), job_interval_list.end(), [](std::pair<int, std::vector<int>> jobA, std::pair<int, std::vector<int>> jobB) {
        return jobA.second[0] < jobB.second[0];
    });

    // Initialize Profit earned
    std::vector<int> dp_profit(job_interval_list.size(), 0);

    // For every interval, check which of the previous interval end time <= current interval start time 
    // and then the max DP of them and add to the current interval's profit.
    for (int current_job_interval = 0; current_job_interval < job_interval_list.size(); current_job_interval++) {
        int maxProfitFromPreviousJobInterval = -1;
        for (int previous_job_interval = 0; previous_job_interval < current_job_interval; previous_job_interval++) {
            // The below block executes if there is a previous job interval whose end time <= current job interval start time
            if (job_interval_list[current_job_interval].second[0] >= job_interval_list[previous_job_interval].second[1]) {
                maxProfitFromPreviousJobInterval = std::max(maxProfitFromPreviousJobInterval, dp_profit[previous_job_interval]);
            }
        }

        // If there is maxProfitFromPreviousJobInterval, add it to the current job interval profit
        if (maxProfitFromPreviousJobInterval != -1) {
            dp_profit[current_job_interval] = maxProfitFromPreviousJobInterval + job_interval_list[current_job_interval].first;
        } else {
            // If there is no profit from the previous job interval, current job interval profilt = profit from the current job
            dp_profit[current_job_interval] = job_interval_list[current_job_interval].first;
        }

        // Find the max Profit instead of finding the max from the dp_profit list
        maxProfit = std::max(maxProfit, dp_profit[current_job_interval]);
    }

    return maxProfit;
}


int main() {
    std::vector<int> startTime {6,15,7,11,1,3,16,2};
    std::vector<int> endTime {19,18,19,16,10,8,19,8};
    std::vector<int> profit {2,9,1,19,5,7,3,19};
    std::cout << jobScheduling(startTime, endTime, profit) << std::endl;
}