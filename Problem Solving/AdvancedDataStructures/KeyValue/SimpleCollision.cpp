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
    int m_value;
    LinkedListNode* m_next = nullptr;
    LinkedListNode(int value) : m_value(value) {}
};

/**
 * Logic for PUT - When we add something as key/value pair,  find the bucket and add that value to the tail and make that value as tail.
 * Logic for GET - Find the bucket and retrieve the tail. If exists, return the value of the tail 
 */
class KeyValueSimpleCollision {
private:
    std::vector<LinkedListNode*> bucketList {2, nullptr};
public:

    int getFromKeyValuePair(string key) {
        int bucket = key.length() % bucketList.size();
        LinkedListNode* tail = bucketList.at(bucket);
        if (tail) {
            return tail->m_value;
        }
        return -1;
    }

    void putIntoKeyValuePair(string key, int value) {
        int bucket = key.length() % bucketList.size();
        LinkedListNode* node = new LinkedListNode(value);
        addValueToTail(bucket, node);
    }

    void addValueToTail(int bucket, LinkedListNode* nodeToBeAdded) {
        LinkedListNode* tail = bucketList.at(bucket);

        // If tail is nullptr, nodeToBeAdded will be the new tail
        if (!tail) {
            tail = nodeToBeAdded;
        } else {
            // If tail exists, connect nodeToBeAdded to the existing tail and make nodeToBeAdded to be the new tail
            nodeToBeAdded->m_next = tail;
            tail = nodeToBeAdded;
        }

        // Update the bucketList with the new tail
        bucketList[bucket] = tail;
    }
};


int main(){
    KeyValueSimpleCollision hashMap;
    
    cout << "=== Simple HashMap with Collision Test ===" << endl;
    cout << "Hash function: key.length() % 2" << endl;
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
    cout << "   GET('cat'): " << hashMap.getFromKeyValuePair("cat") << " (should be 200 - most recent in bucket 1)" << endl;
    cout << "   GET('dog'): " << hashMap.getFromKeyValuePair("dog") << " (should be 200 - most recent in bucket 1)" << endl;
    cout << "   GET('bird'): " << hashMap.getFromKeyValuePair("bird") << " (should be 300 - only item in bucket 0)" << endl;
    
    // Test 3: More collisions in bucket 1
    cout << "\n3. More collisions in bucket 1:" << endl;
    hashMap.putIntoKeyValuePair("ant", 400);      // length=3, bucket=1 (collision!)
    cout << "   PUT('ant', 400) - bucket 1 (another collision)" << endl;
    
    cout << "   GET('cat'): " << hashMap.getFromKeyValuePair("cat") << " (should be 400 - newest in bucket 1)" << endl;
    cout << "   GET('dog'): " << hashMap.getFromKeyValuePair("dog") << " (should be 400 - newest in bucket 1)" << endl;
    cout << "   GET('ant'): " << hashMap.getFromKeyValuePair("ant") << " (should be 400 - newest in bucket 1)" << endl;
    
    // Test 4: Bucket 0 collision
    cout << "\n4. Testing bucket 0 collision:" << endl;
    hashMap.putIntoKeyValuePair("fish", 500);     // length=4, bucket=0 (collision with bird)
    cout << "   PUT('fish', 500) - bucket 0 (collision with bird)" << endl;
    
    cout << "   GET('bird'): " << hashMap.getFromKeyValuePair("bird") << " (should be 500 - newest in bucket 0)" << endl;
    cout << "   GET('fish'): " << hashMap.getFromKeyValuePair("fish") << " (should be 500 - newest in bucket 0)" << endl;
    
    // Test 5: Non-existent keys (empty bucket simulation)
    cout << "\n5. Non-existent key test:" << endl;
    // No keys of length 0, 2, 6, etc. have been added
    cout << "   GET(''): " << hashMap.getFromKeyValuePair("") << " (length=0, bucket=0, should return 500)" << endl;
    cout << "   GET('no'): " << hashMap.getFromKeyValuePair("no") << " (length=2, bucket=0, should return 500)" << endl;
    cout << "   GET('x'): " << hashMap.getFromKeyValuePair("x") << " (length=1, bucket=1, should return 400)" << endl;
    
    cout << "\n=== HashMap Collision Test Completed ===" << endl;
    cout << "Note: All keys that hash to the same bucket return the most recent value stored in that bucket." << endl;
    
    return 0;
}
