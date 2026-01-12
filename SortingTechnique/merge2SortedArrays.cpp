#include <iostream>
using namespace std;

/**
 * 1. Make sure both the sorted arrays are of equal length by insert API (begin, end, default value)
 * 2. Loop through both the arrays with 2 pointers - one pointing to first array and the second one pointing to second array
 * 3. Evaluate and place them in a third array.
 * 
 * O(N) - Also have a look at merge 2 sorted linked lists
 */
void mergeTwoSortedArraysWithoutExtraSpace(vector<long long> &a, vector<long long> &b)
{	
    vector<long long> res;
    int totalSizeOfA = a.size();
    int totalSizeOfB = b.size();
    a.insert(a.end(), totalSizeOfB + totalSizeOfA, 2000000);
    b.insert(b.end(), totalSizeOfB + totalSizeOfA, 2000000);

    int i = 0;
    int j = 0;
    while (i < totalSizeOfA || j < totalSizeOfB)
    {
        if (a[i] <= b[j])
        {
            res.push_back(a[i]);
            i++;
        }
        else
        {
            res.push_back(b[j]);
            j++;
        }
    }

    for (int p = 0; p < res.size(); p++) {
        cout << res[p] << " ";
    }
    cout << "\n";

    a = {};
    a.insert(a.begin(), res.begin(), res.begin() + totalSizeOfA);
    b = {};
    b.insert(b.begin(), res.begin() + totalSizeOfA, res.end());
}

int main() 
{
    vector<long long> A = {1, 1, 3, 6, 9, 9};
    vector<long long> B = {1, 2, 3, 4, 8};
    mergeTwoSortedArraysWithoutExtraSpace(A, B);
    cout << "Final" << "\n";
    for (int i = 0; i < A.size(); i++) {
        cout << A[i] << " ";
    }
    cout << "\n";
    for (int i = 0; i < B.size(); i++) {
        cout << B[i] << " ";
    }
    cout << "\n";
}