#include <iostream>
using namespace std;

/**
 * 1. Since the range of elements will always be [1, N], loop through the array size and pput the element as key and count as value in the map.
 * 2. So, irrespective an element is present or not, the map will show map[element] = 0
 * 3. Loop through the array and update the count in the map.
 * 
 * 4. Loop through the map and check if a number has count >1 , its repeated and if its zero, its missing.
 */
vector<int> findMissingRepeatingNumbers(vector <int> a) {
    std::unordered_map<int, int> count;

    for (int i = 1; i <= a.size(); i++)
    {
        count[i] = 0;
    }

    for (int i = 0; i < a.size(); i++)
    {
        if (count[a[i]] >= 0)
        {
            count[a[i]] = ++count[a[i]];
        }
    }

    int missingNumber = 0;
    int duplicateNumber = 0;
    for (const auto& pair : count) {
        if (pair.second == 2) 
        {
            duplicateNumber = pair.first;
            cout << "duplicateNumber  is: " << duplicateNumber << std::endl;
        }
        else if (pair.second == 0)
        {
            missingNumber = pair.first;
            cout << "missingNumber  is: " << missingNumber << std::endl;
        }
    }
    return {duplicateNumber, missingNumber};
}


int main() 
{
    vector<int> A = {1, 3, 1};
    auto ans = findMissingRepeatingNumbers(A);
    cout << "Final" << "\n";
    for (int i = 0; i < ans.size(); i++) {
        cout << ans[i] << " ";
    }
    cout << "\n";
}