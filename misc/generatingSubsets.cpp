#include <iostream>
using namespace std;

/**
 * https://www.hellointerview.com/community/questions/generate-subsets/cm5eh7nrh04p6838oj8vsejv3?level=MID_LEVEL
 * 
 * 1. Add an empty set to the results
 * 2. Copy the results while iterating every element and add that element to the results copy.
 * 3. Append the results copy to the results.
 * 
 * This will create 2^N subsets (N = size of the nums)
 */
vector<vector<int>> subsets(vector<int>& nums) {
    std::vector<std::vector<int>> results = {{}};
    for (int num : nums) {
        std::vector<std::vector<int>> copyResults = results;
        for (std::vector<int> copyElement : copyResults) {
            copyElement.push_back(num);
            results.push_back(copyElement);
        }
    }
    return results;
}

int main() {
    vector<int> nums = {1, 2, 3};
    vector<vector<int>> result = subsets(nums);

    for (const auto& subset : result) {
        cout << "[";
        for (int i = 0; i < subset.size(); i++) {
            cout << subset[i];
            if (i < subset.size() - 1) cout << ",";
        }
        cout << "]" << endl;
    }
    return 0;
}