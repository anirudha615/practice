#include <unordered_map>
#include <unordered_set>
#include <iostream>
using namespace std;

/**
 * Get the number of vowels for the given set of queries
 * 
 * 1. We will loop through the word and create a map of index to vowel count.
 * 2. For every query, we will check the value in the prefixMap[j] - prefixMap[i-1] (We need to include both sides of query)
 */
vector<int> getVowelCount(const string& word, const vector<std::pair<int, int>> queries) {
    std::unordered_set<char> vowels {'a', 'e', 'i', 'o', 'u'};
    std::unordered_map<int, int> myMap;
    myMap[-1] = 0;
    vector<int> result;

    for (int i=0; i<word.size(); i++) {
        if (vowels.count(word[i])) {
            myMap[i] = myMap[i-1] + 1;
        } else {
            myMap[i] = myMap[i-1];
        }
    }

    for (std::pair<int, int> query : queries){
        result.push_back(myMap[query.second] - myMap[query.first - 1]);
    }

    return result;
}

int main ()
{
    string word = "prefixSum";
    vector<std::pair<int, int>> queries = {{0, 2}, {1, 4}, {3, 5}};
    auto res = getVowelCount(word, queries);
    for (auto r : res)
    {
        std::cout << r << std::endl;
    }
}