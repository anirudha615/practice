#include <iostream>
using namespace std;

/**
 * https://www.hellointerview.com/community/questions/string-integer-atoi/cm5eh7nrh04n8838o7rsq3465?level=SENIOR
 */
int myAtoi(string s) {
    string integerString = "";
    bool isNegative = false;
    bool hasSeenAnyChar = false;
    for (int index = 0; index < s.size(); index++) {
        char element = s.at(index);

        // Get rid of whitespaces
        if (element == ' ') {
            if (hasSeenAnyChar) {
                break;
            }
            continue;
        }

        // Record the negative sign
        if (element== '-') {
            if (hasSeenAnyChar) {
                break;
            }
            hasSeenAnyChar = true;
            isNegative = true;
            continue;
        } else if (element== '+') {
            if (hasSeenAnyChar) {
                break;
            }
            hasSeenAnyChar = true;
            continue;
        }

        int digit = element - '0';
        if (digit >=0 && digit <= 9) {
            hasSeenAnyChar = true;
            // Get rid of trailing zeroes
            if (integerString.empty() && !digit) {
                continue;
            }

            integerString += element;
        } else {
            break; // indication to stop reading further string.
        }
    }
    
    int integer = 0;
    for (int index = 0; index < integerString.size(); index++) {
    int digit = integerString.at(index) - '0';
    // Keep the range [INT32_MIN, INT32_MAX]
    if (integer + (digit * pow(10, (integerString.size() - index - 1))) > INT32_MAX) {
        return isNegative ? INT32_MIN : INT32_MAX;
    }
    integer += digit * pow(10, (integerString.size() - index - 1));
    }

    return isNegative ? -integer : integer;
}