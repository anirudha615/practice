#include <iostream>
using namespace std;

/**
 * Count the number of elements in the array and if the count is more than N/2, add them to the set.
 */
int majorityElement(vector<int> v) {
    std::unordered_map<int, int> countMap;

    for (auto x : v)
    {
        if (countMap[x] >= 0)
        {
            countMap[x] = ++countMap[x];
            if (countMap[x] > (v.size() / 2))
            {
                return x;
            }
        }
        else
        {
            countMap[x] = 0;
        }
    }
    return -1;
}

int main() 
{
    vector<int> Mat {2,2,1,1,1,2,2};
    auto ans = majorityElement(Mat);
    cout << "ans : " << ans << "\n";
}