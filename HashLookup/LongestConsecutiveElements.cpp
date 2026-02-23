#include <iostream>
#include <unordered_set>
using namespace std;

/**
 * This is not a contiguous problem. The problem mentions that you have to sort the list and 
 * then find length of consecutive elements (which has a difference of 1)
 * However, we need to solve it in O(N) so sorting is not allowed.
 * 
 * Approach -
 *   a. Start with the list in unordered set which will take O(N) time to eliminate duplication
 *   b. Proceed with calculation of consecutive elements
 *        - Check if the current element (in the loop) -1 exists in the set. IF yes, skip it.
 *        - The intention is to start counting with elements which is the starting of the sequence
 *        - and then iterate to find the consecutive elements (with a difference of 1)
 */
int longestSuccessiveElements(vector<int>& nums) {
    if (nums.empty()) {
        return 0;
    }
    std::unordered_set<int> state;
    for (int data: nums) {
        state.insert(data);
    }
    int maxLength = 0;
    for (int data : state) {
        // If data - 1 is not in set, then data is the starting sequence.
        if (!state.count(data - 1)) {
            int currentLength = 1;
            // Find if +1 is present in the set and increment the count.
            // Those +1 will not be re-evaluated because their -1 is in the set (top condition)
            int number = data + 1;
            while (state.count(number)) {
                currentLength++;
                number++;
            }
            maxLength = std::max(maxLength, currentLength);
        }
    }
    return maxLength;
}


int main() 
{
    vector<int> Mat {15, 6, 2, 1, 16, 4, 2, 29, 9, 12, 8, 5, 14, 21, 8, 12, 17, 16, 6, 26, 3};
    auto ans = longestSuccessiveElements(Mat);
    cout << "ans : " << ans << "\n";
}