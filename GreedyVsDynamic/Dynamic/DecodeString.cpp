#include <iostream>
#include <unordered_map>
#include <cmath>
using namespace std;

bool decodeAsSingle(int digit) {
    return digit != 0;
}

bool decodeAsDouble(int digit) {
    return digit > 9 and digit < 27;
}

/**
 * You have map of Numbers to Alphabet from 1 --> A to 26 --> Z.
 * Find out all possible ways to decode string.
 * 
 * Puzzle - 
 *    1. Figure out the base cases -
 *       a. For a single non-zero digit, there is only 1 way to decode. For a single zero digit, there is no way to decode.
 *       b. For a double digit, there are 2 ways to decode if the digit is between 1 and 26. If it is 00 or above 26 but less than 99, there is no way or 1 way to decode it.
 *    2. So, for three digits, check if the last character can be decoded as single. If yes, then check the number of ways to decode the first 2 characters which is DP[2].
 *       Similarly, if the last 2 characters can be decoded as double, then check the number of ways we can decode the 1st character which is DP[1]
 * 
 * Since we are looping through the string 'ABC', DP[1] means how many ways we can decode the first character (A), DP[2] means how many ways we can decode the first 2 characters (AB).
 * Hence for third character, we fix the third char (C) and see how many ways we could decode AB and then fix 'BC' DP[2] and see how many ways we decode 'A' [DP1]
 * 
 * NOTE: If the last character can't be decoded on its own, then any decoding that tries to end with that single character will be invalid. 
 *       So all those dp[i-1] ways of decoding the prefix become useless for this particular ending.
 * 
 */
int decodeString(std::string number) {
    std:vector<int> dp (number.size(), 0);
    
    // Get the base cases
    if (number.size() >= 1) {
        int firstDigit = number.at(0) - '0';
        dp[0] = decodeAsSingle(firstDigit) ? 1 : 0;
    }

    if (number.size() >= 2) {
        int lastDigit = number.at(1) - '0';
        int firstTwoDigit = 10 * (number.at(0) - '0') + (number.at(1) - '0');
        dp[1] = (decodeAsSingle(lastDigit) ? dp[0] : 0) + (decodeAsDouble(firstTwoDigit) ? 1 : 0);
    }

    // Return 
    if (number.size() == 2) {
        return dp[1];
    } else if (number.size() == 1) {
        return dp[0];
    } else if (number.empty()){
        return 0;
    }

    for (int index = 2; index < number.size(); index++) {
        std::string substring = number.substr(0, index + 1);
        int lastDigit = substring.at(substring.size() - 1) - '0';
        int lastTwoDigit = 10 * (substring.at(substring.size() - 2) - '0') + substring.at(substring.size() - 1) - '0';
        dp[index] = (decodeAsSingle(lastDigit) ? dp[index - 1] : 0) + (decodeAsDouble(lastTwoDigit) ? dp[index - 2] : 0);
    }
    
    return dp[number.size() - 1];
}


int main() {
    int result = decodeString("150");
    std::cout << result << std::endl;
}

