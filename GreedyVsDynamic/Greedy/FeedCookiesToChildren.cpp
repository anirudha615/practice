#include <iostream>
#include <unordered_map>
#include <cmath>
using namespace std;

/**
 * Sort the greed/children and the cookies in ascending order.
 * The logic is to find cookies whose size is JUST greater or equal to the greed of the children (BEST FIT)
 */
int findContentChildren(vector<int>& greeds, vector<int>& cookies) {
    sort(greeds.begin(), greeds.end());
    sort(cookies.begin(), cookies.end());
    int satisfiedChildren = 0;
    int greedPointer = 0;
    int cookiePointer = 0;
    while (greedPointer < greeds.size() && cookiePointer < cookies.size()) {
        if (cookies.at(cookiePointer) >= greeds.at(greedPointer)) {
            satisfiedChildren++;
            greedPointer++;
        }
        cookiePointer++;
    }

    return satisfiedChildren;
}

int main() {
    std::vector<int> greeds {1, 3, 3, 4};
    std::vector<int> cookies {2, 2, 3, 4};
    std::cout << findContentChildren(greeds, cookies) << std::endl;
}

