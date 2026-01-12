#include <iostream>
#include <map>
using namespace std;

/**
 * 1. Sort the array
 * 2. Fix the first pointer on the left most element
 * 3. Fix the second pointer on the second left most element
 * 4. Rotate the third pointer from left to right as it is ascending
 * 5. Rotate the forth pounter from right to left as it is descending.
 * 6. check if all the four pointers sum up to be target.
 * 7. If yes, collect the quadruple and break
 * 
 * Optimization: Check for all pointers if the next value is a duplicate of previous values and if yes, skip.
 */
vector<std::tuple<int, int, int, int>> fourSum(vector<int>& arr, int target) {
    sort(arr.begin(), arr.end());
    vector<std::tuple<int, int, int, int>> quads;
    for (int firstPointer = 0; firstPointer < arr.size() - 3; firstPointer++)
    {
        if ((firstPointer - 1 >= 0 && arr.at(firstPointer - 1) == arr.at(firstPointer)))
        {
            continue;
        }
        for (int secondPointer = firstPointer + 1; secondPointer < arr.size() - 2; secondPointer++)
        {
            if ((secondPointer != (firstPointer + 1) && arr.at(secondPointer - 1) == arr.at(secondPointer)))
            {
                continue;
            }
            int leftPointer = secondPointer + 1;
            int rightPointer = arr.size() - 1;
             while (leftPointer < rightPointer)
            {
                // Not counting duplicates
                if (leftPointer - 1 != secondPointer && arr.at(leftPointer - 1) == arr.at(leftPointer))
                {
                    ++leftPointer;
                    continue;
                } 
                if (rightPointer + 1 < arr.size() && arr.at(rightPointer + 1) == arr.at(rightPointer))
                {
                    --rightPointer;
                    continue;
                }
                int res = arr.at(firstPointer) + arr.at(secondPointer) + arr.at(leftPointer) + arr.at(rightPointer);
                if (res == target)
                {
                    quads.push_back({arr.at(firstPointer), arr.at(secondPointer), arr.at(leftPointer), arr.at(rightPointer)});
                    ++leftPointer;
                } else if (res < target)
                {
                    /** We need bigger values, hence increasing the third pointer in hoping to get a bigger value */
                    ++leftPointer;
                } else
                {
                    /** We need smaller values, hence decreasing the fourth pointer in hoping to get a bigger value */
                    --rightPointer;
                }
            }
        }
    }
    return quads;
}

int main() 
{
    vector<int> arr {2, 2, 2, 2, 1, 3};
    vector<std::tuple<int, int, int, int>> result = fourSum(arr, 8);
    for (auto y: result)
    {
        std::cout<< std::get<0>(y) << ", " << std::get<1>(y) << ", " << std::get<2>(y) << ", " << std::get<3>(y) << std::endl;
    }
   
}