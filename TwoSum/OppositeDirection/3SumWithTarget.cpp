#include <iostream>
using namespace std;

std::vector<std::tuple<int, int, int>>  threeSum(vector<int>& arr, int target) {
    sort(arr.begin(), arr.end());
    std::vector<std::tuple<int, int, int>> results;
    // -2 because of 2 pointers doing
    for (int firstPointer = 0 ; firstPointer < arr.size() - 2; firstPointer++) {
        // Not counting duplicates
        if (firstPointer - 1 >=0 && arr.at(firstPointer - 1) == arr.at(firstPointer)) {
            continue;
        }
        int leftPointer = firstPointer + 1;
        int rightPointer = arr.size() - 1;
        while (leftPointer < rightPointer) {
            // Not counting duplicates
            if (leftPointer - 1 != firstPointer && arr.at(leftPointer - 1) == arr.at(leftPointer))
            {
                ++leftPointer;
                continue;
            } else if (rightPointer + 1 < arr.size() && arr.at(rightPointer + 1) == arr.at(rightPointer))
            {
                --rightPointer;
                continue;
            }

            int sum = arr.at(firstPointer) + arr.at(leftPointer) + arr.at(rightPointer);
            if (sum == target) {
                results.push_back(std::make_tuple(arr.at(firstPointer), arr.at(leftPointer), arr.at(rightPointer)));
                ++leftPointer;
            } else if (sum < target)
            {
                ++leftPointer;
            } else {
                --rightPointer;
            }
        }
    }
    return results;
}

int main() 
{
    vector<int> arr {0,0,0,0};
    std::vector<std::tuple<int, int, int>> results = threeSum(arr, 0);
    for (auto result : results) {
        std::cout << std::get<0>(result) << ", " << std::get<1>(result) << ", " << std::get<2>(result) << std::endl;
    }
}