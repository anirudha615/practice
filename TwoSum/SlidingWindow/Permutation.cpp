#include <iostream>

using namespace std;

/**
 * This is a fixed window problem
 * 1. Create a s1 state 
 * 2. Fixed window should be present in s2
 *      a. Add to s2 state only if it is present in s1 state
 *      b. Once the window size exceeds s1 size, remove from s2 state from left
 *      c. At every index, check if s1 == s2. 
 * 
 * https://www.hellointerview.com/community/questions/permutation-string/cm5eguhab02g5838ovs5uqris?level=SENIOR
 */
bool checkInclusion(string s1, string s2) {
    if (s2.size() < s1.size()) {
        return false;
    }

    // Map because 'aa' can be stored as a with 2 frequency. Both Maps should match char and frequency.
    std::unordered_map<char, int> s1State;
    for (char c : s1) {
        s1State[c]++;
    }

    int left = 0;
    std::unordered_map<char, int> s2State;
    for (int right = 0; right < s2.size(); right++) {

        // add it to state 
        if (s1State.count(s2.at(right))) {
            s2State[s2.at(right)]++;
        }

        // The permutation needs to be contiguous. If the window size exceeds the permutation string,
        // it is not contiguous. Hence shrink the window by updating the s2State. 
        if (right - left + 1 > s1.size()) {
            if (s2State.count(s2.at(left))) {
                s2State[s2.at(left)]--;
                if (!s2State[s2.at(left)]) {
                    s2State.erase(s2.at(left));
                }
            }
            left++;
        }

        // Process the window - If any point of time, s1 and s2 are equal, that means they are contiguous
        // ordering does not matter here.
        if (s1State == s2State) {
            return true;
        }
    }
    return false;
}

int main() {
    // Example 1: s1 = "ab", s2 = "eidbaooo"
    // "ba" is a permutation of "ab" and is a substring of s2
    // Expected: true
    cout << "Example 1: " << (checkInclusion("ab", "eidbaooo") ? "true" : "false") << "\n";

    // Example 2: s1 = "ab", s2 = "eidboaoo"
    // No permutation of "ab" is a substring of s2
    // Expected: false
    cout << "Example 2: " << (checkInclusion("ab", "eidboaoo") ? "true" : "false") << "\n";

    return 0;
}
