#include <iostream>
using namespace std;

int getDiff(char previousChar, char currentChar) {
    int diff = currentChar - previousChar;
    return (diff < 0) ? (diff + 26)%26 : diff%26;
}

/**
 * https://www.hellointerview.com/community/questions/group-shifted-strings/cm5eh7nrh04tx838olh23fjzs?level=MID_LEVEL
 * 
 * Consider 2 strings as identical - 
 *   a. If their length is same
 *   b. The diff between characters in the string are same.
 */
vector<vector<string>> groupStrings(vector<string>& strings) {
    std::unordered_map<string, std::vector<string>> m_state;
    for (string element : strings) {
        int size = element.size();
        string charDiff = "";
        if (size > 1) {
            for (int index = 1; index < size; index++) {
                char previousChar = element.at(index-1);
                char currentChar = element.at(index);
                charDiff += "_" + std::to_string(getDiff(previousChar, currentChar));
            }
        }
        string normalizedKey = std::to_string(size) + charDiff;
        m_state[normalizedKey].push_back(element);
    }

    std::vector<std::vector<string>> results;
    for (std::pair<string, std::vector<string>> keyValue : m_state) {
        results.push_back(keyValue.second);
    }
    return results;
}

int main() {
    vector<string> strings = {"abc", "bcd", "acef", "xyz", "az", "ba", "a", "z"};
    vector<vector<string>> result = groupStrings(strings);

    for (const auto& group : result) {
        cout << "[";
        for (int i = 0; i < group.size(); i++) {
            cout << group[i];
            if (i < group.size() - 1) cout << ", ";
        }
        cout << "]" << endl;
    }
    return 0;
}