#include <iostream>
using namespace std;

/**
 * 1. Sort the array
 * 2. Increment the longest sequence if the difference is 1. Else reset the longest sequence to 1.
 * 3. Calculate maximum longest sequence after every loop
 */
int longestSuccessiveElements(vector<int>&a) {
    std::sort(a.begin(), a.end());
    for (auto x : a) 
    {
        cout << x << ", ";
    }
    cout << endl;
    
    int longestSequence = 1;
    int maxLongestSequence = longestSequence;

    for (int i = 1; i < a.size(); i++)
    {
        if (a.at(i) - a.at(i-1) == 1)
        {
            ++longestSequence;
        } else if (a.at(i) - a.at(i-1) > 1)
        {
            longestSequence = 1;
        }
        maxLongestSequence = max(maxLongestSequence, longestSequence);
    }
    return maxLongestSequence;
}


int main() 
{
    int R = 3;
    int C = 5;
    vector<int> Mat {15, 6, 2, 1, 16, 4, 2, 29, 9, 12, 8, 5, 14, 21, 8, 12, 17, 16, 6, 26, 3};
    auto ans = longestSuccessiveElements(Mat);
    cout << "ans : " << ans << "\n";
}