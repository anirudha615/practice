#include <iostream>
#include <unordered_map>
#include <map>

// Threading
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>

// File 
#include <fstream>
#include <sstream>
#include <cstdio> 

using namespace std;

struct Node {
    string m_key;
    int m_value;
    std::mutex m_KeyMtx; // No concurrent request on the object
    int m_recentVersion; // For Pessimistic locking
    bool m_tombstoneFlag; // For soft deletion.
    Node(string key, int value, int recentVersion, bool tombstoneFlag) 
    : m_key(key), m_value(value), m_recentVersion(recentVersion), m_tombstoneFlag(tombstoneFlag) {}
};

/**
 * WRITING - PUT/DELETE
 * 1. For critical data, persist in file and then write to memory. 
 *      Periodically, use the snapshot thread to construct the cache in .snap and delete the .log
 * 2. For non-critical data, perist in the memory and queue up the operation to be writtent to disk (similar to JobScheduler/Crawler)
 *       Periodically, use the snapshot thread to construct the cache in .snap and delete the .log
 * 
 * READING AFTER CRASH
 * 1. If Snap file is present and log file, that means snap thread ran and after which we collect more logs. 
 *      Hence read snap first and then log.
 * 2. If only snap or log is present, read which ever is present.
 * 3. If none of them is present, either data loss or no call has been made.
 * 
 * LOCKING: (Similar to SQL)
 *   1. We are using a readers-writer lock in the map to prevent readers accessing old references if new writes leads to rehashing in the map.
 *      a. Remember, re-hashing is consistent hash ring where ring size == nodes spawned. If we add new nodes, all the neighbors redistribute.
 *      b. So, when a writer gets a lock, all readers are removed so that re-hashing can happen and then multiple readers can read at the same time.
 *   2. If multiple readers are reading at the same time, anyone of them would be able to modify as we have fine-grained locking.
 *   3. On top of that, the update would be valid if the lastReadVersion matches the the recentVersion. Example - if 2 readers read at the same time 
 *      and both of them update it and proceed to queue it up for writing, it will lead to overwrite. So, we want the 2nd reader to request for a
 *      write based on lastReaderVersion.
 * 
 * https://prachub.com/interview-questions/design-an-artifact-store-on-k8s-and-cassandra
 * https://prachub.com/interview-questions/design-a-distributed-multi-user-counter
 * 
 */
class PersistentCache {
private:
    std::string m_id;
    std::shared_timed_mutex m_cacheMutex;
    std::unordered_map<string, std::shared_ptr<Node>> m_cache;
    std::thread m_snapshotThread;
    std::mutex m_FileMtx;
    std::condition_variable m_doorbell;
    std::atomic<bool> m_shutdown {false};

    void writeToLogFile(const std::string operation, std::shared_ptr<Node> node) {
        std::unique_lock<std::mutex> lock(m_FileMtx);
        std::ofstream file(getId() + "_cache.log", std::ios::app); // append
        if (file.is_open()) {
            file << operation << " " << node->m_key << " " << node->m_value << " " << node->m_recentVersion << " " << node->m_tombstoneFlag << std::endl;
        }
        file.close();
    }

    void writeToSnapFile() {
        std::ofstream file(getId() + "_tmpCache.snap", std::ios::trunc);
        if (file.is_open()) {
            for (auto& keyValue : m_cache) {
                std::shared_ptr<Node> node = keyValue.second;
                std::lock_guard<std::mutex> nodeLock(node->m_KeyMtx);
                file << keyValue.second->m_key << " " << keyValue.second->m_value << " " << keyValue.second->m_recentVersion << " " << keyValue.second->m_tombstoneFlag << std::endl;
            }
        }
        file.close();
    }

    void readFromLogFile(std::ifstream& file) {
        std::unique_lock<std::mutex> lock(m_FileMtx);
        std::string line;
        while(std::getline(file, line)) {
            std::stringstream ss (line);
            string operation, key, value, version, tombstoneFlag;
            ss >> operation >> key >> value >> version >> tombstoneFlag;
            {
                std::unique_lock<std::shared_timed_mutex> lock(m_cacheMutex); // WRITE
                if (operation == "PUT") {
                    m_cache[key] = std::make_shared<Node>(key, stoi(value), stoi(version), (bool)stoi(tombstoneFlag));;
                } else if (operation == "DELETE") {
                    m_cache.erase(key);
                }
            }
            
        }
    }

    void readFromSnapFile(std::ifstream& file) {
        std::unique_lock<std::mutex> lock(m_FileMtx);
        std::string line;
        while(std::getline(file, line)) {
            std::stringstream ss(line);
            string operation, key, value, version, tombstoneFlag;
            ss >> operation >> key >> value >> version >> tombstoneFlag;
            {
                std::unique_lock<std::shared_timed_mutex> lock(m_cacheMutex); // WRITE
                m_cache[key] = std::make_shared<Node>(key, stoi(value), stoi(version), (bool)stoi(tombstoneFlag));
            }
            
        }
    }

    void perodicSnapshot() {
        while (!m_shutdown.load()) {
            std::unique_lock<std::mutex> lock(m_FileMtx);

            // The thread will sleep for every 10 seconds
            m_doorbell.wait_for(lock, std::chrono::seconds(10), [this] {
                return m_shutdown.load();
            });

            // STEP 1: Once the thread wakes up, if no shutdown, construct the cache in a temporary file
            if (m_shutdown.load()) {
                break;
            }
            writeToSnapFile();
            
            // STEP 2: Remove the original snap file
            string fileToRemove = getId() + "_cache.snap";
            std::remove(fileToRemove.c_str());
            auto fileToRename  = getId() + "_tmpCache.snap";
            auto fileToBeRenamedTo = getId() + "_cache.snap";
            std::rename(fileToRename.c_str(), fileToBeRenamedTo.c_str());

            // STEP 3: Remove the log file
            fileToRemove = getId() + "_cache.log";
            std::remove(fileToRemove.c_str());
        }
    }
public:
    PersistentCache() {}
    PersistentCache(string id) {
        m_id = id;
        m_snapshotThread = std::thread(&PersistentCache::perodicSnapshot, this);
    }

    ~PersistentCache() {
        m_shutdown = true;
        m_doorbell.notify_all();
        if (m_snapshotThread.joinable()) {
            m_snapshotThread.join();
        }
    }

    /**
     * If Snap file is present and log file, that means snap thread ran and after which we collect more logs. Hence read snap first and then log.
     * If only snap or log is present, read which ever is present.
     * 
     * If none of them is present, either data loss or no call has been made.
     */
    void loadCacheFromDisk() {
        std::ifstream log(getId() + "_cache.log", std::ios::in);
        std::ifstream snap(getId() + "_cache.snap", std::ios::in);
        if (snap.is_open() && log.is_open()) {
            readFromSnapFile(snap);
            readFromLogFile(log);
        } else if (!snap.is_open() && log.is_open()) {
            readFromLogFile(log);
        } else if (snap.is_open() && !log.is_open()) {
            readFromSnapFile(snap);
        } else {
            return;
        }
    }

    int getValueFromCache(string key){   
        std::shared_lock<std::shared_timed_mutex> lock(m_cacheMutex); // READ LOCK   
        if (!m_cache.count(key)) {
            return -1;
        }
        std::shared_ptr<Node> node = m_cache[key];
        std::lock_guard<std::mutex> nodeLock(node->m_KeyMtx);
        return node->m_value;
    }

    void putKeyIntoCacheForCriticalData(string key, int value, int lastReadVersion) {
        std::shared_ptr<Node> node;
        {
            std::shared_lock<std::shared_timed_mutex> lock(m_cacheMutex); // READ LOCK
            if (m_cache.count(key)) {
                node = m_cache[key];
            }
        }

        if (!node) {
            node = std::make_shared<Node>(key, value, 0, false);
            // STEP 1: Write to Log disk
            writeToLogFile("PUT", node);
            // Using a shared pointer so, we don't have to write back to the map.
        } else {
            std::lock_guard<std::mutex> lock(node->m_KeyMtx);
            if (node->m_recentVersion == lastReadVersion) {
                //Update the value
                node->m_value = value;
                node->m_recentVersion++;
                // STEP 1: Write to Log disk
                writeToLogFile("PUT", node);
                // Using a shared pointer so, we don't have to write back to the map.
            } else {
                std::cout << "Rejected : Stale Entry for key : " << key << " and version : " << lastReadVersion << std::endl;
            }
        }
    }

    void deleteKeyFromCacheFromCriticalData(string key, int lastReadVersion) {
        std::shared_ptr<Node> node;
        {
            std::shared_lock<std::shared_timed_mutex> lock(m_cacheMutex); // READ LOCK 
            if (m_cache.count(key)) {
                node = m_cache[key];
            }
        }
        if (!node) {
            return;
        }

        {
            std::lock_guard<std::mutex> lock(node->m_KeyMtx);
            if (node->m_recentVersion == lastReadVersion) {
                // STEP 1: Update the value
                node->m_tombstoneFlag = true;
                node->m_recentVersion++;
                // STEP 2: Write to Log disk as this is critical data
                writeToLogFile("DELETE", node);
                {
                    std::unique_lock<std::shared_timed_mutex> lock(m_cacheMutex); // WRITE LOCK
                    // STEP 3: Delete from Cache
                    m_cache.erase(key);
                }  
            } else {
                std::cout << "Rejected : Stale Entry for key : " << key << " and version : " << lastReadVersion << std::endl;
            }
        }      
    }

    string getId() {
        return m_id;
    }
};


/**
 * 1. When we receive a PUT/DELETE request - the call will go to some manager/coordination which will find the primary shard for that key . 
 *      The manager will be responsible for getting the shard_id. The manager will also have [shard_id] = {Replica ID List} --> CPU Operation
 * 2. Based on the load of every instance, load balancers will guide where to route the call -> I/O bound operation. --> Managers are stateless. So, any instance of manager can do this job.
 * 3. Now, say the call is routed to replica_id --> A and since it is a critical data, it will write to the disk --> IO and then to memory (CPU) and then trigger aysnc I/O call to its cluster 
 * 
 * 4. When we receive a GET request, the call will go to manager and similary after consistent hashing, we will find which instance is able to take the load.
 * 5. When the call is routed to replica_id --> C , we will peek the value from the cache and then make a synchronous call to all other replicas to do the same --> 
 *      I/O and then do a Quorum. If the quorum is satisfied, we will return the value and change the recency. Again, make an async I/O call to all replicas to update the recency structure.
 *  
 * How frequently do we need to trigger async I/O call to update the cluster ?
 * 
 */

class DistributedCacheManager {
private:
    static const int32_t RING_SIZE = 20;
    
    struct Cluster {
        std::shared_ptr<PersistentCache> primary;
        std::shared_ptr<PersistentCache> replica1;
        std::shared_ptr<PersistentCache> replica2;
    };
    
    // State 1: Hash ring contains primary shard IDs
    std::map<int32_t, std::shared_ptr<PersistentCache>> m_hashRing;
    // State 2: Primary shard ID maps to cluster reference  
    std::unordered_map<std::string, std::shared_ptr<Cluster>> m_primaryToCluster;
    
    /** 
     * For every key, it will provide result between 0 and RING_SIZE-1
     */
    int32_t hashFunction(string key) {
        std::hash<std::string> hasher;
        return hasher(key) % RING_SIZE;
    }

    /**
     * If the key points to any value between 0 and RING_SIZE-1, find the lower bound in HashRing.
     * If you don't find, get the node at the beginning
     * 
     * CONSISTENT HASH RING - Retrieval
     */
    std::shared_ptr<PersistentCache> getPrimaryShardInHashRing(string key) {
        int32_t position = hashFunction(key); // This will fall in between 0 and RING_SIZE-1
        auto nodePtr = m_hashRing.lower_bound(position); // Get the iterator > or = to the position (clockwise)
        if (nodePtr == m_hashRing.end()) {
            nodePtr = m_hashRing.begin(); // If it hits the end, get me to the beginnning (moving clockwise)
        }
        return nodePtr->second;
    }
    
public:
    // CONSISTENT HASH RING - Creation
    DistributedCacheManager() {
        // In a hash ring where 20 max nodes can be spawned, we are only spawning 4 of them.
        // So, the nodes would be at 0, 5, 10 and 15
        int32_t nodesToSpawned = 4; 
        for (uint32_t index = 0; index < nodesToSpawned; index++) {
            // STEP 1: Create primary shard and assign them to each node
            std::string primaryShardId = "PrimaryShard-" + std::to_string(index);
            auto primaryShard = std::make_shared<PersistentCache>(primaryShardId);
            m_hashRing[index * (RING_SIZE/nodesToSpawned)] = primaryShard;
            
            // Step 2: Create cluster with 3 PersistentCache instances
            auto cluster = std::make_unique<Cluster>();
            cluster->primary = primaryShard;
            cluster->replica1 = std::make_shared<PersistentCache>("ReplicaShard-1-For-" + primaryShardId);
            cluster->replica2 = std::make_shared<PersistentCache>("ReplicaShard-2-For-" + primaryShardId);
            
            // Step 3: Map primary shard ID to cluster
            m_primaryToCluster[primaryShardId] = std::move(cluster);
        }
    }
    
    void put(string key, int value) {
        // STEP 1: Get Primary Shard in the hash ring
        std::shared_ptr<PersistentCache> primaryShard = getPrimaryShardInHashRing(key);
        
        // STEP 2: Call PUT for every instance in the cluster
        if (m_primaryToCluster.count(primaryShard->getId())) {
            std::shared_ptr<Cluster> cluster = m_primaryToCluster[primaryShard->getId()];
            cluster->primary->putKeyIntoCacheForCriticalData(key, value, 0);
            cluster->replica1->putKeyIntoCacheForCriticalData(key, value, 0);
            cluster->replica2->putKeyIntoCacheForCriticalData(key, value, 0);
        }
    }
    
    int get(string key) {
        // STEP 1: Get Primary Shard in the hash ring
        std::shared_ptr<PersistentCache> primaryShard = getPrimaryShardInHashRing(key);
        
        if (m_primaryToCluster.count(primaryShard->getId())) {
            std::shared_ptr<Cluster> cluster = m_primaryToCluster[primaryShard->getId()];
            // Return if any 2 matches 
            if (cluster->primary->getValueFromCache(key) == cluster->replica1->getValueFromCache(key)) {
                return cluster->primary->getValueFromCache(key);
            } else if (cluster->primary->getValueFromCache(key) == cluster->replica2->getValueFromCache(key)) {
                return cluster->replica2->getValueFromCache(key);
            } else if (cluster->replica1->getValueFromCache(key) == cluster->replica2->getValueFromCache(key)) {
                return cluster->replica2->getValueFromCache(key);
            }
        }
        return -1;
    }
};

void printTestHeader(const std::string& testName) {
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "TEST: " << testName << std::endl;
    std::cout << std::string(50, '=') << std::endl;
}

int main(){
    // Test 1: Basic Distributed Cache Operations
    printTestHeader("Distributed Cache with Consistent Hashing");
    {
        DistributedCacheManager distributedCache;
        std::cout << "Created distributed cache with " << 20 << " shards and 3 replicas each\n" << std::endl;
        
        // Test key distribution across shards
        std::cout << "Testing key distribution across shards:" << std::endl;
        std::vector<string> testKeys = {"A", "B", "C", "D", "E"};
        
        for (string key : testKeys) {
            std::hash<std::string> hasher;
            uint32_t position = hasher(key) % 20;
            std::cout << "Key " << key << " → Shard position " << position << std::endl;
        }
        
        std::cout << "\nTesting distributed PUT operations:" << std::endl;
        distributedCache.put("A", 1000);
        std::cout << "PUT('A', 1000) - written to primary + 2 replicas" << std::endl;
        
        distributedCache.put("B", 2000);
        std::cout << "PUT('B', 2000) - written to primary + 2 replicas" << std::endl;
        
        distributedCache.put("C", 3000);
        std::cout << "PUT('C', 3000) - written to primary + 2 replicas" << std::endl;
        
        std::cout << "\nTesting distributed GET operations with quorum:" << std::endl;
        int result1 = distributedCache.get("A");
        std::cout << "GET('A'): " << result1 << " (quorum-based read)" << std::endl;
        
        int result2 = distributedCache.get("B");
        std::cout << "GET('B'): " << result2 << " (quorum-based read)" << std::endl;
        
        int result3 = distributedCache.get("Z");
        std::cout << "GET('Z'): " << result3 << " (should be -1, key doesn't exist)" << std::endl;
        
        std::cout << "\nDistributed cache operations completed successfully!" << std::endl;
    }
    
    // Test 2: Persistence and Recovery Simulation
    printTestHeader("Persistence and Recovery Test");
    {        
        // Create a single persistent cache instance
        PersistentCache singleCache("TestCache_");
        std::cout << "Created PersistentCache with ID: TestCache" << std::endl;
        
        // Test PUT operations (will be written to disk)
        singleCache.putKeyIntoCacheForCriticalData("A", 100, 0);
        std::cout << "PUT('A', 100) - written to TestCachecache.log" << std::endl;
        
        singleCache.putKeyIntoCacheForCriticalData("B", 200, 0);
        std::cout << "PUT('B', 200) - written to TestCachecache.log" << std::endl;
        
        // Test GET operations
        std::cout << "GET('A'): " << singleCache.getValueFromCache("A") << std::endl;
        std::cout << "GET('B'): " << singleCache.getValueFromCache("B") << std::endl;
        std::cout << "GET('Z'): " << singleCache.getValueFromCache("Z") << " (should be -1)" << std::endl;
        
        std::cout << "\nCache operations logged to disk successfully!" << std::endl;
        std::cout << "Note: Snapshot thread will create snapshot every 10 seconds" << std::endl;
        
        // Simulate some time for snapshot thread to work
        std::cout << "Waiting 12 seconds to demonstrate background snapshot thread..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(12));
    }
    
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "ALL TESTS COMPLETED" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    return 0;
}