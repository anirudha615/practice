#include <iostream>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <cassert>
#include <thread>
#include <chrono>

using namespace std;

/**
 * Multiple producer threads hits PUT which will wait to enqueue for a specific time and will simply exit.
 * Multiple consumer threads hits TAKE which will wait to deque for a specific time and will simply exit.
 * 
 * Instead of using a queue, I am using a vector with fixed size for contiguous memory allocation.
 */
class Orchestrator {
private:
    std::mutex m_mtx;
    std::atomic<bool> m_shutdown {false};
    std::condition_variable m_notFull;
    std::condition_variable m_notEmpty;

    // state
    int m_totalCapacity = 0;
    int m_currentCapacity = 0;
    int m_head = 0, m_tail = 0; // head to retrieve and tail to insert - moves clockwise in a circular array
    std::vector<string> m_queue;
public:
    Orchestrator(int totalCapacity): m_totalCapacity(totalCapacity) {
        m_queue.reserve(totalCapacity);
    };

    bool put(const string& message, const int64_t& timeout) {
        if (m_shutdown.load()) {
            return false;
        }
        {
            std::unique_lock<std::mutex> lock(m_mtx);
            if (m_currentCapacity == m_totalCapacity) {
                if (!timeout) {
                    return false;
                }

                // Sleep till the timeout
                m_notFull.wait_for(lock, std::chrono::milliseconds(timeout), [this] {
                    return m_currentCapacity < m_totalCapacity || m_shutdown.load();
                });

                // Once the thread wakes up, check termination conditions
                if (m_shutdown.load() || m_currentCapacity == m_totalCapacity) {
                    return false;
                }
            }
            m_queue[m_head] = message;
            m_head = (m_head + 1) % m_totalCapacity; // Forward the head pointer
            m_currentCapacity++; // Making consumer thread to work if they are sleeping because nothing to process
            // This will wake up the consumer thread if there's any and will check current capacity 
            // is still equal to 0, which will lead to dequeue the message;
            m_notEmpty.notify_one();
            return true;
        }
    }

    string take(const int64_t& timeout) {
        if (m_shutdown.load()) {
            return "Can't Work Anymore";
        }

        {
            std::unique_lock<std::mutex> lock(m_mtx);
            if (m_currentCapacity == 0) {
                if (!timeout) {
                    return "Nothing To Process";
                }

                // Sleep until timeout
                m_notEmpty.wait_for(lock, std::chrono::milliseconds(timeout), [this]{
                    return m_currentCapacity != 0 || m_shutdown.load();
                });

                // Once the thread wakes up, check termination conditions
                if (m_shutdown || !m_currentCapacity) {
                    return "Nothing To Process";
                }
            }
            string message = m_queue[m_tail];
            m_tail = (m_tail + 1) % m_totalCapacity; // Forward the tail pointer
            m_currentCapacity--; // Making space for producer thread to enqueue.
            // This will wake up the producer thread if there's any and will check current capacity 
            // is still equal to total capacity, which will lead to enque the message;
            m_notFull.notify_one(); 
            return message;
        }
    }

    void shutdown() {
        m_shutdown = true;
        m_notEmpty.notify_all();
        m_notFull.notify_all();
    }
};

/**
 * Link: https://prachub.com/coding-questions/implement-thread-safe-blocking-queue
 */
class OrchestratorWithoutThreads {
private:
    int m_totalCapacity = 0;
    std::queue<std::pair<int, int>> m_enqueueWaitingStation;
    std::queue<int> m_dequeueWaitingStation;
    std::queue<int> m_actualQueue;
public:

    OrchestratorWithoutThreads(int totalCapacity) : m_totalCapacity(totalCapacity) {}

    std::vector<std::pair<string, string>> ENQ(const int& number, const int& timestamp) {
        // If the queue is full, you can sit in the enqueue station until deque requests 
        if (m_actualQueue.size() == m_totalCapacity) {
            m_enqueueWaitingStation.push({number, timestamp});
            return {};
        }  
        
        if (m_actualQueue.empty()) {
            // If the queue is empty, check if someone is waiting in dequeue station, skip adding to the queue.
            if (m_dequeueWaitingStation.size()) {
                m_dequeueWaitingStation.pop();
                return {{std::to_string(number), std::to_string(timestamp)}, {"OK", std::to_string(timestamp)}};
            }
            // No one is waiting at the dequeue stattion, add to the queue
            m_actualQueue.push(number);
            return {{"OK", std::to_string(timestamp)}};
        }

        // If it neither full or nor empty, push to the queue.
        m_actualQueue.push(number);
        return {{"OK", std::to_string(timestamp)}};
    }

    std::vector<std::pair<string, string>> DEQ(const int& timestamp) {
        if (m_actualQueue.empty()) {
            // If the queue is empty, check if someone is waiting in enqueue station, skip adding to the queue.
            if (m_enqueueWaitingStation.size()) {
                std::pair<int, int> enqueueRequest = m_enqueueWaitingStation.front();
                m_enqueueWaitingStation.pop();
                return {{"OK", std::to_string(timestamp)}, {std::to_string(enqueueRequest.first), std::to_string(timestamp)}};
            }
            m_dequeueWaitingStation.push(timestamp);
            return {};
        }

        // Simply poll out from actual queue
        int number = m_actualQueue.front();
        m_actualQueue.pop();
        return {{std::to_string(number), std::to_string(timestamp)}};
    }
};

void testOrchestratorWithoutThreads() {
    OrchestratorWithoutThreads q1(2);
    
    // Test Example 1: capacity=2
    auto r1 = q1.ENQ(5, 0);   assert(r1[0] == std::make_pair("OK", "0"));
    auto r2 = q1.ENQ(6, 0);   assert(r2[0] == std::make_pair("OK", "0"));
    auto r3 = q1.DEQ(1);      assert(r3[0] == std::make_pair("5", "1"));
    auto r4 = q1.DEQ(1);      assert(r4[0] == std::make_pair("6", "1"));
    auto r5 = q1.DEQ(1);      // waits
    auto r6 = q1.ENQ(7, 3);   // pairs with r5
    assert(r6.size() == 2);
    assert(r6[0] == std::make_pair("7", "3"));
    assert(r6[1] == std::make_pair("OK", "3"));
    
    // Test 3: capacity=1 edge
    OrchestratorWithoutThreads q3(1);
    assert(q3.ENQ(42, 99)[0] == std::make_pair("OK", "99"));
    assert(q3.DEQ(100)[0] == std::make_pair("42", "100"));
    
    std::cout << "All tests passed!" << std::endl;
}


int main() {
    testOrchestratorWithoutThreads();
    return 0;
}