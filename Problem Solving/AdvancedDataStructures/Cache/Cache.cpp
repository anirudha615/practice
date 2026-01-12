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

struct SelfBalancingNode{
    int m_key;
    int m_value;
    int64_t m_expirationtime;
    bool m_isDead;
    SelfBalancingNode() {}
    SelfBalancingNode(int key, int value, int64_t expirationTime) 
    : m_key(key),
    m_value(value),
    m_expirationtime(expirationTime),
    m_isDead(false) {}

    const bool operator<(const SelfBalancingNode& otherNode) const {
        return m_expirationtime < otherNode.m_expirationtime; // Ascending
    }
};

// LRU and LFU
struct DoublyLinkedListNode {
    int m_key;
    int m_value;
    int m_frequency; // For LFU
    DoublyLinkedListNode* m_prevNode;
    DoublyLinkedListNode* m_nextNode;
    DoublyLinkedListNode(int key, int value) 
    : m_key(key),
    m_value(value),
    m_frequency(1),
    m_prevNode(nullptr),
    m_nextNode(nullptr) {}
};

class ICache {
public:
    virtual ~ICache() = default;
    virtual int getValueFromCache(int key) = 0;
    virtual void putValueIntoCache(int key, int value, int64_t expirationTime) = 0;
    virtual void deletekeyFromCache(int key) = 0;
    virtual void runTest() = 0;
};

/**
 * We will use Map<Key, Node> where Node will contain the value and Node will be part of 1 DoublyLinkedList.
 * For DoublyLinkedList, we will maintain head and tail 
 * 
 * PUT Logic - Anytime a key/value pair is asked to be PUT,
 *   a. If the key exists in the map --> (1) update the value of the node, 
 *      (2) connect its prev and next and (3) Put the node at the tail.
 *   b. If the key does not exist in the map, (1) add the key to the cache, 
 *      (2) increase the current capacity and (3) put that node at the tail.
 *   c. If the current capacity increases total capacity, (1) decrement current capacity 
 *      and (2) detach the head 
 * 
 * GET Logic - Anytime we need to fetch the key from the cache,
 *    a. If the node exists in the map, (1) connect its prev and next, 
 *      (2) Put the node at the tail and (3) return the value.
 *    b. If the node does not exist in the map, return -1.
 * 
 * GET and PUT and DELETE operation is O(1)
 * 
 * Link: https://www.hellointerview.com/community/questions/lru-cache/cm5eh7nrh04r2838os7lk8nwf
 */
class LeastRecentlyUsedCache : public ICache {
private:
    DoublyLinkedListNode* m_tail = nullptr;
    DoublyLinkedListNode* m_head = nullptr;
    std::unordered_map<int, DoublyLinkedListNode*> m_cache;
    int m_totalCapacity;
    int m_currentCapacity = 0;
    std::mutex lock_cache;
public:

    LeastRecentlyUsedCache(int totalCapacity) : m_totalCapacity(totalCapacity) {}

    // Get but without affecting recency
    int peekValueFromCache(int key) {
        std::lock_guard<std::mutex> lock(lock_cache);
        // Key does not exist
        if (!m_cache.count(key)) {
            return -1;
        }
        return m_cache[key]->m_value;
    }

    int getValueFromCache(int key) {
        std::lock_guard<std::mutex> lock(lock_cache);
        if (!m_cache.count(key)) {
            return -1;
        }

        auto node = m_cache[key];
        addNodeToTail(m_cache[key]);
        return node->m_value;
    }

    void deleteKeyFromCache(int key) {
        std::lock_guard<std::mutex> lock(lock_cache);
        if (!m_cache.count(key)) {
            return;
        }
        auto node = m_cache[key];

        // STEP 1 : Delete the key from the map
        m_cache.erase(key);

        // STEP 2: Connect the prev and next of the pointer or if it is head or tail, mark it successor as head or tail.
        if (node->m_prevNode && node->m_nextNode) {
            // Node is in the middle
            node->m_prevNode->m_nextNode = node->m_nextNode;
            node->m_nextNode->m_prevNode = node->m_prevNode;
        } else if (!node->m_prevNode && node->m_nextNode) {
            // Node is the tail
            m_tail = node->m_nextNode;
            node->m_nextNode = nullptr;
            m_tail->m_prevNode = nullptr;
        } else if (node->m_prevNode && !node->m_nextNode) {
            // Node is the head
            m_head = node->m_prevNode;
            node->m_prevNode = nullptr;
            m_head->m_nextNode = nullptr;
        } else {
            // node is both head or tail;
            m_head = nullptr;
            m_tail = nullptr;
        }
        delete node;
        --m_currentCapacity;
    }

    void putValueIntoCache(int key, int value, int64_t expirationTime) {
        std::lock_guard<std::mutex> lock(lock_cache);
        // If key exists in the map, retrieve the node. 
        DoublyLinkedListNode* node = nullptr;
        if (m_cache.count(key)) {
            node = m_cache[key];
            node->m_value = value;  // Update the value for existing key
        } else {
            // Else put the newly created node into cache and increment the current capacity
            m_currentCapacity++;
            node = new DoublyLinkedListNode(key, value);
            m_cache[key] = node;
        }
        // Irrespective of whether node exists or not, put that node at the tail
        addNodeToTail(node);
        // Safety check - If current capacity exceeds total capacity, remove from the head
        if (m_currentCapacity > m_totalCapacity) {
            m_currentCapacity--;
            updateCapacity();
        }
    }

    void addNodeToTail(DoublyLinkedListNode* node) {
        // STEP 1: First update the node's prev and next pointers
        if (node->m_prevNode && node->m_nextNode) {
            // Node is in the middle
            node->m_prevNode->m_nextNode = node->m_nextNode;
            node->m_nextNode->m_prevNode = node->m_prevNode;
        } else if (node->m_prevNode && !node->m_nextNode) {
            // Node is the head
            node->m_prevNode->m_nextNode = nullptr;
            m_head = node->m_prevNode;  // Update head pointer to the new head
            if (m_head) {
                m_head->m_nextNode = nullptr;
            }
        } else if (node->m_nextNode && !node->m_prevNode) {
            // node is already tail
            return;
        }
        

        // Proceed to add that node to the tail
        node->m_nextNode = m_tail;
        node->m_prevNode = nullptr;
        if (m_tail) {
            m_tail->m_prevNode = node;
        }
        m_tail = node;

        // If head is null but tail exists, that means the element is 1. Assign that node to head also.
        if (m_tail && !m_head) {
            m_head = m_tail;
        }
    }

    /**
     * Detach the head and update head and tail.
     */
    void updateCapacity() {
        DoublyLinkedListNode* nodeToBeDeleted = m_head;
        m_head = nodeToBeDeleted->m_prevNode;
        nodeToBeDeleted->m_prevNode = nullptr;
        if (m_head) {
            m_head->m_nextNode = nullptr;
        }
        m_cache.erase(nodeToBeDeleted->m_key);
        m_currentCapacity--;
        delete nodeToBeDeleted;
       
        // If head exists but tail is nullptr, there is 1 element and assign it as head and tail.
        if (m_head && !m_tail) {
            m_tail = m_head;
        }
    }

    // Method to print the doubly linked list structure with arrows
    void printDLLState() {
        std::lock_guard<std::mutex> lock(lock_cache);
        
        cout << "   Current DDL State: ";
        if (m_currentCapacity == 0) {
            cout << "[EMPTY]" << endl;
            return;
        }
        
        cout << "TAIL(most recent)";
        DoublyLinkedListNode* current = m_tail;
        while (current) {
            cout << " -> [" << current->m_key << ":" << current->m_value << "]";
            current = current->m_nextNode;  // Follow next pointer from tail to head
        }
        cout << " -> HEAD(least recent)" << endl;
    }

    // Simple test method to demonstrate LRU cache functionality
    void runTest() {
        cout << "=== LRU Cache Test ===" << endl;
        cout << "Cache capacity: " << m_totalCapacity << endl;
        printDLLState();
        
        // Test 1: Basic PUT operations
        cout << "\n1. Testing PUT operations:" << endl;
        putValueIntoCache(1, 100, 0);
        cout << "   PUT(1, 100) - Cache size: " << m_cache.size() << endl;
        printDLLState();
        
        putValueIntoCache(2, 200, 0);
        cout << "   PUT(2, 200) - Cache size: " << m_cache.size() << endl;
        printDLLState();
        
        putValueIntoCache(3, 300, 0);
        cout << "   PUT(3, 300) - Cache size: " << m_cache.size() << endl;
        printDLLState();
        
        // Test 2: Basic GET operations
        cout << "\n2. Testing GET operations:" << endl;
        cout << "   GET(1): " << getValueFromCache(1) << endl;
        printDLLState();
        cout << "   GET(2): " << getValueFromCache(2) << endl;
        printDLLState();
        cout << "   GET(3): " << getValueFromCache(3) << endl;
        printDLLState();
        cout << "   GET(999): " << getValueFromCache(999) << " (should be -1)" << endl;
        
        // Test 3: LRU eviction
        cout << "\n3. Testing LRU eviction (adding 4th element):" << endl;
        cout << "   Before PUT(4, 400) - GET(1): " << getValueFromCache(1) << " (accessing key 1)" << endl;
        printDLLState();
        putValueIntoCache(4, 400, 0);
        cout << "   PUT(4, 400) - should evict least recently used" << endl;
        printDLLState();
        cout << "   After eviction - GET(1): " << getValueFromCache(1) << endl;
        printDLLState();
        cout << "   After eviction - GET(2): " << getValueFromCache(2) << " (should be -1, evicted)" << endl;
        printDLLState();
        cout << "   After eviction - GET(3): " << getValueFromCache(3) << endl;
        printDLLState();
        cout << "   After eviction - GET(4): " << getValueFromCache(4) << endl;
        printDLLState();
        
        // Test 4: Update existing key
        cout << "\n4. Testing update existing key:" << endl;
        putValueIntoCache(3, 999, 0);
        cout << "   PUT(3, 999) - updating existing key" << endl;
        printDLLState();
        cout << "   GET(3): " << getValueFromCache(3) << " (should be 999)" << endl;
        printDLLState();
        
        cout << "\n=== Test Completed ===" << endl;
    }
};

/**
 * 1. We will use the previous LRU concept of Map<Key, Node> where Node will contain value and frequency.
 * 2. Instead of having 1 doubly linked list as of LRU, we will have N number of doubly linked list which is equal to max Frequency.
 *    For that, we need Map<Frequency, <head, tail>> 
 * 
 * Frequently counter will increase if we do PUT and GET on the same key.
 * 
 * PUT logic - 
 *    1. If the key does not exist in the map, create a node with frequency 1 and add it to the DLL of frequency 1.
 *    2. If the key does exist in the map, (1) extract the node and increment the counter.
 *       (2) Don't add the node yet. Check if current capacity increases the total capacity, if yes, detach the head of the minFrequencyDLL and remove it from the map. 
 *           STEP 2 is important to not make future PUT operations obselete. 
 *       (2) connect prev and next of the old DLL (Check if DDL is empty, if yes, delete it and updateMinFrequency)
 *       (3) Add the modified node to the tail of the new DDL based on the frequency counter and updateMinFrequency (Check if DDL exists, if not create it)
 *   
 * 
 * GET logic - 
 *   1. If the key does not exist in the map, return -1.
 *   2. If the key exist in the map, (1) extract the node and increment the counter,
 *      (2) connect prev and next of the old DLL (Check if DDL is empty, if yes, delete it),
 *      (3) Add the modified node to the tail of the new DDL based on the frequency counter. (Check if DDL exists, if not create it)
 * 
 * GET and PUT operation is O(1)
 * 
 * Link: https://www.hellointerview.com/community/questions/lfu-cache/cm5eguhab02d6838o87irm4sz
 * 
 */
class LeastFrequentlyUsedCache : public ICache {
private:
    std::unordered_map<int, DoublyLinkedListNode*> m_cache;
    std::unordered_map<int, std::pair<DoublyLinkedListNode*, DoublyLinkedListNode*>> m_frequencyDLL;
    int m_totalCapacity;
    int m_currentCapacity = 0;
    int minFrequency = INT32_MAX;
    std::mutex lock_cache;
public:
    LeastFrequentlyUsedCache(int totalCapacity) : m_totalCapacity(totalCapacity) {}

    int getValueFromCache(int key) {
        std::lock_guard<std::mutex> lock(lock_cache);
        // Key does not exist
        if (!m_cache.count(key)) {
            return -1;
        } 
        // Key exists
        auto node = m_cache[key];
        int previousFrequency = node->m_frequency;
        node->m_frequency++;
        addNodeToTail(node, previousFrequency);

        return node->m_value;
    }

    void putValueIntoCache(int key, int value, int64_t expirationTime) {
        std::lock_guard<std::mutex> lock(lock_cache);
        DoublyLinkedListNode* node = nullptr;
        // Key does not exist
        if (!m_cache.count(key)) {
            ++m_currentCapacity;
            if (m_currentCapacity > m_totalCapacity) {
                updateCapacity();
            }
            node = new DoublyLinkedListNode(key, value);
            m_cache[key] = node;
            addNodeToTail(node, 0);
        } else {
            // Key exists
            node = m_cache[key];
            node->m_value = value;
            int previousFrequency = node->m_frequency;
            node->m_frequency++;
            addNodeToTail(node, previousFrequency);
        }
    }

    // In practice, this loop rarely runs more than 1-2 iterations
    void updateMinFrequency() {
        if (m_frequencyDLL.empty()) {
            minFrequency = INT32_MAX;
        } else {
            while (m_frequencyDLL.find(minFrequency) == m_frequencyDLL.end()) {
                minFrequency++;
            }
        }
        
    }

    /**
     * 3 responsibilities - 
     *    1. If the node exists in some DLL, connect prev and next of it. If DLL becomes empty, delete it.
     *    2. Add the node to DLL of its frequency and set head and tail appropriately
     *    3. Update minFrequency
     */
    void addNodeToTail(DoublyLinkedListNode* node, int previousFrequency) {
        // STEP 1: If previous frequency is not 0, then 
        // connect prev and next of the old DLL (Check if DDL is empty, if yes, delete it)
        if (previousFrequency) {
            std::pair<DoublyLinkedListNode*, DoublyLinkedListNode*>& head_tail = m_frequencyDLL[previousFrequency];
            if (node->m_prevNode && node->m_nextNode) {
              // Node is in middle
              node->m_prevNode->m_nextNode = node->m_nextNode;
              node->m_nextNode->m_prevNode = node->m_prevNode;
            } else if (!node->m_nextNode && node->m_prevNode) {
                // Node is the head, so detach it.
                head_tail.first = node->m_prevNode;
                node->m_prevNode = nullptr;
                if (head_tail.first) {
                    head_tail.first->m_nextNode = nullptr;
                }
            } else if (node->m_nextNode && !node->m_prevNode) {
                // Node is the tail, detach the tail.
                head_tail.second = node->m_nextNode;
                node->m_nextNode = nullptr;
                if (head_tail.second) {
                    head_tail.second->m_prevNode = nullptr;
                }   
            } else if (node == head_tail.first && node == head_tail.second) {
                head_tail.first = nullptr;
                head_tail.second = nullptr;
            }

            // If DLL is empty, delete the entry from m_frequencyDLL and updateMinFrequency Else update the head_tail
            if (!head_tail.first && !head_tail.second) {
                m_frequencyDLL.erase(previousFrequency);
                updateMinFrequency(); 
            }
        }

        // STEP 2: Irrespective of previous frequency, add the node to the tail of DDL with current frequency.
        // Check of the m_frequencyDLL has that frequency, if not create it.
        if (!m_frequencyDLL.count(node->m_frequency)) {
            m_frequencyDLL[node->m_frequency] = {nullptr, nullptr};
        }
        std::pair<DoublyLinkedListNode*, DoublyLinkedListNode*>& head_tail = m_frequencyDLL[node->m_frequency];
        // Add the node to the tail
        node->m_nextNode = head_tail.second;
        if (head_tail.second) {
            head_tail.second->m_prevNode = node;
        }
        head_tail.second = node;
        // head is null but tail is not null, head and tail are same.
        if (!head_tail.first && head_tail.second) {
            head_tail.first = head_tail.second;
        }

        // STEP 3: update the minFrequency
        minFrequency = std::min(minFrequency, node->m_frequency); 
    }

    /**
     * Get the DLL of the minFrequency
     * Detach the head and update the head and decrement the counter
     * Check if DLL is empty and if yes, updateMinFrequency
     * 
     */
    void updateCapacity() {
        std::pair<DoublyLinkedListNode*, DoublyLinkedListNode*>& head_tail = m_frequencyDLL[minFrequency];
        DoublyLinkedListNode* nodeToBeDeleted = head_tail.first;
        head_tail.first = nodeToBeDeleted->m_prevNode;
        nodeToBeDeleted->m_prevNode = nullptr;
        if (head_tail.first) {
            head_tail.first->m_nextNode = nullptr;
        }
        m_cache.erase(nodeToBeDeleted->m_key);
        m_currentCapacity--;
        delete nodeToBeDeleted;
        m_frequencyDLL[minFrequency] = head_tail;

        // If DLL is empty, delete the entry from m_frequencyDLL and updateMinFrequency.
        if (!head_tail.first && !head_tail.second) {
            m_frequencyDLL.erase(minFrequency);
            updateMinFrequency(); 
        }

        // head is null but tail is not null, head and tail are same.
        if (head_tail.first && !head_tail.second) {
            head_tail.second = head_tail.first;
        }
    }



    void printFrequencyState() {
        std::lock_guard<std::mutex> lock(lock_cache);
        cout << "   minFrequency: " << minFrequency << ", capacity: " << m_currentCapacity << "/" << m_totalCapacity << endl;
        cout << "   Frequency DDLs:" << endl;
        if (m_frequencyDLL.empty()) {
            cout << "   [NONE]" << endl;
            return;
        }
        
        for (auto& freq_pair : m_frequencyDLL) {
            int freq = freq_pair.first;
            auto& head_tail = freq_pair.second;
            cout << "   Freq" << freq << ": TAIL(recent)";
            
            DoublyLinkedListNode* current = head_tail.second; // Start from tail
            while (current) {
                cout << " -> [" << current->m_key << ":" << current->m_value << "]";
                current = current->m_nextNode;
            }
            cout << " -> HEAD(early)" << endl;
        }
    }

    void runTest() {
        cout << "=== LFU Cache Test ===" << endl;
        cout << "Cache capacity: " << m_totalCapacity << endl;
        
        // Test 1: Single Node Promotion
        cout << "\n1. Single Node Promotion Test:" << endl;
        putValueIntoCache(1, 100, 0);
        cout << "   PUT(1, 100)" << endl;
        printFrequencyState();
        
        cout << "   GET(1) - promote single node from freq 1 to 2" << endl;
        cout << "   GET(1): " << getValueFromCache(1) << endl;
        printFrequencyState();
        
        // Test 2: Multiple Single Node Promotions
        cout << "\n2. Multiple Promotions Test:" << endl;
        cout << "   GET(1) again - freq 2 to 3" << endl;
        cout << "   GET(1): " << getValueFromCache(1) << endl;
        printFrequencyState();
        
        // Test 3: Add more nodes and test minFrequency updates
        cout << "\n3. MinFrequency Update Test:" << endl;
        putValueIntoCache(2, 200, 0);
        putValueIntoCache(3, 300, 0);
        cout << "   PUT(2, 200), PUT(3, 300)" << endl;
        printFrequencyState();
        
        // Test 4: Promote all nodes from minFrequency
        cout << "\n4. Promote All From MinFreq Test:" << endl;
        cout << "   GET(2) and GET(3) - should update minFrequency from 1 to 2" << endl;
        cout << "   GET(2): " << getValueFromCache(2) << endl;
        printFrequencyState();
        cout << "   GET(3): " << getValueFromCache(3) << endl;
        printFrequencyState();
        
        // Test 5: LFU Eviction
        cout << "\n5. LFU Eviction Test:" << endl;
        cout << "   PUT(4, 400) - should evict key 2 of frequency 2" << endl;
        putValueIntoCache(4, 400, 0);
        printFrequencyState();
        cout << "   Verify key 1 still exists: GET(1) = " << getValueFromCache(1) << endl;
        printFrequencyState();
        
        // Test 6: Complex frequency scenario
        cout << "\n6. Complex Frequency Scenario:" << endl;
        cout << "   GET(4) to make it freq 2 alongside freq 3" << endl;
        cout << "   GET(4): " << getValueFromCache(4) << endl;
        printFrequencyState();
        
        cout << "\n=== Test Completed ===" << endl;
    }
};

/**
 * 1. We will use Map<Key, Node> where Node will contain value, expiration time and whether it is dead.
 * 2. We will not be using DLL because unlike LRU, LFU, we don't know where to add the recent PUT. 
 * 3. We will proceed with Self Balancing Trees (RBT/AVL)
 *      a. Since the problem has expiration time, it can fit anywhere, so we need to re-sort after every PUT.
 *      b. This is because when we hit total capacity, we need to remove the ones which expired or going to expire soon.
 *      c. Additionally, we also considered heap but heap would fail if I need to update the TTL as I have to find
 *         the existing element which is O(NLogN) and then reinsert the ones popped out (NLogN)
 * 
 * Logic for PUT - 
 *    1. If the key does not exist in the map, 
 *      (1) Add to cache and increment the capacity. 
 *      (2) Trigger Step 2.a and 2.b as cleanup mechanism
 *        (2.a) If capacity exceeds the total capacity, remove the top element from the SelfBalancingTrees. O(LogN)
 *        (2.b) We will trigger a clean up mechansim to remove all (or max threshold) expired elements from the SelfBalancingTrees. O(K LogN)
 *      (3) Add the node to the SelfBalancingTrees and it will self balance it. O(LogN)
 *    2. If the key exists in the map, 
 *       (1) Retrieve from the cache and create a copy of it.
 *       (2) Trigger cleanup mechanism similar to 2.a and 2.b
 *       (3) Find the node in the SelfBalancingTrees and delete it. O(LogN)
 *       (4) Add the node to the SelfBalancingTrees. O(LogN)
 * 
 * Logic for GET - 
 *     1. If the key does not exist in the map, return -1.
 *     2. If the key exist in the map, retrieve node. If expired, set it as Dead else return.
 * 
 * GET - O(1)
 * PUT - O(K LogN) K == constant ~= O(Log N)
 * 
 */
class TTLBasedCache: public ICache {
private:
    std::set<SelfBalancingNode> m_selfBalancingBST;
    std::unordered_map<int, std::set<SelfBalancingNode>::iterator> m_cache;
    int m_currentCapacity = 0;  // Initialize to 0
    int m_totalCapacity;
    std::mutex lock_cache;
public:
    TTLBasedCache(int totalCapacity) : m_totalCapacity(totalCapacity) {}

    int getValueFromCache(int key) {
        std::lock_guard<std::mutex> lock(lock_cache);  // Add thread safety
        if (!m_cache.count(key)) {
            return -1;
        }

        auto nodeIt = m_cache[key];
        if (nodeIt->m_expirationtime < getCurrentTimeInMillis()) {
            // Mark as dead by creating a copy, removing old, and inserting updated
            SelfBalancingNode updatedNode = *nodeIt;
            updatedNode.m_isDead = true;
            m_selfBalancingBST.erase(nodeIt);
            auto newIt = m_selfBalancingBST.insert(updatedNode);
            m_cache[key] = newIt.first;
            return -1;
        }

        return nodeIt->m_value;
    }

    void putValueIntoCache(int key, int value, int64_t expirationTime) {
        std::lock_guard<std::mutex> lock(lock_cache);
        
        // If key does not exist
        if (!m_cache.count(key)) {
            ++m_currentCapacity;
            SelfBalancingNode newNode(key, value, expirationTime);
            cleanup(); // O(k log n)
            auto result = m_selfBalancingBST.insert(newNode);
            m_cache[key] = result.first;  // Store iterator
        } else {
            // Key exists - remove old, insert new
            auto oldIt = m_cache[key];
            m_selfBalancingBST.erase(oldIt);  // Remove old version
            cleanup(); // O(k log n)
            SelfBalancingNode updatedNode(key, value, expirationTime);
            auto result = m_selfBalancingBST.insert(updatedNode);
            m_cache[key] = result.first;  // Update iterator
        }
    }

    /**
     * 1. Check if current capacity exceeds total and remove the top node.
     * 2. Remove all expired nodes or dead nodes - 
     * 
     * O(N LogN) 
     * 
     * You can also mention max 10 entries to be cleaned up every PUT operation - which makes complexity is O(LogN)
     */
    void cleanup() {
        if (m_currentCapacity > m_totalCapacity) {
            auto earliestIt = m_selfBalancingBST.begin(); // O(1) - iterator
            m_cache.erase(earliestIt->m_key);   // O(1) - remove from map first
            m_selfBalancingBST.erase(earliestIt); // O(log n) - remove from set
            m_currentCapacity--;
        }

        auto currentTime = getCurrentTimeInMillis();
        while (!m_selfBalancingBST.empty()) {
            auto earliestIt = m_selfBalancingBST.begin(); //O(1) - iterator to earliest
            if (earliestIt->m_expirationtime > currentTime && !earliestIt->m_isDead) {
                break;  // Stop only when entry is VALID (not expired AND not dead)
            }
            
            // Clean up properly:
            m_cache.erase(earliestIt->m_key); // O(1) - remove from map first
            m_selfBalancingBST.erase(earliestIt); //O(log n) - remove from set
            m_currentCapacity--;
        }
    }

    int64_t getCurrentTimeInMillis() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    
    void printTTLState() {
        std::lock_guard<std::mutex> lock(lock_cache);
        cout << "   Capacity: " << m_currentCapacity << "/" << m_totalCapacity << endl;
        cout << "   TTL Tree (earliest to latest expiration):" << endl;
        if (m_selfBalancingBST.empty()) {
            cout << "   [EMPTY]" << endl;
            return;
        }
        
        auto currentTime = getCurrentTimeInMillis();
        cout << "   Current time: " << currentTime << endl;
        cout << "   ";
        for (auto node : m_selfBalancingBST) {
            string status = "";
            if (node.m_isDead) status = "(DEAD)";
            else if (node.m_expirationtime < currentTime) status = "(EXPIRED)";
            else status = "(VALID)";
            
            cout << "[" << node.m_key << ":" << node.m_value << ", exp:" << node.m_expirationtime << status << "] ";
        }
        cout << endl;
    }

    void runTest() {
        cout << "=== TTL Cache Test ===" << endl;
        cout << "Cache capacity: " << m_totalCapacity << endl;
        
        int64_t currentTime = getCurrentTimeInMillis();
        
        // Test 1: Basic PUT with TTL (using much larger time spans)
        cout << "\n1. Basic PUT with TTL Test:" << endl;
        putValueIntoCache(1, 100, currentTime + 3600000);  // expires in 1 hour
        cout << "   PUT(1, 100, +1hour)" << endl;
        printTTLState();
        
        putValueIntoCache(2, 200, currentTime + 7200000); // expires in 2 hours  
        cout << "   PUT(2, 200, +2hours)" << endl;
        printTTLState();
        
        putValueIntoCache(3, 300, currentTime + 1800000);  // expires in 30 minutes (earliest)
        cout << "   PUT(3, 300, +30min)" << endl;
        printTTLState();
        
        // Test 2: GET operations with valid entries
        cout << "\n2. Testing GET operations:" << endl;
        cout << "   GET(1): " << getValueFromCache(1) << endl;
        cout << "   GET(2): " << getValueFromCache(2) << endl;
        cout << "   GET(3): " << getValueFromCache(3) << endl;
        cout << "   GET(999): " << getValueFromCache(999) << " (should be -1)" << endl;
        
        // Test 3: Simulate expiration by using past time
        cout << "\n3. Testing Expiration Logic:" << endl;
        putValueIntoCache(4, 400, currentTime + 1000);  // Expires in 1s
        cout << "   PUT(4, 400, +1s) " << endl;
        printTTLState();
        cout << "   Waiting 1 seconds for expiration..." << endl;
        this_thread::sleep_for(chrono::seconds(3));
        cout << "   GET(4): " << getValueFromCache(4) << " (should be -1, expired)" << endl;
        printTTLState();
        
        // Test 4: TTL Update for existing key
        cout << "\n4. Testing TTL Update:" << endl;
        putValueIntoCache(1, 150, currentTime + 10800000); // Update key 1 with new TTL (3 hours)
        cout << "   PUT(1, 150, +3hours) - update existing key" << endl;
        printTTLState();
        cout << "   GET(1): " << getValueFromCache(1) << " (should be 150)" << endl;
        
        // Test 5: Real-time expiration demonstration
        cout << "\n5. Real-time Expiration Test:" << endl;
        putValueIntoCache(5, 500, currentTime + 5000);  // expires in 5 seconds
        cout << "   PUT(5, 500, +5sec)" << endl;
        printTTLState();
        cout << "   GET(5) immediately: " << getValueFromCache(5) << " (should be 500)" << endl;
        cout << "   Waiting 6 seconds for expiration..." << endl;
        this_thread::sleep_for(chrono::seconds(6));
        cout << "   GET(5) after 6 seconds: " << getValueFromCache(5) << " (should be -1, expired)" << endl;
        printTTLState();
        
        // Test 6: Capacity eviction with cleanup
        cout << "\n6. Testing Capacity Eviction:" << endl;
        putValueIntoCache(6, 600, currentTime + 5400000);  // expires in 1.5 hours
        cout << "   PUT(6, 600, +1.5hours) - trigger cleanup" << endl;
        printTTLState();
        
        cout << "\n=== TTL Test Completed ===" << endl;
    }
};

class CacheFactoryPattern {
public:
    static std::unique_ptr<ICache> getCache(int capacity, const std::string& algorithm) {
        if (algorithm == "LRU") {
            return std::make_unique<LeastRecentlyUsedCache>(capacity);
        } else if (algorithm == "LFU") {
            return std::make_unique<LeastFrequentlyUsedCache>(capacity);
        } else if (algorithm == "TTL") {
            return std::make_unique<TTLBasedCache>(capacity);
        }
        return std::make_unique<LeastRecentlyUsedCache>(capacity);
    }
};

int main(){
    CacheFactoryPattern factory;
    std::unique_ptr<ICache> cache = factory.getCache(3, "TTL");
    cache->runTest();
    return 0;
}
