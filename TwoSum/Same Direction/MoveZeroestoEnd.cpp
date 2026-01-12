#include <iostream>
#include <unordered_set>
using namespace std;

/**
 * Move Zeroes to the end
 * 
 * 1. Two leftmost pointers to loop through the array. If the first pointer is 0 and second pointer is non-zero, 
 *     we will swap them and continue the loop.
 * 2. If both the pointers == 0, we will increment the second pointer with an intent to find non-zero.
 * 3. If first pointer is non-zero and second pointer is 0 or both the pointers are non-zero, we will increment both the pointers
 *    as we are fulfilling the condition.
 */
void moveZeroes(std::vector<int>& arr)
{
    std::vector<std::vector<int>> results;
    int secondPointer = 1;
    int firstPointer = secondPointer - 1;

    while (secondPointer < arr.size())
    {
        if (arr.at(firstPointer) == 0 && arr.at(secondPointer) != 0)
        {
            int temp = arr.at(firstPointer);
            arr.at(firstPointer) = arr.at(secondPointer);
            arr.at(secondPointer) = temp;
        }
        else if (arr.at(firstPointer) == 0 && arr.at(secondPointer) == 0)
        {
            secondPointer++;
        }
        else 
        {
            firstPointer++;
            secondPointer++;
        }
    }
}

int main()
{
    std::vector<int> arr {0,0,0,9,0,0,0,1};
    moveZeroes(arr);
    for (const auto& element : arr) {
        std::cout << element << std::endl;
    }
}