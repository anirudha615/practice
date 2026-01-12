#include <iostream>
#include <unordered_set>
using namespace std;

/**
 * Sort Color 0, 1 and 2 in ascending order without sort function.
 * 
 * 1. In this case, as we loop through the array, we will count the number of 2s and erase the one in the array.
 * 2. At the same time, we will move the 1s to the end.
 * 3. We will keep a first and second pointer and if the first pointer is 1 and second pointer is 0, we will swap and move to the next loop.
 * 4. If the first and second pointer is 1, we will increment the second pointer with an intent to find 0.
 * 5. If the first and second pointer is 0 or first pointer is 0 and second pointer is 1, we will increment first and second pointer both
 *    as we are fulfilling the condition.
 * 6. Once the loop is over, we will add 2s as per the counter in Step 1.
 */
void sortColors(std::vector<int>& arr)
{
    int leftPointer = 0;
    int rightPointer = leftPointer + 1;
    int twoNumbers = 0;

    while (rightPointer < arr.size())
    {
        if (arr.at(leftPointer) == 2)
        {
            arr.erase(arr.begin() + leftPointer);
            twoNumbers++;
            continue;
        }
        else if (arr.at(rightPointer) == 2)
        {
            arr.erase(arr.begin() + rightPointer);
            twoNumbers++;
            continue;
        }

        if (arr.at(leftPointer) == 1 && arr.at(rightPointer) == 0)
        {
            int temp = arr.at(leftPointer);
            arr.at(leftPointer) = arr.at(rightPointer);
            arr.at(rightPointer) = temp;
        }
        else if (arr.at(rightPointer) == 1 && arr.at(leftPointer) == 1)
        {
            rightPointer++;
        }
        else 
        {
            leftPointer++;
            rightPointer++;
        }
    }
    arr.insert(arr.end(), twoNumbers, 2);
}

int main()
{
    std::vector<int> arr {0,0,1,0,1,1, 2};
    sortColors(arr);
    for (const auto& element : arr) {
        std::cout << element << std::endl;
    }
}