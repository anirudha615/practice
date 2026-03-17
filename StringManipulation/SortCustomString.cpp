#include <iostream>
using namespace std;

/**
 * https://www.hellointerview.com/community/questions/custom-sort-string/cm5eguhac02md838obkf6rxgc?level=STAFF
 * 
 * Logic - 
 *    1. Prepare a rank map from Order char's to index.
 *    2. Sort using custom comparator
 */
string customSortString(string order, string s) {
    // Build rank map: each character in order gets a rank (0, 1, 2, ...)
    unordered_map<char, int> rankMap;
    for (int index = 0; index < order.length(); index++) {
        rankMap[order[index]] = index;
    }
    
    // Sort using custom comparator
    vector<char> sVec(s.begin(), s.end());
    std::sort(sVec.begin(), sVec.end(), [&rankMap](char a, char b) {
        int rankA = rankMap.count(a) ? rankMap[a] : 26;
        int rankB = rankMap.count(b) ? rankMap[b] : 26;
        return rankA < rankB;
    });
    
    // Join sorted characters back into string
    string result(sVec.begin(), sVec.end());
    
    return result;
}

int main() {
    string order = "cba", s = "abcd";
    std::cout << customSortString(order,s) << std::endl;
}