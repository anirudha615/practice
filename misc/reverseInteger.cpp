#include <iostream>
using namespace std;

/**
 * 1. Collect the digits from unit place to infinite in a vector via % and /
 * 2. Loop the vector and build the reverse using pow of 10
 * 
 * Negatives are already captured by modulo 
 * The result of -11 % 10 is exactly -1.
 * the result of -11 / 10 is -1
 */
int reverse(int x) {
    std::vector<int32_t> digits;
    while (x) {
        digits.push_back(x%10);
        x = x/10;
    }
    // Digits in reversed

    int64_t reverse = 0;
    for (int index = 0; index < digits.size(); index++) {
        reverse += digits[index] * pow(10, digits.size()-1-index); // -1 because at place 3, you need 2 zeroes
        // Explicit check so that reverse is 
        if (reverse > INT32_MAX || reverse < INT32_MIN) {
            return 0;
        }
    }
    return reverse;
}

int main() {
    std::cout << reverse(123) << std::endl;
}