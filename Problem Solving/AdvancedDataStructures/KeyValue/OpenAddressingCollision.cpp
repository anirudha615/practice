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

struct Node {
    string m_key;
    int m_value;
    Node(const string& key, const int& value): m_key(key), m_value(value) {};
};

class KeyValueOpenAddressingCollision {
private:
    int64_t m_bucketSize;
    int64_t m_numElements = 0;
    const float LOAD_FACTOR_THRESHOLD = 0.9f;  // Trigger rehash when the bucket is 90% full
    std::vector<std::shared_ptr<Node>> m_bucketList;

    int32_t getBucketIndex(const std::string& key) {
        std::hash<string> hasher;
        return hasher(key) % m_bucketSize;
    }

    float getLoadFactor() const {
        return static_cast<float>(m_numElements) / m_bucketSize;
    }

    void rehash() {
        // STEP 1: Persist the old bucket information
        int64_t oldBucketSize = m_bucketSize;
        int64_t oldNumElements = m_numElements;
        std::vector<std::shared_ptr<Node>> oldBucket = m_bucketList;

        // STEP 2: Update the new bucket size and clean the bucket list
        m_bucketSize = oldBucketSize * 2;
        m_bucketList.resize(m_bucketSize, nullptr);
        m_numElements = 0;

        // STEP 2: Retrieve the node from old bucket index, get the index in the new bucket and place it there.
        for (int64_t index = 0; index < oldBucketSize; index++) {
            std::shared_ptr<Node> originalNode = oldBucket[index];
            if (!originalNode) {
                continue;
            }
            int32_t newBucketIndex = getBucketIndex(originalNode->m_key);
            int32_t startIndex = newBucketIndex;
            std::shared_ptr<Node> nodeInNewBucket = m_bucketList[newBucketIndex];
            while (nodeInNewBucket) {
                newBucketIndex = (newBucketIndex + 1) % m_bucketSize; // so that it does not crash at the next line
                if (newBucketIndex == startIndex) {
                    break; // break if it reached the starting index as you completed the entire round scan
                }
                nodeInNewBucket = m_bucketList.at(newBucketIndex);
            }
            m_bucketList[newBucketIndex] = originalNode;
            ++m_numElements;
        }

        if (m_numElements == oldNumElements) {
            std::cout << "Rehashing is successful" << std::endl;
        } else {
            std::cout << "Rehashing failed" << std::endl;
        }
    }
public:

    KeyValueOpenAddressingCollision(int bucketSize) : m_bucketSize(bucketSize) {
        m_bucketList.resize(m_bucketSize, nullptr);
    }

    // O(N)
    int getFromKeyValuePair(const string& key) { 
        int32_t bucketIndex = getBucketIndex(key);
        int32_t startIndex = bucketIndex;
        std::shared_ptr<Node> node = m_bucketList.at(bucketIndex);
        while (node) {
            if (node->m_key == key) {
                return node->m_value;
            }
            bucketIndex = (bucketIndex + 1) % m_bucketSize; // so that it does not crash at the next line
            if (bucketIndex == startIndex) {
                break; // break if it reached the starting index as you completed the entire round scan
            }
            node = m_bucketList.at(bucketIndex);
        }
        return -1;
    }

    void putIntoKeyValuePair(const string& key, const int& value) {
        int32_t bucketIndex = getBucketIndex(key);
        int32_t startIndex = bucketIndex;
        std::shared_ptr<Node> node = m_bucketList.at(bucketIndex);

        // STEP 1: If something exists in the index, loop through if the key exists to update the value.
        while (node) {
            if (node->m_key == key) {
                node->m_value = value;
                return;
            }
            bucketIndex = (bucketIndex + 1) % m_bucketSize; // so that it does not crash at the next line
            if (bucketIndex == startIndex) {
                break; // break if it reached the starting index as you completed the entire round scan
            }
            node = m_bucketList.at(bucketIndex);
        }

        // If nothing exists in the index or there is no key for the request even after facing collision
        node = std::make_shared<Node>(key, value);
        m_bucketList[bucketIndex] = node;
        ++m_numElements;

        // STEP 4: Check if the load factor exceeded and if yes, re-hash.
        if (getLoadFactor() > LOAD_FACTOR_THRESHOLD) {
            rehash();
        }
    }
};

int main() {
    cout << "=== HashMap Test (start size = 2) ===" << endl;

    // Test 1: Basic PUT/GET + first rehash
    {
        KeyValueOpenAddressingCollision map(2);
        cout << "\nTest 1: Basic PUT/GET" << endl;

        map.putIntoKeyValuePair("first", 100);   // load 1/2
        map.putIntoKeyValuePair("second", 200);  // triggers rehash to 4
        map.putIntoKeyValuePair("third", 300);   // load 3/4

        cout << "GET('first'): "  << map.getFromKeyValuePair("first")  << " (expected 100)" << endl;
        cout << "GET('second'): " << map.getFromKeyValuePair("second") << " (expected 200)" << endl;
        cout << "GET('third'): "  << map.getFromKeyValuePair("third")  << " (expected 300)" << endl;
    }

    // Test 2: Update existing key
    {
        KeyValueOpenAddressingCollision map(2);
        cout << "\nTest 2: Update existing key" << endl;

        map.putIntoKeyValuePair("key", 10);
        map.putIntoKeyValuePair("key", 20);  // update
        map.putIntoKeyValuePair("key", 30);  // update again

        cout << "GET('key'): " << map.getFromKeyValuePair("key") 
             << " (expected 30)" << endl;
    }

    // Test 3: Non-existent key + small rehash sequence
    {
        KeyValueOpenAddressingCollision map(2);
        cout << "\nTest 3: Missing keys + rehash" << endl;

        map.putIntoKeyValuePair("a", 1);
        map.putIntoKeyValuePair("b", 2);   // will rehash to 4
        map.putIntoKeyValuePair("c", 3);

        cout << "GET('a'): "      << map.getFromKeyValuePair("a")      << " (expected 1)" << endl;
        cout << "GET('missing'): "<< map.getFromKeyValuePair("missing")<< " (expected -1)" << endl;
    }

    return 0;
}
