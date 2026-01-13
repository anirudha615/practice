#include <iostream>
#include <map>
#include <vector>
#include <unordered_map>
#include <algorithm>

using namespace std;

int getLongestSubstringWithKRepeating(string word, int k) {
    std::unordered_map<char, int> state;
    int leftBoundary = 0;
    int maxLength = INT_MIN;
    for (int rightBoundary = 0; rightBoundary < word.size(); rightBoundary++) {
        state[word.at(rightBoundary)]++; // Update the state

        // Check if the window is invalid
        while (state.size() > k) {
            // Logic here - we can directly look if the current char has exceeded the map's size
            // and if yes, shorten the window until rule is met.
            state[word.at(leftBoundary)]--;
            if (!state[word.at(leftBoundary)]) {
                state.erase(word.at(leftBoundary));
            }
            ++leftBoundary;
        }

        // When the while loop exists, one of the element in the map is at most K.
        // Hence, process the window
        int length = rightBoundary - leftBoundary + 1;
        maxLength = std::max(maxLength, length);
    }

    return maxLength;
}

int main() {
    std::cout << "result is : " << getLongestSubstringWithKRepeating("eceba", 2) << std::endl;
}
