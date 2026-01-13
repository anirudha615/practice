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

struct LinkedListNode {
    string m_key;
    int m_value;
    LinkedListNode* m_next = nullptr;
    LinkedListNode(string key, int value) : m_key(key), m_value(value) {}
};

/**
 * 
 * Simple Collision - Start with 2 buckets and no load factor
 *    1. PUT - When we add K/V pair, find the bucket index using bucket-modulo algorithm and add the K/V to that bucket.
 *       a. Since, we don't have a load factor, the size of bucket will keep on increasing which will make the PUT and GET equal to O(N).
 *    2. GET - Find bucket index using  bucket-modulo algorithm and retrieve the LL. Loop through LL and retrieve the value which is O(N)
 * 
 * Simple Collision + Re-hashing - 
 *    1. Let's introduce a load factor say 1 where if the number of elements == number of buckets so that we don't have to insert into linked list.
 *    2. However, if the load factor > 1, we trigger re-hash where we have to distribute the old bucket elements to the new bucket.
 * 
 */
class KeyValueChainCollision {
private:
    int64_t m_bucketSize;
    int64_t m_numElements = 0;
    const float LOAD_FACTOR_THRESHOLD = 1.0f;  // Trigger rehash at 100% load
    std::vector<LinkedListNode*> m_bucketList;

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
        std::vector<LinkedListNode*> oldBucket = m_bucketList;

        // STEP 2: Update the new bucket size and clean the bucket list
        m_bucketSize = oldBucketSize * 2;
        m_bucketList.resize(m_bucketSize, nullptr);
        m_numElements = 0;

        // STEP 2: For every key in every bucket of oldBucketList
        for (int64_t index = 0; index < oldBucketSize; index++) {
            LinkedListNode* tail = oldBucket[index];
            while (tail) {
                LinkedListNode* temp = tail->m_next;
                // Put the node in the new bucket's tail
                int32_t newBucketIndex = getBucketIndex(tail->m_key);
                tail->m_next = m_bucketList[newBucketIndex];
                m_bucketList[newBucketIndex] = tail;
                
                // Use tmp to advance through the oldBucket
                tail = temp;

                // Increase the number of elements
                ++m_numElements;
            }
        }

        if (m_numElements == oldNumElements) {
            std::cout << "Rehashing is successful" << std::endl;
        } else {
            std::cout << "Rehashing failed" << std::endl;
        }
    }
public:

    KeyValueChainCollision(int bucketSize) : m_bucketSize(bucketSize) {
        m_bucketList.resize(m_bucketSize, nullptr);
    }

    int getFromKeyValuePair(const string& key) {
        int32_t bucketIndex = getBucketIndex(key);
        LinkedListNode* tail = m_bucketList.at(bucketIndex);
        
        while (tail) {
            if (key == tail->m_key) {
                return tail->m_value;
            }
            tail = tail->m_next;
        };
        return -1;
    }

    void putIntoKeyValuePair(const string& key, const int& value) {
        int32_t bucketIndex = getBucketIndex(key);
        LinkedListNode* tail = m_bucketList.at(bucketIndex);
        // STEP 1: If the key exists, update the value
        while (tail) {
            if (key == tail->m_key) {
                tail->m_value = value;
                return;
            }
            tail = tail->m_next;
        };

        // STEP 2: If the key does not exists, add the K/V as the tail of the bucket index
        tail = m_bucketList.at(bucketIndex);
        LinkedListNode* node = new LinkedListNode(key, value);
        // If tail is nullptr, nodeToBeAdded will be the new tail
        if (!tail) {
            tail = node;
        } else {
            // If tail exists, connect nodeToBeAdded to the existing tail and make nodeToBeAdded to be the new tail
            node->m_next = tail;
            tail = node;
        }
        // Update the bucketList with the new tail
        m_bucketList[bucketIndex] = tail;

        // STEP 3: Increase the number of elements
        ++m_numElements;

        // STEP 4: Check if the load factor exceeded and if yes, re-hash.
        if (getLoadFactor() > LOAD_FACTOR_THRESHOLD) {
            rehash();
        }
    }
};


int main(){
    KeyValueChainCollision hashMap(2);
    
    cout << "=== Simple HashMap with Collision Test ===" << endl;
    cout << "2 buckets: [0] and [1]" << endl;
    
    // Test 1: Basic PUT operations
    cout << "\n1. Basic PUT operations:" << endl;
    hashMap.putIntoKeyValuePair("cat", 100);      // length=3, bucket=1
    cout << "   PUT('cat', 100) - bucket 1" << endl;
    
    hashMap.putIntoKeyValuePair("dog", 200);      // length=3, bucket=1 (collision!)  
    cout << "   PUT('dog', 200) - bucket 1 (collision with cat)" << endl;
    
    hashMap.putIntoKeyValuePair("bird", 300);     // length=4, bucket=0
    cout << "   PUT('bird', 300) - bucket 0" << endl;
    
    // Test 2: GET operations
    cout << "\n2. GET operations:" << endl;
    cout << "   GET('cat'): " << hashMap.getFromKeyValuePair("cat") << " (should be 100 - O(N))" << endl;
    cout << "   GET('dog'): " << hashMap.getFromKeyValuePair("dog") << " (should be 200 - O(1) as this is most recent)" << endl;
    cout << "   GET('bird'): " << hashMap.getFromKeyValuePair("bird") << " (should be 300 - O(1) as this is most recent)" << endl;
    
    // Test 3: More collisions in bucket 1
    cout << "\n3. More collisions in either bucket:" << endl;
    hashMap.putIntoKeyValuePair("ant", 400);
    cout << "   GET('ant'): " << hashMap.getFromKeyValuePair("ant") << " (should be 400 - newest in bucket 1)" << endl;
    
    return 0;
}