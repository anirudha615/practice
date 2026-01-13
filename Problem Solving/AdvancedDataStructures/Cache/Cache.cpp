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
    virtual void deleteKeyFromCache(int key) = 0;
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

    int getValueFromCache(int key) override {
        std::lock_guard<std::mutex> lock(lock_cache);
        if (!m_cache.count(key)) {
            return -1;
        }

        auto node = m_cache[key];
        addNodeToTail(m_cache[key]);
        return node->m_value;
    }

    void deleteKeyFromCache(int key) override {
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

    void putValueIntoCache(int key, int value, int64_t expirationTime) override {
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
    void runTest() override {
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

    int getValueFromCache(int key) override {
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

    void putValueIntoCache(int key, int value, int64_t expirationTime) override {
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

    void deleteKeyFromCache(int key) {
        return;
    }
};

struct Node{
    int m_key;
    int m_value;
    int64_t m_expirationtime;
    bool m_isDead;
    std::shared_ptr<Node> next = nullptr;
    std::shared_ptr<Node> prev = nullptr;
    Node() {}
    Node(int key, int value, int64_t expirationTime) 
    : m_key(key),
    m_value(value),
    m_expirationtime(expirationTime),
    m_isDead(false) {}
};

struct Comparator {
    bool operator()(const std::shared_ptr<Node> nodeA, const std::shared_ptr<Node> NodeB) {
        return nodeA->m_expirationtime > NodeB->m_expirationtime; // Ascending
    }
};

/**
 * This is TTL Based Cache with LRU Eviction.
 * 
 * 1. We will maintain Map<Key, Node*> for cache.
 * 2. We will maintain DLL for capacity.
 * 3. We will maintain a minHeap for expiration time sorting
 * 
 * 1. GET call - O(1)
 *      a. If the node is expired, mark it as dead, remove from map and remove from DLL. All of these operations
 *         are O(1). You don't have to remove from MinHeap which can be taken care async using TimeBoundedCleanup.
 *      b. If the node is not expired, updateDLL and return the value
 * 
 * 2. PUT call - O(LogN)
 *      a. If a node exists, update its value, updateDLL and reinsert into minHeap. The heap will get polluted
 *         but during lazy cleanup, it should get cleared up.
 *      b. If a node does not exist, create a new node, updateDLL, updateMap, increase capacity and insert into MinHeap.
 *      c. Check if current capacity exceeds total capacity, if yes, mark head as dead, remove from map and remove from DLL.
 * 
 * 3. Delete - O(1)
 *      a. If a node exists, mark node as dead, remove from map and remove from DLL.
 *      b. Async cleanup will take care of removing the node.
 * 
 * 4. Cleanup - O(K LogN)
 *      a. While the heap is not empty or you have exceeded the nodes to clean up, only pop out nodes which are expired. 
 *      b. If a node is expired but not dead, remove from map and remove from DLL and pop from heap.
 *      c. If a node is expired and dead, you already removed from map and DLL. So, only remove from heap.
 *      d. For other cases, don't do anything.
 * 
 * 
 */
class TTLBasedCache: public ICache {
private:
    std::unordered_map<int, std::shared_ptr<Node>> m_cache;
    std::priority_queue<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>, Comparator> m_minHeap;
    std::shared_ptr<Node> m_tail; // Tail for LRU DLL
    std::shared_ptr<Node> m_head; // Head for LRU DLL
    int m_currentCapacity = 0;  // Initialize to 0
    int m_totalCapacity;
    std::mutex lock_cache;

    int64_t getCurrentTimeInMillis() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    void removeNodeFromDLL(const std::shared_ptr<Node> nodeToBeRemoved) {
        if (nodeToBeRemoved->prev && nodeToBeRemoved->next) {
            // nodeToBeRemoved is in the middle
            nodeToBeRemoved->prev->next = nodeToBeRemoved->next;
            nodeToBeRemoved->next->prev = nodeToBeRemoved->prev;
        } else if (!nodeToBeRemoved->prev && nodeToBeRemoved->next) {
            // nodeToBeRemoved is the tail
            m_tail = nodeToBeRemoved->next;
        } else if (nodeToBeRemoved->prev && !nodeToBeRemoved->next) {
            // nodeToBeRemoved is the head
            m_head = nodeToBeRemoved->prev;
            if (m_head) {
                m_head->next = nullptr;
            }
            nodeToBeRemoved->prev = nullptr;
        } else {
            // nodeToBeRemoved is the only element
            m_head = m_tail = nullptr;
        }
    }

    void addNodeToDLL(const std::shared_ptr<Node> nodeToBeAdded) {
        if (nodeToBeAdded->prev && nodeToBeAdded->next) {
            // nodeToBeAdded is in the middle
            nodeToBeAdded->prev->next = nodeToBeAdded->next;
            nodeToBeAdded->next->prev = nodeToBeAdded->prev;
            nodeToBeAdded->next = m_tail;
            nodeToBeAdded->prev = nullptr;
            m_tail->prev = nodeToBeAdded;
            m_tail = nodeToBeAdded;
        } else if (!nodeToBeAdded->prev && nodeToBeAdded->next) {
            // nodeToBeAdded is the tail
            m_tail = nodeToBeAdded;
        } else if (nodeToBeAdded->prev && !nodeToBeAdded->next) {
            // nodeToBeAdded is the head
            m_head = nodeToBeAdded->prev;
            if (m_head) {
                m_head->next = nullptr;
            }
            nodeToBeAdded->next = m_tail;
            nodeToBeAdded->prev = nullptr;
            m_tail->prev = nodeToBeAdded;
            m_tail = nodeToBeAdded;
        } else {
            // nodeToBeAdded is the only element
            m_head = m_tail = nodeToBeAdded;
        }
    }

    /**
     * Loop through the minHeap and only stop if an node is not expired
     * 
     * If the node is dead but not expired, we already removed it from Map and DLL and 
     *    those nodes will be eventually cleaned up. So, don't do anything.
     * If the node is not dead and not expired, don't do anything.
     * If the node is dead and expired, it has been already removed from map and DLL. Only needs to be removed from heap
     * If the node is not dead but expired, it has to be removed from map, DLL and heap
     * 
     * Make it Time bounded cleanup ~= O(K LogN) ~= O(LogN)
     */
    void cleanup() {
        int64_t currentTime = getCurrentTimeInMillis();
        int maxNodesToCleanedUp = 0;
        while (!m_minHeap.empty() && maxNodesToCleanedUp < 2) {
            std::shared_ptr<Node> node = m_minHeap.top();
            // If the node is dead but not expired, we already removed it from Map and DLL and those nodes will be eventually cleaned up.
            // If the node is not dead and not expired, don't do anything.
            // If the node is dead and expired, it has been already removed from map and DLL. Only needs to be removed from heap
            // If the node is not dead but expired, it has to be removed from map, DLL and heap
            if (node->m_isDead && node->m_expirationtime < currentTime) {
                // Pop from heap - O(LogN)
                m_minHeap.pop();
            } else if (!node->m_isDead && node->m_expirationtime < currentTime) {
                // Remove node from map -- O(1)
                m_cache.erase(node->m_key);
                // Remove node from DLL -- O(1)
                removeNodeFromDLL(node);
                // Decrement capacity
                --m_currentCapacity;
                // Pop from heap - O(LogN)
                m_minHeap.pop();
            } else if (node->m_expirationtime > currentTime) {
                break;
            }
            maxNodesToCleanedUp++;
        }
    }

public:
    TTLBasedCache(int totalCapacity) : m_totalCapacity(totalCapacity) {}

    int getValueFromCache(int key) override {
        std::lock_guard<std::mutex> lock(lock_cache);  // Add thread safety
        if (!m_cache.count(key)) {
            return -1;
        }

        std::shared_ptr<Node> node = m_cache[key];
        if (node->m_expirationtime < getCurrentTimeInMillis()) {
            // Mark node as dead
            node->m_isDead = true;
            // Remove node from map -- O(1)
            m_cache.erase(node->m_key);
            // Remove node from DLL -- O(1)
            removeNodeFromDLL(node);
            // Decrement capacity
            --m_currentCapacity;
            return -1;
        }

        addNodeToDLL(node); // O(1)
        return node->m_value;
    }

    void putValueIntoCache(int key, int value, int64_t expirationTime) override {
        std::lock_guard<std::mutex> lock(lock_cache);
        std::shared_ptr<Node> node;

        // Key exists
        if (m_cache.count(key)) {
            node = m_cache[key];
            // Update the values
            node->m_key = key;
            node->m_value = value;
            node->m_expirationtime = expirationTime;
            // update DLL
            addNodeToDLL(node); // O(1)
            // Insert into minHeap
            m_minHeap.push(node); // O(LogN)
        } else {
            // If key does not exist
            ++m_currentCapacity;
            node = std::make_shared<Node>(key, value, expirationTime);
            // Insert into Map
            m_cache[key] = node;
            // update DLL
            addNodeToDLL(node); // O(1)
            // Insert into minHeap
            m_minHeap.push(node); // O(LogN)
        }

        if (m_currentCapacity > m_totalCapacity) {
            // Mark node as dead
            m_head->m_isDead = true;
            // Remove node from map -- O(1)
            m_cache.erase(m_head->m_key);
            // Remove node from DLL -- O(1)
            removeNodeFromDLL(m_head);
            // Decrement capacity
            --m_currentCapacity;
        }

        //lazy cleanup - should be async
        cleanup();
    }

    void deleteKeyFromCache(int key) {
        std::lock_guard<std::mutex> lock(lock_cache);
        if (!m_cache.count(key)) {
            return;
        }
        std::shared_ptr<Node> node = m_cache[key];
        // Mark node as dead
        node->m_isDead = true;
        // Remove node from map -- O(1)
        m_cache.erase(node->m_key);
        // Remove node from DLL -- O(1)
        removeNodeFromDLL(node);
        // Decrement capacity
        --m_currentCapacity;
    }

    void runTest() {
        cout << "=== TTL Cache Test Suite (Capacity: " << m_totalCapacity << ") ===" << endl;
        int testsPassed = 0;
        int totalTests = 0;

        int64_t now = getCurrentTimeInMillis();

        // Test 1: Basic PUT + GET (no expiration)
        cout << "\n=== Test 1: Basic PUT + GET ===" << endl;
        {
            putValueIntoCache(1, 100, now + 10000);   // +10s
            int v1 = getValueFromCache(1);
            totalTests++;
            if (v1 == 100) { cout << "PASS: GET(1)==100\n"; testsPassed++; }
            else           { cout << "FAIL: GET(1) expected 100, got " << v1 << "\n"; }

            totalTests++;
            if (m_currentCapacity == 1) { cout << "PASS: capacity==1\n"; testsPassed++; }
            else                        { cout << "FAIL: capacity expected 1, got " << m_currentCapacity << "\n"; }
        }

        // Test 2: Expiration on GET
        cout << "\n=== Test 2: TTL Expiration on GET ===" << endl;
        {
            int64_t t = getCurrentTimeInMillis();
            putValueIntoCache(2, 200, t + 1000);  // +1s
            int vBefore = getValueFromCache(2);
            cout << "   GET(2) before sleep: " << vBefore << "\n";
            this_thread::sleep_for(chrono::seconds(2));
            int vAfter = getValueFromCache(2);
            totalTests++;
            if (vAfter == -1) { cout << "✓ PASS: GET(2) after TTL returns -1\n"; testsPassed++; }
            else              { cout << "✗ FAIL: GET(2) after TTL expected -1, got " << vAfter << "\n"; }

            totalTests++;
            if (m_currentCapacity == 1) { cout << "✓ PASS: capacity decremented after expiration\n"; testsPassed++; }
            else                        { cout << "✗ FAIL: capacity expected 1, got " << m_currentCapacity << "\n"; }
        }

        // Test 3: Update existing key (value + TTL)
        cout << "\n=== Test 3: Update Existing Key ===" << endl;
        {
            int64_t t = getCurrentTimeInMillis();
            putValueIntoCache(1, 150, t + 5000);   // update key 1
            int v = getValueFromCache(1);
            totalTests++;
            if (v == 150) { cout << "✓ PASS: updated GET(1)==150\n"; testsPassed++; }
            else          { cout << "✗ FAIL: updated GET(1) expected 150, got " << v << "\n"; }

            totalTests++;
            if (m_currentCapacity == 1) { cout << "✓ PASS: capacity unchanged on update\n"; testsPassed++; }
            else                        { cout << "✗ FAIL: capacity expected 1, got " << m_currentCapacity << "\n"; }
        }

        // Test 4: LRU eviction at capacity=3
        cout << "\n=== Test 4: LRU Eviction (capacity=3) ===" << endl;
        {
            // Currently only key 1 is present
            int64_t t = getCurrentTimeInMillis();
            putValueIntoCache(2, 200, t + 10000);
            putValueIntoCache(3, 300, t + 10000);
            // Cache: {1,2,3} (some LRU order), capacity==3
            totalTests++;
            if (m_currentCapacity == 3) { cout << "✓ PASS: capacity==3 after adding 1,2,3\n"; testsPassed++; }
            else                        { cout << "✗ FAIL: capacity expected 3, got " << m_currentCapacity << "\n"; }

            // Make key 2 most recently used
            (void)getValueFromCache(2);
            // Add key 4 → should evict LRU (either 1 or 3, depending on your DLL logic)
            putValueIntoCache(4, 400, t + 1);

            int v2 = getValueFromCache(2);
            totalTests++;
            if (v2 == 200) {
                cout << "✓ PASS: recently-used key (2) is present\n";
                testsPassed++;
            } else {
                cout << "✗ FAIL: expected GET(2)==200 & GET(4)==400, got 2:" << v2 << "\n";
            }

            totalTests++;
            if (m_currentCapacity == 3) { cout << "✓ PASS: capacity remains 3 after eviction\n"; testsPassed++; }
            else                        { cout << "✗ FAIL: capacity expected 3, got " << m_currentCapacity << "\n"; }
        }

        // Test 5: deleteKeyFromCache
        cout << "\n=== Test 5: Explicit Delete ===" << endl;
        {
            int before = m_currentCapacity;
            deleteKeyFromCache(2);
            int v = getValueFromCache(2);
            totalTests++;
            if (v == -1) { cout << "✓ PASS: deleted key returns -1\n"; testsPassed++; }
            else         { cout << "✗ FAIL: deleted key expected -1, got " << v << "\n"; }

            totalTests++;
            if (m_currentCapacity == before - 1) {
                cout << "✓ PASS: capacity decremented after delete\n";
                testsPassed++;
            } else {
                cout << "✗ FAIL: capacity expected " << (before - 1) << ", got " << m_currentCapacity << "\n";
            }
        }

        cout << "\n=== Summary: " << testsPassed << "/" << totalTests << " tests passed ===" << endl;
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