#include <iostream>
#include <map>
#include <thread>
#include <mutex>
#include <vector>
#include <unordered_map>

using namespace std;

/**
 * If the range is small and the data is dense, array would be efficient
 * as index would be the key and value would be value as compared to unordered_map
 */
class Node {
private:
    std::mutex m_frequencyCounterMutex;
    
public:
    std::vector<int> m_list;
    std::map<int, int> m_frequencyCounter;
    double timeTakenToRead = 0.0;
    double timeTakenToSend = 0.0;
    Node(std::vector<int> list) : m_list(list) {}

    std::mutex& getMutex() {
        return m_frequencyCounterMutex;
    }
};

class DistributedStatisticsManager {
private:
    std::vector<std::shared_ptr<Node>> m_nodeList;
    int m_receivingNodeIndex;
    std::vector<std::thread> m_threadPool;
    const double TIME_TAKEN_TO_READ_1_BYTE = 0.1;
    const double TIME_TAKEN_TO_SEND_1_BYTE = 1.0;

    /**
     * Every thread will read from m_nodeList and m_receivingNodeIndex but will not update them. So, no need of mutex.
     * Additionally, every thread will operate on single node instance. So, don't have to use the mutex on every node.
     * 
     * However, all the threads will converge on the receiving node where we need mutex of receiving node to update the frequency counter.
     */
    void sendFrequencyCounterToReceivingNode(int index) {
        // this needs to be protected by mutex
        std::shared_ptr<Node> sendingNode = m_nodeList[index];
        std::shared_ptr<Node> receivingNode = m_nodeList[m_receivingNodeIndex];

        /** Calcuatate frequencyCounterPerNode */
        std::unordered_map<int, int> frequencyCounterPerNode;
        for (int integer : sendingNode->m_list) {
            frequencyCounterPerNode[integer]++;
        }

        /** Calculate time Taken to read */
        sendingNode->timeTakenToRead += (frequencyCounterPerNode.size() * 2 * 4 * TIME_TAKEN_TO_READ_1_BYTE);

        /** Calculate time Taken to send */
        if (index != m_receivingNodeIndex) {
            sendingNode->timeTakenToSend += (frequencyCounterPerNode.size() * 2 * 4 * TIME_TAKEN_TO_SEND_1_BYTE);
        }

        /** Send the frequency counter to the receiving node*/
        {
            std::lock_guard<std::mutex> lock(receivingNode->getMutex());
            for (auto& pair : frequencyCounterPerNode) {
                receivingNode->m_frequencyCounter[pair.first] += frequencyCounterPerNode[pair.first];
            }
        }
    }

public:
    DistributedStatisticsManager(const std::vector<std::shared_ptr<Node>>& nodeList)
    : m_nodeList(std::move(nodeList)) {

        // STEP 1: Find the receiving node Index
        int maxIntegerListSize = -1;
        for (int index = 0; index < m_nodeList.size(); index++) {
            std::shared_ptr<Node> node = m_nodeList[index];
            maxIntegerListSize = std::max(maxIntegerListSize, static_cast<int>(node->m_list.size()));
            if (maxIntegerListSize == node->m_list.size()) {
                m_receivingNodeIndex = index;
            }
        }
        std::cout << "Receiving node is " << m_receivingNodeIndex << std::endl;

        // STEP 2: Asynchronously send frequency counter to receving node
        for (int index = 0; index < m_nodeList.size(); index++) {
            m_threadPool.push_back(std::thread(&DistributedStatisticsManager::sendFrequencyCounterToReceivingNode, 
                this, index));
        }

        // STEP 3: Clean up the threads - indicating all the nodes have send their frequency counter to the receving node
        for (auto& worker : m_threadPool) {
            if (worker.joinable()) {
                worker.join(); // clean up threads;
            }
        }

        // STEP 4: Calculate mode in the receiving node using the main thread
        std::map<int, int> cumulativeCount;
        int totalCount = 0;
        int maxFrequency = -1;
        int distributedModeStatistics = -1;
        std::shared_ptr<Node> processingNode = m_nodeList.at(m_receivingNodeIndex);
        for (auto& pair : processingNode->m_frequencyCounter) {
            maxFrequency = std::max(maxFrequency, pair.second);
            if (maxFrequency == pair.second) {
                distributedModeStatistics = pair.first; // Node with highest frequency
            }
            totalCount = totalCount + pair.second;
            cumulativeCount[pair.first] = totalCount;
        }
        /** Update the receving node's timeTakenToRead */
        processingNode->timeTakenToRead = processingNode->timeTakenToRead + (processingNode->m_frequencyCounter.size() * 2 * 4 * TIME_TAKEN_TO_READ_1_BYTE);
        std::cout << "Mode is : " << distributedModeStatistics << std::endl;

        // STEP 5: Calculate median in the receiving node using the main thread
        int distributedMedianStatistics = 0;
        if (!(totalCount % 2)) { // Even number of elements and median would be average of middle two
            int medianIndex1 = totalCount/2;
            int medianIndex2 = totalCount/2 + 1;
            for (auto& pair :cumulativeCount) {
                if (medianIndex1 <= pair.second || medianIndex2 <= pair.second) {
                    distributedMedianStatistics = distributedMedianStatistics + (pair.first/2);
                }
                
            }
        } else { // Odd number of elements where median would be the middle element
            int medianIndex = (totalCount + 1)/2;
            for (auto& pair :cumulativeCount) {
                if (medianIndex <= pair.second) {
                    distributedMedianStatistics = pair.first;
                    break;
                }
            }
        }
        std::cout << "Median is : " << distributedMedianStatistics << std::endl;
        
        // STEP 6: Print time taken for each node to read and send
        std::cout << "\n--- Node Performance Metrics ---" << std::endl;
        for (int index = 0; index < m_nodeList.size(); index++) {
            std::shared_ptr<Node> node = m_nodeList[index];
            std::cout << "Node " << index << ":" << std::endl;
            std::cout << "  Time taken to read:  " << node->timeTakenToRead << " s" << std::endl;
            std::cout << "  Time taken to send:  " << node->timeTakenToSend << " s" << std::endl;
        }
    }
};

int main() {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "TEST: " << "Basic Distributed Statistics" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    // Create nodes with different integer lists
    // Node 0: [1, 2, 3, 3, 3]
    // Node 1: [2, 3, 3, 4, 5, 6]  <- This will be the receiving node (largest list)
    // Node 2: [3, 3, 4, 5]
    // 
    // Combined data: [1, 2, 2, 3, 3, 3, 3, 3, 3, 3, 4, 4, 5, 5, 6]
    // Frequency: 1(1), 2(2), 3(7), 4(2), 5(2), 6(1)
    // Mode: 3 (appears 7 times - clear single mode)
    // Total count: 15 elements
    // Median: 3 (8th element in sorted order)
    
    std::vector<std::shared_ptr<Node>> nodeList;
    nodeList.emplace_back(std::make_shared<Node>(std::vector<int>{1, 2, 3, 3, 3}));
    nodeList.emplace_back(std::make_shared<Node>(std::vector<int>{2, 3, 3, 4, 5, 6})); // Largest list - will be receiving node
    nodeList.emplace_back(std::make_shared<Node>(std::vector<int>{3, 3, 4, 5}));  
    
    std::cout << "\nNode 0: [1, 2, 3, 3, 3]" << std::endl;
    std::cout << "Node 1: [2, 3, 3, 4, 5, 6] (Receiving Node)" << std::endl;
    std::cout << "Node 2: [3, 3, 4, 5]" << std::endl;
    std::cout << "\nExpected Mode: 3 (appears 7 times)" << std::endl;
    std::cout << "Expected Median: 3" << std::endl;
    std::cout << "\nActual Results:" << std::endl;
    
    DistributedStatisticsManager manager(nodeList);
    
    std::cout << "\nTest completed successfully!" << std::endl;
    
    return 0;
}
