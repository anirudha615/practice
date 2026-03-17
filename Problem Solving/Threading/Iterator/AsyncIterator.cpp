#include <iostream>
#include <vector>
#include <thread>
#include <future>
#include <atomic>

using namespace std;

struct DataSource {
    int m_currentPosition = 0;
    int m_totalPosition;
    int jumpBy = 1 + (rand() % 11);
    DataSource(int totalPosition): m_totalPosition(totalPosition) {}
};

/**
 * Link: https://www.hellointerview.com/community/questions/resumable-iterator/cmbskz7ck004r07ad6f1jxlni
 */
class AsyncIterator {
private:
    std::vector<std::shared_ptr<DataSource>> dataSourceList;
    std::vector<std::thread> activeWorkerThreads;
    // this is atomic and shared between main thread and all the new threads created
    std::atomic<bool> resumeSignal{false}; // resume and pause on the while loop
    std::atomic<bool> stopSignal{false}; // terminate the while loop
    std::mutex m_resumeMutex;
    std::condition_variable m_doorbell;

    void incrementDataSource(std::shared_ptr<DataSource> dataSource) {
        while (!stopSignal.load()) {

            // lock the pause mutex
            std::unique_lock<std::mutex> lock(m_resumeMutex);

            // Sleep indefinitely till resume signal or stop signal is true
            m_doorbell.wait(lock, [this] {
                return resumeSignal.load() || stopSignal.load();
            });

            // When the thread wake up, check if it is asked to stop and abort
            if (stopSignal.load()) {
                break;
            }

            // If you reached there, resume signal is true but stop signal is false
            if (dataSource->m_currentPosition < dataSource->m_totalPosition) {
                dataSource->m_currentPosition += dataSource->jumpBy;
            }
            std::this_thread::sleep_for(chrono::milliseconds(100));
        }
        
    }

public:
    /**
     * (2) Create thread per data source and call incrementDataSource.
       (3) Push all those threads into the activeWorkerThreads
     */
    AsyncIterator(std::vector<int> dataSourceTotalPosition) {
        for (auto position : dataSourceTotalPosition) {
            std::shared_ptr<DataSource> dataSource = std::make_shared<DataSource>(position);
            dataSourceList.push_back(dataSource);
            activeWorkerThreads.push_back(std::thread(&AsyncIterator::incrementDataSource, this, dataSource));
        }
    }
    
    /**
     * Resume Playing - 
     *    1. If the resumeSignal is false, make it true.
     *    2. Notify all threads to wake up.
     */
    void resumePlaying() {
        if (!resumeSignal.load()) {
            resumeSignal = true;
            m_doorbell.notify_all();
            std::cout << "All threads notified to resume" << std::endl;
        } else {
            std::cout << "All threads are already resumed" << std::endl;
        }
    }

    /**
     * Pause Playing - Mark resumeSignal as false and notify all threads.
     */
    std::vector<int> stopPlayingAndGetState() {
        std::vector<int> stateList;
        if (resumeSignal.load()) {
            resumeSignal = false;
            m_doorbell.notify_all();
            std::cout << "All threads notified to pause" << std::endl;
        } else {
            std::cout << "All threads are already paused" << std::endl;
        }
        
        for (auto dataSource : dataSourceList) {
            stateList.push_back(dataSource->m_currentPosition);
        }

        return stateList;
    }

    /**
     * Set the state from where to play in DataSource itself and call setStateAndResumePlaying() 
     * which will enable the resume signal and notify all threads
     */
    void setStateAndResumePlaying(std::vector<int> currentPosition) {
        for (int index = 0; index < currentPosition.size(); index++) {
            dataSourceList.at(index)->m_currentPosition = currentPosition.at(index);
        }
        resumePlaying();
    }

    /**
     * Request all threads to terminate the while loop
     */
    void stopPlaying() {
        if (stopSignal.load()) {
            std::cout << "All threads are already aborted" << std::endl;
            return;
        }

        stopSignal = true;
        m_doorbell.notify_all();
        std::cout << "All threads notified to stop and abort" << std::endl;

        // Join all threads properly
        for (auto& worker : activeWorkerThreads) {
            if (worker.joinable()) {
                worker.join();
            }
        }
        activeWorkerThreads.clear();
    }
    
    // Add destructor to prevent crash on object destruction
    ~AsyncIterator() {
        stopPlaying();
    }
};

int main() {
    AsyncIterator iterator({6000, 2000, 9000});
    
    cout << "=== Async Iterator with Pause/Resume Test ===" << endl;
    cout << "3 Data sources with sizes: 6000, 2000, 9000" << endl;
    
    // Test 1: Start concurrent iteration
    iterator.resumePlaying();
    cout << "   All 3 threads started and running concurrently" << endl;
    
    // Let threads run for 2 seconds
    this_thread::sleep_for(chrono::seconds(2));
    cout << "   Threads have been processing data since 2 seconds ..." << endl;
    
    // Test 2: Pause and capture state
    vector<int> pausedState = iterator.stopPlayingAndGetState();
    cout << "   All threads paused and state captured" << endl;
    for (int i = 0; i < pausedState.size(); i++) {
        cout << "     DataSource " << i << ": position " << pausedState[i] << endl;
    }
    
    // Test 3: Create new iterator and restore state
    cout << "\n4. Creating new iterator and restoring state..." << endl;
    AsyncIterator iterator2({6000, 2000, 9000});
    iterator2.setStateAndResumePlaying(pausedState);
    
    // Let it run for another 1 second
    cout << "\n5. Running for 1 more second from restored state..." << endl;
    this_thread::sleep_for(chrono::seconds(1));
    
    // Final pause to show progression
    cout << "\n6. Final pause to show progression from restored state..." << endl;
    vector<int> finalState = iterator2.stopPlayingAndGetState();
    for (int i = 0; i < finalState.size(); i++) {
        cout << "     DataSource " << i << ": position " << finalState[i] 
             << " (progressed " << (finalState[i] - pausedState[i]) << " from restored state)" << endl;
    }
    
    cout << "\n7. Stopping all remaining threads..." << endl;
    iterator2.stopPlaying();
    
    cout << "\n=== Async Iterator Test Completed ===" << endl;
    cout << "Successfully demonstrated pause, state capture, restoration, and resume!" << endl;
    
    return 0;
}
