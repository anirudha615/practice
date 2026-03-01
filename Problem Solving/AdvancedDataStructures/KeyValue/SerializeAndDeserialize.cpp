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
 * {"key:1": "val=ue", "k\ney": "v"}
 * 
 * Key and value can have delimiters. 
 * 
 * Length-Prefix Serialization/DeSerialization
 * 
 * Serialize Logic - For every key/value pair, contains the length of the key and append to <Length>:<Key>:<Length>:<Value> and this will be your complete string
 * Deserialize Logic - Retrieve the length and use the length to retrieve key/value
 * 
 * Link: https://www.hellointerview.com/community/questions/kv-serialize-deserialize/cm6xw6unw00003b6qz67bpfaj
 *                     
 */
class SerializeKeyValuePair {
private:
    std::vector<string> serializedStrings;
    std::unordered_map<string, string> m_cache;
public:

    void putInCache(string key, string value) {
        m_cache[key] = value;
    }

    void serializeCache() {
        for (auto pair : m_cache) {
            string serializedString =  std::to_string(pair.first.length()) + ":" + pair.first + ":" + std::to_string(pair.second.length()) + ":" + pair.second;
            serializedStrings.push_back(serializedString);
        }

        for (auto s : serializedStrings) {
            std::cout << s << std::endl;
        }
    }

    void deSerializeStrings() {
        for (auto s : serializedStrings) {
            int currentPointer = 0;
            // STEP 1: Find the length of the key
            int lengthToExtractLength = 0;
            while (s.at(currentPointer + lengthToExtractLength) != ':') {
                lengthToExtractLength++;
            }
            int keyLength = std::stoi(s.substr(currentPointer, lengthToExtractLength));
            
            // STEP 2: Use the key length to extract the key string
            currentPointer = currentPointer + lengthToExtractLength + 1; // Jump 'Key Length' and : and land on 'Key String'
            string key = s.substr(currentPointer, keyLength);

            // STEP3: Find the length of the value 
            currentPointer = currentPointer + keyLength + 1; // Jump Key String and : and land on 'Value Length' 
            lengthToExtractLength = 0;
            while (s.at(currentPointer + lengthToExtractLength) != ':') {
                lengthToExtractLength++;
            }
            int valueLength = std::stoi(s.substr(currentPointer, lengthToExtractLength));

            // STEP 4: Use the value length to extract value
            currentPointer = currentPointer + lengthToExtractLength + 1; // Jump 'Value Length' and : and land on 'Value String'
            string value = s.substr(currentPointer, valueLength);

            if (m_cache.count(key) && m_cache[key] == value){
                std::cout << "Deserialization Complete for key : " << key << " AND " << "value : " << value << std::endl;
            }
        }
    }

};


int main() {
    SerializeKeyValuePair store;
    
    cout << "=== Serialize/Deserialize Key-Value Store Test ===" << endl;
    cout << "Testing length-prefixed serialization with special characters" << endl;
    
    // Test 1: Basic key-value pairs
    cout << "\n1. Basic key-value pairs:" << endl;
    store.putInCache("name", "John");
    store.putInCache("age", "25");
    store.putInCache("city", "NYC");
    
    // Test 2: Keys and values with special characters (from problem description)
    cout << "\n2. Special characters test:" << endl;
    store.putInCache("key:1", "val=ue");
    store.putInCache("k\ney", "v");
    
    // Test 3: More challenging special characters
    cout << "\n3. More special characters:" << endl;
    store.putInCache("config::db", "host=localhost:port=5432");
    store.putInCache("message\nwith\nnewlines", "Hello\nWorld\n!");
    store.putInCache("", "empty_key");
    store.putInCache("empty_value", "");
    
    // Test 4: Edge cases
    cout << "\n4. Edge cases:" << endl;
    store.putInCache("numbers:123", "symbols!@#$%^&*()");
    store.putInCache("spaces and tabs", "value\twith\ttabs");
    
    cout << "\n--- Serialization Output ---" << endl;
    store.serializeCache();
    
    cout << "\n--- Deserialization Test ---" << endl;
    store.deSerializeStrings();
    
    cout << "\n=== Test Completed ===" << endl;
    cout << "If all deserializations match, the implementation handles special characters correctly!" << endl;
    
    return 0;
}
