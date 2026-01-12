#include <iostream>
#include <unordered_map>

// Threading
#include <thread>
#include <mutex>
#include <condition_variable>

// File 
#include <fstream>
#include <sstream>
#include <cstdio> 

using namespace std;


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
 */
class PersistentCache {
private:
    std::string m_id;
    std::unordered_map<int, int> m_cache;
    std::thread m_snapshotThread;
    std::mutex m_sharedStateMtx;
    std::condition_variable m_doorbell;

    void writeToLogFile(const std::string operation, int key, int value) {
        std::ofstream file(getId() + "_cache.log", std::ios::app); // append
        if (file.is_open()) {
            file << operation << " " << key << " " << value << std::endl;
        }
        file.close();
    }

    void writeToSnapFile() {
        std::ofstream file(getId() + "_tmpCache.snap", std::ios::trunc);
        if (file.is_open()) {
            for (auto& keyValue : m_cache) {
                file << keyValue.first << " " << keyValue.second << std::endl;
            }
        }
        file.close();
    }

    void readFromLogFile(std::ifstream& file) {
        std::string line;
        while(std::getline(file, line)) {
            std::stringstream ss (line);
            string operation, key, value;
            ss >> operation >> key >> value;

            if (operation == "PUT") {
                m_cache[std::stoi(key)] = std::stoi(value);
            } else if (operation == "DELETE") {
                m_cache.erase(std::stoi(key));
            }
        }
    }

    void readFromSnapFile(std::ifstream& file) {
        std::string line;
        while(std::getline(file, line)) {
            std::stringstream ss(line);
            string key, value;
            ss >> key >> value;
            m_cache[std::stoi(key)] = std::stoi(value);
        }
    }

    void perodicSnapshot() {
        while (true) {
            std::unique_lock<std::mutex> lock(m_sharedStateMtx);

            // The thread will sleep for every 10 seconds
            m_doorbell.wait_for(lock, std::chrono::seconds(10));

            // STEP 1: Once the thread wakes up, construct the cache in a temporary file
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
        m_snapshotThread.detach(); // bad design
        // USe atomic bool to exit from the loop and cleanup the threads
    }

    /**
     * If Snap file is present and log file, that means snap thread ran and after which we collect more logs. Hence read snap first and then log.
     * If only snap or log is present, read which ever is present.
     * 
     * If none of them is present, either data loss or no call has been made.
     */
    void loadCacheFromDisk() {
        std::lock_guard<std::mutex> lock(m_sharedStateMtx);

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

    int getValueFromCache(int key){
        std::lock_guard<std::mutex> lock(m_sharedStateMtx);
        
        if (!m_cache.count(key)) {
            return -1;
        }
        return m_cache[key];
    }

    void putKeyIntoCacheForCriticalData(int key, int value) {
        std::lock_guard<std::mutex> lock(m_sharedStateMtx);

        // STEP 1: Write to Log disk
        writeToLogFile("PUT", key, value);

        // STEP 2: Cache to RAM
        m_cache[key] = value;
    }

    void deleteKeyFromCacheFromCriticalData(int key) {
        std::lock_guard<std::mutex> lock(m_sharedStateMtx);

        // STEP 1: Write to Log file
        writeToLogFile("DELETE", key, m_cache[key]);

        // STEP 2: Delete from Cache
        m_cache.erase(key);
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
    static const int RING_SIZE = 20;
    
    struct Cluster {
        std::shared_ptr<PersistentCache> primary;
        std::shared_ptr<PersistentCache> replica1;
        std::shared_ptr<PersistentCache> replica2;
    };
    
    // State 1: Hash ring contains primary shard IDs
    std::unordered_map<uint32_t, std::shared_ptr<PersistentCache>> m_hashRing;
    // State 2: Primary shard ID maps to cluster reference  
    std::unordered_map<std::string, std::shared_ptr<Cluster>> m_primaryToCluster;
    
    /** 
     * For every key, it will provide result between 0 and RING_SIZE-1
     */
    uint32_t hashFunction(int key) {
        std::hash<std::string> hasher;
        return hasher(std::to_string(key)) % RING_SIZE;
    }
    
public:
    DistributedCacheManager() {
        for (uint32_t index = 0; index < RING_SIZE; index++) {
            // STEP 1: Create primary shard and assign them in positions starting from 0 to RING_SIZE-1 in the hash ring
            std::string primaryShardId = "PrimaryShard-" + std::to_string(index);
            auto primaryShard = std::make_shared<PersistentCache>(primaryShardId);
            m_hashRing[index] = primaryShard;
            
            // Step 2: Create cluster with 3 PersistentCache instances
            auto cluster = std::make_unique<Cluster>();
            cluster->primary = primaryShard;
            cluster->replica1 = std::make_shared<PersistentCache>("ReplicaShard-1-For-" + primaryShardId);
            cluster->replica2 = std::make_shared<PersistentCache>("ReplicaShard-2-For-" + primaryShardId);
            
            // Step 3: Map primary shard ID to cluster
            m_primaryToCluster[primaryShardId] = std::move(cluster);
        }
    }
    
    void put(int key, int value) {
        // STEP 1: Get the primary shard ID from hash ring
        uint32_t position = hashFunction(key); // This will fall in between 0 and RING_SIZE-1
        std::shared_ptr<PersistentCache> primaryShard = m_hashRing[position];
        
        // STEP 2: Call PUT for every instance in the cluster
        if (m_primaryToCluster.count(primaryShard->getId())) {
            std::shared_ptr<Cluster> cluster = m_primaryToCluster[primaryShard->getId()];
            cluster->primary->putKeyIntoCacheForCriticalData(key, value);
            cluster->replica1->putKeyIntoCacheForCriticalData(key, value);
            cluster->replica2->putKeyIntoCacheForCriticalData(key, value);
        }
    }
    
    int get(int key) {
        // STEP 1: Get the primary shard ID from hash ring
        uint32_t position = hashFunction(key); // This will fall in between 0 and RING_SIZE-1
        std::shared_ptr<PersistentCache> primaryShard = m_hashRing[position];
        
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
        std::vector<int> testKeys = {100, 200, 300, 400, 500};
        
        for (int key : testKeys) {
            std::hash<std::string> hasher;
            uint32_t position = hasher(std::to_string(key)) % 20;
            std::cout << "Key " << key << " → Shard position " << position << std::endl;
        }
        
        std::cout << "\nTesting distributed PUT operations:" << std::endl;
        distributedCache.put(100, 1000);
        std::cout << "PUT(100, 1000) - written to primary + 2 replicas" << std::endl;
        
        distributedCache.put(200, 2000);
        std::cout << "PUT(200, 2000) - written to primary + 2 replicas" << std::endl;
        
        distributedCache.put(300, 3000);
        std::cout << "PUT(300, 3000) - written to primary + 2 replicas" << std::endl;
        
        std::cout << "\nTesting distributed GET operations with quorum:" << std::endl;
        int result1 = distributedCache.get(100);
        std::cout << "GET(100): " << result1 << " (quorum-based read)" << std::endl;
        
        int result2 = distributedCache.get(200);
        std::cout << "GET(200): " << result2 << " (quorum-based read)" << std::endl;
        
        int result3 = distributedCache.get(999);
        std::cout << "GET(999): " << result3 << " (should be -1, key doesn't exist)" << std::endl;
        
        std::cout << "\nDistributed cache operations completed successfully!" << std::endl;
    }
    
    // Test 2: Persistence and Recovery Simulation
    /**printTestHeader("Persistence and Recovery Test");
    {        
        // Create a single persistent cache instance
        PersistentCache singleCache("TestCache_");
        std::cout << "Created PersistentCache with ID: TestCache" << std::endl;
        
        // Test PUT operations (will be written to disk)
        singleCache.putKeyIntoCacheForCriticalData(1, 100);
        std::cout << "PUT(1, 100) - written to TestCachecache.log" << std::endl;
        
        singleCache.putKeyIntoCacheForCriticalData(2, 200);
        std::cout << "PUT(2, 200) - written to TestCachecache.log" << std::endl;
        
        // Test GET operations
        std::cout << "GET(1): " << singleCache.getValueFromCache(1) << std::endl;
        std::cout << "GET(2): " << singleCache.getValueFromCache(2) << std::endl;
        std::cout << "GET(999): " << singleCache.getValueFromCache(999) << " (should be -1)" << std::endl;
        
        std::cout << "\nCache operations logged to disk successfully!" << std::endl;
        std::cout << "Note: Snapshot thread will create snapshot every 10 seconds" << std::endl;
        
        // Simulate some time for snapshot thread to work
        std::cout << "Waiting 12 seconds to demonstrate background snapshot thread..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(12));
    }*/
    
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "ALL TESTS COMPLETED" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    return 0;
}
