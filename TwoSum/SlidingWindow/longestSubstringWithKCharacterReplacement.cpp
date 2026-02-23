#include <iostream>
#include <map>
#include <vector>
#include <unordered_map>
#include <algorithm>

using namespace std;

/**
 * Window Invalid check should be the - 
 *   current substring collected - max Frequency of the character in the state > k
 */
int getLongestSubstringWithKCharactersReplacement(string word, int k) {
    std::unordered_map<char, int> state;
    std::string substring = "";
    int left = 0;
    int maxFrequencyCharacter = 0;
    int maxLength = 0;
    for (int right = 0; right < word.size(); right++) {
        char character = word.at(right);

        // Add to state
        state[character]++;
        maxFrequencyCharacter = std::max(maxFrequencyCharacter, state[character]);
        substring += character;

        // Check if the window is invalid
        while (substring.size() - maxFrequencyCharacter > k) {
            // Shrink the window
            substring.erase(substring.begin());
            state[word.at(left)]--;
            left++;
        }

        // Process the window
        maxLength = std::max<int>(maxLength, substring.size());
    }
    return maxLength;
}

int main() {
    std::cout << "result is : " << getLongestSubstringWithKCharactersReplacement("AABABBA", 1) << std::endl;
}
