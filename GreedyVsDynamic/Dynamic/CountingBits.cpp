#include <iostream>
#include <unordered_map>
#include <cmath>
using namespace std;

/**
 * Count the number of 1s in every number.
 * 
 * The trick is to come up with puzzle/logic - 
 *    0 - 0 (0 1s)
 *    1 - 1 (1 1s)
 *    2 - 01 (1 1s)
 *    3 - 11 (2 1s)
 *    4 - 100 (1 1s)
 *    5 - 101 (2 1s)
 *    6 - 110 (2 1s)
 *    7 - 111 (3 1s)
 *    8 - 1000 (1 1s)
 *    9 - 1001 (2 1s)
 *    10 - 1010 (2 1s)
 * 
 * Search for a pattern - 
 * 1. All even numbers - You will see the pattern of 1s = 1s in their half
 * 2. All odd numbers - You will see the pattern of 1s = number of 1s in the previous value + additonal 1
 * 
 */
std::vector<int> countBits(int target) {
     std::vector<int> dp (target+1, 0);

    for (int index = 1; index < target+1; index++) {
        if (index % 2) {
            // All odd numbers - You will see the pattern of 1s = number of 1s in the previous value + additonal 1
            dp[index] = dp[index - 1] + 1;
        } else {
            // All even numbers - You will see the pattern of 1s = 1s in their half
            dp[index] = dp[index/2];
        }
    }

    return dp; 
}

int main() {
    auto list = countBits(6);
    for (auto x : list) {
        std::cout << x << std::endl;
    }
}

