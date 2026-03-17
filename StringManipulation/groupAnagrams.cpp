#include <iostream>
using namespace std;

vector<vector<string>> groupAnagrams(vector<string>& strs) {
    std::vector<std::vector<std::string>> results;
    std::unordered_map<std::string, std::vector<std::string>> groups;
    
    for (auto& str : strs) {
        string sortedStr = str;
        std::sort(sortedStr.begin(), sortedStr.end());
        groups[sortedStr].push_back(str);
    }

    for (std::pair<std::string, std::vector<std::string>> keyValue : groups) {
        results.push_back(keyValue.second);
    }
    return results;
}

int main() {
    std::vector<string> input {"eat","tea","tan","ate","nat","bat"};
    vector<vector<string>> results = groupAnagrams(input);
    for (vector<string> res : results) {
        for (auto str : res) {
            std::cout << str << " ," ;
        }
        std::cout << std::endl;
    }
}