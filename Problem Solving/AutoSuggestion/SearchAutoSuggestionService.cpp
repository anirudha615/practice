#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <queue>
#include <vector>
#include <string>
#include <functional>
#include <algorithm>

using namespace std;

const int k = 3;

// Read words from file and build frequency map
unordered_map<string, int> buildWordFrequencyMap(const string& filename) {
    unordered_map<string, int> freqMap;
    ifstream file(filename);
    string word;

    if (!file.is_open()) {
        cerr << "Error: Cannot open file " << filename << endl;
        return freqMap;
    }

    while (file >> word) {
        ++freqMap[word];
    }

    return freqMap;
}

/**
 * Helper function to display the top K suggestions from a min heap
 * Used by all search auto suggestion implementations
 * 
 */
void convertMinToMaxHeap(priority_queue<pair<int, string>, vector<pair<int, string>>, greater<>>& topKMinHeap) {    
    // Store elements in a vector
    vector<pair<int, string>> suggestions;
    while (!topKMinHeap.empty()) {
        suggestions.push_back(topKMinHeap.top());
        topKMinHeap.pop();
    }
    
    // Print in reverse order (highest frequency first) using reverse iterator
    for (auto it = suggestions.rbegin(); it != suggestions.rend(); ++it) {
        cout << it->second << " " << it->first << endl;
    }
}

/**
 * Trie is a tree data structure where the parent can have multiple children.
 * It is useful for auto-complete suggestions.
 */
struct TrieNode {
    unordered_map<char, TrieNode*> children;
    priority_queue<pair<int, string>, vector<pair<int, string>>, greater<>> topKMinHeap; // caching at every node
    /** only leaf nodes */
    bool isValidWord;
    int frequency;
    string word;
    
};

class Trie {
public:
    TrieNode* root;

    Trie() {
        root = new TrieNode();
        root->isValidWord = false;
        root->frequency = 0;
        root->word = "";
    }

    /**
     * Complexity of the 'For loop' - O(N * LogK) where N is the length of the word and LogK is heapify operation.
     */
    void insert(const string& word, int frequency) {
        TrieNode* current = root;
        for (char c : word) {
            if (!current->children.count(c)) {
                current->children[c] = new TrieNode();
            }
            current = current->children[c];
            updateTopKMinHeap(current, word, frequency); // O(LogK)
        }
        current->isValidWord = true;
        current->frequency = frequency;
        current->word = word;
    }

    /**
     * O(LogK) where K is the size of the heap
     */
    void updateTopKMinHeap(TrieNode* node, const string& word, int frequency) {
        if (node->topKMinHeap.size() < k) {
            node->topKMinHeap.push({frequency, word});
        } else if (frequency > node->topKMinHeap.top().first) {
            node->topKMinHeap.pop();
            node->topKMinHeap.push({frequency, word});
        }
    }

    /**
     * O(L + K) where L is the length of the prefix and K is converting TopKMin to TopKMax 
     */
    void searchTrieReadFriendly(const string& prefix) {
        TrieNode* current = root;
        // O(L)
        for (char c : prefix) {
            if (!current->children.count(c)) {
                cout << "No suggestions found for prefix: " << prefix << endl;
                return;
            }
            current = current->children[c];
        }

        // O(K)
        convertMinToMaxHeap(current->topKMinHeap);
    }

    /**
     * DFS on the Children's children
     * Used by searchAutoSuggestionWithReadPenalty
     */
    void collectWordsAndBuildHeap(TrieNode* node, priority_queue<pair<int, string>, vector<pair<int, string>>, greater<>>& topKMinHeap) {
        if (node->isValidWord) {
            if (topKMinHeap.size() < k) {
                topKMinHeap.push({node->frequency, node->word});
            } else if (node->frequency > topKMinHeap.top().first) {
                topKMinHeap.pop();
                topKMinHeap.push({node->frequency, node->word});
            }
        }

        // O(N) is the length of the word and O(LogK) is heapify
        // Total complexity is O(N * LogK)
        for (const auto& child : node->children) {
            collectWordsAndBuildHeap(child.second, topKMinHeap); // O(LogK)
        }

    }
    
    /**
     * Hybrid Approach: Build TopK by collecting all the word from the node where prefix ended.
     * 
     * Overall: O(L + N * logK + K) ~= O(NLogK)
     */
    void searchTrieReadPenalty(const string& prefix) {
        TrieNode* current = root;
        
        // O(L) - Length of Prefix
        for (char c : prefix) {
            if (current->children.find(c) == current->children.end()) {
                cout << "No suggestions found for prefix: " << prefix << endl;
                return;
            }
            current = current->children[c];
        }
        
        
        priority_queue<pair<int, string>, vector<pair<int, string>>, greater<>> topKMinHeap;
        collectWordsAndBuildHeap(current, topKMinHeap); // O (N * LogK)
        
        // Display the results - O(K)
        convertMinToMaxHeap(topKMinHeap);
    }
};













/**
 * Naive Approach: Build TopK Heap while matching the prefix with the frequency map
 * O(M * N * LogK) time complexity
 */
void searchAutoSuggestionWithReadPenalty(unordered_map<string, int>& freqMap, const string& prefix) {
    priority_queue<pair<int, string>, vector<pair<int, string>>, greater<>> topKMinHeap;

    // Map is O(M) + SubString is O(N) + LogK is heapify
    // Total Complexity is O(M * N * LogK)
    for (const auto& entry : freqMap) {
        const string& word = entry.first;
        if (word.substr(0, prefix.length()) == prefix) {
            if (topKMinHeap.size() < k) {
               topKMinHeap.push({entry.second, word});
            } 
            else if (entry.second > topKMinHeap.top().first) {
                topKMinHeap.pop(); // the top element will always have the lesser value as compared to other
                topKMinHeap.push({entry.second, word});
            }
        }
    }

    // Display the results - O(K)
    convertMinToMaxHeap(topKMinHeap);
}

/** 
 * Read Friendly Approach: 
 * 1. Build Trie for every word in the frequency map. As you loop through every character, prepare TopK for that Trie Node.
 * 2. Repeat for every word.
 * 3. When the prefix is given, loop till the prefix is over and get the topK suggestions from that node.
 */
void searchAutoSuggestionWithWritePenaltyAndReadFriendly(unordered_map<string, int>& freqMap, const string& prefix) {
    Trie* trie_root = new Trie();
    
    // Map is O(M) and insert is O(N * LogK) where M = length of Map, N = length of word in a map and LogK = heapify
    // Total Complexity = O(M * N * LogK)
    for (const auto& entry : freqMap) {
        trie_root->insert(entry.first, entry.second); 
    }

    // O(L+K)
    trie_root->searchTrieReadFriendly(prefix);
}

/** 
 *  Read Penalty Approach:
 *    1. Build Trie for every word in the frequency map.
 *    2. When the prefix is given, loop till the prefix is over and retrieve the node.
 *    3. From that node, find all the children till you see 'isValidWord' in the last node.
 *    4. Collect those word and frequency and put them in the TopK Hash Map.
 * 
 * The difference between read penalty and write penalty is during read penalty you need to collect all the children
 * of the current node and then prepare a Top K suggestions.
 * 
 * Since just after writing the prefix, users would want to see the suggestion so write penalty is the right approach.
 */
void searchAutoSuggestionWithWriteFriendlyAndReadPenalty(unordered_map<string, int>& freqMap, const string& prefix) {
    Trie* trie_root = new Trie();
    
    // Map is O(M) and insert is O(N) where M = length of Map, N = length of word in a map
    // Total Complexity = O(M * N)
    for (const auto& entry : freqMap) {
        trie_root->insert(entry.first, entry.second);
    }


    trie_root->searchTrieReadPenalty(prefix);
}

/**
 * Design an auto-suggestion system that provides word suggestions as users type in a search box. 
 * The system should suggest words that start with the user's input prefix, rank suggestions by frequency, 
 * and display only the top K most relevant results. 
 * 
 * Consider the trade-offs between insertion efficiency, and query response time in your solution.
 */
int main() {
    string filename = "words.txt";
    unordered_map<string, int> freqMap = buildWordFrequencyMap(filename);

    cout << "Naive Approach (Preparing TopK while traversing the map):" << endl;
    //searchAutoSuggestionWithReadPenalty(freqMap, "appl"); // Read is O(M * N * LogK)

    cout << "\nRead Friendly Approach (Pre-computed TopK):" << endl;
    searchAutoSuggestionWithWritePenaltyAndReadFriendly(freqMap, "appl"); // Read is O(N+K)
    
    cout << "\nRead Penalty (On-demand TopK):" << endl;
    //searchAutoSuggestionWithWriteFriendlyAndReadPenalty(freqMap, "appl"); // Read is O(NLogK)

    return 0;
}
