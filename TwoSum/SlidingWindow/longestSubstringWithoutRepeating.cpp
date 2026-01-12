#include <iostream>
#include <map>
#include <vector>
#include <unordered_map>
#include <algorithm>

using namespace std;

/**
 * 1. Window size is flexible 
 * 2. The window will be invalid if the values of any key in the map exceeds 1.
 * 3. Step 1: As soon as we loop, add that element in map
 * 4. Step 2: Check whether that element exceeded the threshold (window invalid)
 *    a. You don't have to check for all the states - Just the recent one.
 *    b. If the window is invalid, keep on shortening the window until you see if the recently added element count has reduced below the threshold.
 * 5. Step 3: Process the window (checking if the substring prepared is largest substring or not)
 */
std::vector<char> getLongestSubstringWithoutRepeatingCharacters(string word) {
    std::vector<char> substring; // Temporary
    std::vector<char> longestSubstring; // Final
    int left = 0;
    std::unordered_map<char, int> map;

    for (int right = 0; right < word.size(); right++) {
        map[word.at(right)]++;
        substring.push_back(word.at(right));

        //check when the window is invalid
        // TIP: You don't have to check all the keys of the map - Only the recent updated ones
        while (map[word.at(right)] > 1) {
            map[word.at(left)]--;
            left++;
            substring.erase(substring.begin());
        }

        //process the window
        if (longestSubstring.size() < substring.size()) {
            longestSubstring = substring;
        }
    }
    return longestSubstring;
}

int main() 
{
    //ohomm, abcabcbb, anviaj, pwwkew, tmmzuxt
    auto longestSequenceSubString = getLongestSubstringWithoutRepeatingCharacters("ohomm");
    for (auto chara : longestSequenceSubString)
    {
        std::cout << chara << " " << std::endl;
    }
    std::cout << "length is : " << longestSequenceSubString.size() << std::endl;
}
