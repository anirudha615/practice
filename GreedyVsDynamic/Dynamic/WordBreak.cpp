#include <iostream>
#include <unordered_set>
#include <cmath>
using namespace std;

/**
 * This is very similar to non-contiguous longest substring.
 * 
 * Fix the current index and check whether DP of substring from (0, previous index) is valid and the remaining substring (previous index +1, current index) exists in the dictioniary.
 * Previous index will loop till the current index
 * 
 * For example - helloWorld
 *  a. h -> false dp[1] = false
 *  b. e -> Check h (dp[1]) and e in dict and then he in dictioniary - If either of them is true, then dp[1] = true
 */
bool wordBreak(std::string s, std::vector<std::string> dict) {
    std::vector<bool> dp (s.size() + 1, false);

    // Base case of empty string (not the first character)
    dp[0] = true;

    // Start with the first character because you need dp of the first character
    for (int i = 1; i <= s.size(); i++) {
        // In the second loop, starts from 0 for the string
        for (int j = 0; j < i; j++) {
            std::string substring = s.substr(j, i-j);
            // If the DP of the jth index is true and the remaining substring from j to i exists, the dp[current index] = true
            if (dp[j] && std::find(dict.begin(), dict.end(), substring) != dict.end()) {
                dp[i] = true;
                break;
            }
        }
    }
    return dp[s.size()];
}


int main() {
    std::vector<std::string> dict {"a", "cd"};
    std::cout << wordBreak("aecd", dict) << std::endl;
}