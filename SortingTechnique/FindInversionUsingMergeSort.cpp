#include <iostream>
using namespace std;

/**
 * 1. Make sure both the sorted arrays are of equal length by insert API (begin, end, default value)
 * 2. Loop through both the arrays with 2 pointers - one pointing to first array and the second one pointing to second array
 * 3. Evaluate and place them in a third array.
 * 
 * 4. The variable 'countOfInversions' checks the number of times pointer in the second array is lesser than first array
 * 
 * O(N)
 */
std::tuple<vector<int>, int> mergeTwoSortedArrays(vector<int> &leftSubArray, vector<int> &rightSubArray, int countOfInversions)
{	
    cout << "starting countOfInversions: " << countOfInversions << endl;
    vector<int> res;
    int totalSizeOfLeftSubArray = leftSubArray.size();
    int totalSizeOfRightSubArray = rightSubArray.size();
    leftSubArray.insert(leftSubArray.end(), totalSizeOfLeftSubArray + totalSizeOfRightSubArray, 2000000);
    rightSubArray.insert(rightSubArray.end(), totalSizeOfLeftSubArray + totalSizeOfRightSubArray, 2000000);

    int i = 0;
    int j = 0;
    while (i < totalSizeOfLeftSubArray || j < totalSizeOfRightSubArray)
    {
        if (leftSubArray[i] <= rightSubArray[j])
        {
            res.push_back(leftSubArray[i]);
            i++;
        }
        else
        {
            res.push_back(rightSubArray[j]);
            j++;
            countOfInversions = countOfInversions + (totalSizeOfLeftSubArray - i);
            cout << "countOfInversions update :  " << countOfInversions << endl;
        }
    }

    cout << "Merge 2 arrays ";
    for (int p = 0; p < res.size(); p++) {
        cout << res[p] << " ";
    }
    cout << "\n";
    return {res, countOfInversions};
}

/**
 * 1. Find pivot using n/2 or n/2+1
 * 2. Create left subarray using constructor -  left subarray (begin, pivot) // it won't include pivot element
 * 3. Create right subarray using constructor - right subarray (pivot, end)
 * 4. Recursively call the same function for both the subarrays until and unless the element size in subarray is 1.
 * 5. The last step is to call merge 2 sorted arrays.
 * 
 * Complexity is O(NLogN) 
 *   a. where O(N) is the merging of two sorted lists.
 *   b. and O(LogN) is the process of repeatedly dividing the lists into two parts - Similar to binary search and hence the operation is O(LogN)
 * 
 * 
 * divide {
    if (l > r) return
    if (l == r) return list[l] // This is where we trigger merge as they are now individual elements
    int m = (l+r)/2
    sortedLeft = divide(list.begin() + l, list.begin() + m)
    sortedRight = divide(list.begin() + m + 1, list.begin() + r)
    Merge(sortedLeft, sortedRight)
}
 */
std::tuple<vector<int>, int> breakIntoSingularArrayAndMergeTheSortedArrays(vector<int>&a, int countOfInversions)
{
    // return of singular array
    if (a.size() == 1){
        cout << "Returning single element : " << a[0] << "\n";
        return {a, countOfInversions};
    }

    int pivotIndex = (a.size()/2);
    std::vector<int> leftSubArray(a.begin(), a.begin() + pivotIndex);
    std::vector<int> rightSubArray(a.begin() + pivotIndex, a.end());

    auto result1 = breakIntoSingularArrayAndMergeTheSortedArrays(leftSubArray, countOfInversions);
    auto sortedLeftSubArray = std::get<0>(result1);
    int countOfInversionForSortedLeftSubArray = std::get<1>(result1);
    cout << "countOfInversionForSortedLeftSubArray : " << countOfInversionForSortedLeftSubArray << endl;

    auto result2 = breakIntoSingularArrayAndMergeTheSortedArrays(rightSubArray, countOfInversions);
    auto sortedRightSubArray = std::get<0>(result2);
    int countOfInversionForSortedRightSubArray = std::get<1>(result2);
    cout << "countOfInversionForSortedRightSubArray : " << countOfInversionForSortedRightSubArray << endl;

    // returning the merge of 2 sorted arrays
    return mergeTwoSortedArrays(sortedLeftSubArray, sortedRightSubArray, countOfInversionForSortedLeftSubArray + countOfInversionForSortedRightSubArray);
}

int numberOfInversions(vector<int>&a, int n) {
    int countOfInversions = 0;
    auto result = breakIntoSingularArrayAndMergeTheSortedArrays(a, 0);
    return std::get<1>(result);
}

int main() 
{
    vector<int> A = {1, 20, 6, 4, 5};
    auto ans = numberOfInversions(A, A.size());
    cout << "Final : " << ans << "\n";
    cout << "\n";
}