#include <iostream>
#include <unordered_map>
#include <set>
#include <memory>
#include <thread>
#include <mutex>
#include <chrono>
#include <algorithm>
#include <cmath>

using namespace std;

/**
 * We know the entries to the same key will be appended to the list and it will be increasing monotonically.
 * 
 * Logic for PUT - If the key does not exist, create a list and add to it. If the key exists, append to the list.
 * 
 * Logic for GET - If the key does not exist, return -1. If the key exist, retrieve the list and do binary search on the 
 *                 list to find the entry whose expiration timestamp which is just <= current timestamp
 * 
 * Link: https://www.hellointerview.com/community/questions/time-key-value-store/cm5eguhad02rn838o3ttrkt0w
 */
class KeyValueNonExpiredCollision {
private:
    std::unordered_map<string, std::vector<std::pair<string, int64_t>>> m_cache;
public:

    string getValueFromCache(string key, int64_t queriedTime){
        if (!m_cache.count(key)) {
            return "";
        }

        std::vector<std::pair<string, int64_t>> listToBeSearched = m_cache[key];
        return valueReturnedFromBinarySearch(listToBeSearched, queriedTime);
    }

    string valueReturnedFromBinarySearch(std::vector<std::pair<string, int64_t>> list, int64_t queriedTime) {
        int leftIndex = 0;
        int rightIndex = list.size() - 1;
        int maxIndex = -1;

        while (leftIndex <= rightIndex) {
            int midIndex = (leftIndex+rightIndex)/2;
            
            // If the mid is <= queried, lets go further right to find the maxIndex which also satisfies this condition
            if (list.at(midIndex).second <= queriedTime) {
                maxIndex = std::max(maxIndex, midIndex);
                leftIndex = midIndex + 1;
            } else {
                // Since mid is > queried, move Left to find a slighter lesser value 
                rightIndex = midIndex - 1;
            }
        }

        if (maxIndex != -1) {
            return list.at(maxIndex).first;
        }
        
        return "";
    }

    void putValueIntoCache(string key, string value, int64_t expirationTime) {
        if (!m_cache.count(key)) {
            std::vector<std::pair<string, int64_t>> newList = {{value, expirationTime}};
            m_cache[key] = newList;
        } else {
            m_cache[key].push_back({value, expirationTime});
        }
    }
};


int main() {
    KeyValueNonExpiredCollision store;
    
    cout << "=== Time-Based Key-Value Store Test ===" << endl;
    cout << "Problem: Find largest timestamp <= queried timestamp for each key" << endl;
    
    // Test 1: Basic PUT operations with increasing timestamps
    cout << "\n1. Basic PUT operations:" << endl;
    store.putValueIntoCache("user1", "data_v1", 100);
    cout << "   PUT('user1', 'data_v1', timestamp=100)" << endl;
    
    store.putValueIntoCache("user1", "data_v2", 200);
    cout << "   PUT('user1', 'data_v2', timestamp=200)" << endl;
    
    store.putValueIntoCache("user1", "data_v3", 300);
    cout << "   PUT('user1', 'data_v3', timestamp=300)" << endl;
    
    // Test 2: GET operations with different query times
    cout << "\n2. Floor timestamp finding:" << endl;
    cout << "   GET('user1', 150): '" << store.getValueFromCache("user1", 150) << "' (should be 'data_v1' - timestamp 100)" << endl;
    cout << "   GET('user1', 200): '" << store.getValueFromCache("user1", 200) << "' (should be 'data_v2' - exact match)" << endl;
    cout << "   GET('user1', 250): '" << store.getValueFromCache("user1", 250) << "' (should be 'data_v2' - timestamp 200)" << endl;
    cout << "   GET('user1', 300): '" << store.getValueFromCache("user1", 300) << "' (should be 'data_v3' - exact match)" << endl;
    cout << "   GET('user1', 400): '" << store.getValueFromCache("user1", 400) << "' (should be 'data_v3' - latest available)" << endl;
    
    // Test 3: Edge case - query time before any timestamps
    cout << "\n3. Edge case - query before any data:" << endl;
    cout << "   GET('user1', 50): '" << store.getValueFromCache("user1", 50) << "' (should be empty - no timestamp <= 50)" << endl;
    
    // Test 4: Multiple keys
    cout << "\n4. Multiple keys test:" << endl;
    store.putValueIntoCache("user2", "profile_v1", 150);
    store.putValueIntoCache("user2", "profile_v2", 250);
    cout << "   PUT('user2', 'profile_v1', 150) and PUT('user2', 'profile_v2', 250)" << endl;
    
    cout << "   GET('user2', 175): '" << store.getValueFromCache("user2", 175) << "' (should be 'profile_v1')" << endl;
    cout << "   GET('user2', 300): '" << store.getValueFromCache("user2", 300) << "' (should be 'profile_v2')" << endl;
    
    // Test 5: Non-existent key
    cout << "\n5. Non-existent key test:" << endl;
    cout << "   GET('user3', 200): '" << store.getValueFromCache("user3", 200) << "' (should be empty - key doesn't exist)" << endl;
    
    // Test 6: Complex scenario with many timestamps
    cout << "\n6. Complex scenario:" << endl;
    store.putValueIntoCache("session", "login", 10);
    store.putValueIntoCache("session", "active", 20);
    store.putValueIntoCache("session", "idle", 30);
    store.putValueIntoCache("session", "timeout", 40);
    cout << "   PUT session data with timestamps: 10, 20, 30, 40" << endl;
    
    cout << "   GET('session', 5): '" << store.getValueFromCache("session", 5) << "' (before any data)" << endl;
    cout << "   GET('session', 15): '" << store.getValueFromCache("session", 15) << "' (should be 'login')" << endl;
    cout << "   GET('session', 25): '" << store.getValueFromCache("session", 25) << "' (should be 'active')" << endl;
    cout << "   GET('session', 35): '" << store.getValueFromCache("session", 35) << "' (should be 'idle')" << endl;
    cout << "   GET('session', 50): '" << store.getValueFromCache("session", 50) << "' (should be 'timeout')" << endl;
    
    cout << "\n=== Time-Based Key-Value Store Test Completed ===" << endl;
    
    return 0;
}
