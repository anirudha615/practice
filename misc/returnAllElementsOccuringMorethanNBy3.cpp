#include <iostream>
#include <set>
using namespace std;

/**
 * Count the number of elements in the array and if the count is more than N/3, add them to the set.
 */
set<int> majorityElement(vector<int> v) {
    std::unordered_map<int, int> countMap;
    std::set<int> res;
    sort(v.begin(), v.end());

    for (auto x : v)
    {
        countMap[x] = countMap[x] >= 0 ? ++countMap[x] : 0;
        if (countMap[x] > v.size() / 3 && !res.count(x))
        {
            res.emplace(x);
        }
    }

    return res;
}

int main() 
{
    vector<int> Mat {2,2,1,1,1,2,2};
    auto ans = majorityElement(Mat);
    for (auto x : ans)
    {
        cout << x << " ";
    }
   cout << endl;
}