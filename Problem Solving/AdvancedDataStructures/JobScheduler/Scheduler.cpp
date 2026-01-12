#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <future>
#include <memory>
#include <chrono>

using namespace std;

struct OrderRequest {
    int m_orderId;
    string m_cuisine;
    std::promise<string> m_promise;
    long m_executionTime;
    bool m_isCancelled = false;
    bool m_isExecuted = false;
    OrderRequest(int orderId, string cuisine, long executionTime) 
    : m_orderId(orderId), m_cuisine(cuisine), m_executionTime(executionTime) {}
};

// For Pointer Comparison
struct Comparator {
    bool operator()(std::shared_ptr<OrderRequest> a, std::shared_ptr<OrderRequest> b) const {
        return a->m_executionTime > b->m_executionTime; // Ascending
    }
};

class ExecutorPool {
private:
    std::vector<std::thread> m_threadPool;
    std::queue<std::shared_ptr<OrderRequest>> m_orderQueue; // Queue of orders ready to be executed by worker threads
    std::mutex m_queueMutex;
    std::condition_variable m_doorbell;
    std::atomic<bool> m_shutdownExecutor{false};

    void executeOrder(int worker_id) {
        while (!m_shutdownExecutor.load()) {
            
            // STEP 1: Grab an order
            std::shared_ptr<OrderRequest> currentRequest;
            {
                std::unique_lock<std::mutex> lock(m_queueMutex);
                
                // Sleep until work available OR time to abort
                m_doorbell.wait(lock, [this] { 
                    return !m_orderQueue.empty() || m_shutdownExecutor.load();
                });

                // Once the worker thread wakes up, check is it due to a new order or shutdown signal
                if (m_shutdownExecutor.load()) {
                    return; // exit the loop and close gracefully. Wait for main thread to cleanup the thread via join();
                }

                // If it came here, that means order queue has an order.
                currentRequest = m_orderQueue.front();
                m_orderQueue.pop();
            }

            // STEP 2: Simulate executing order and set up a promise.
            if (currentRequest) {
                std::string food;
                if (currentRequest->m_isCancelled) {
                    food = "Delicious " + currentRequest->m_cuisine + " #" + std::to_string(currentRequest->m_orderId) + " is now cancelled";
                } else {
                    std::cout << "Worker " << worker_id << " is executing order " << currentRequest->m_orderId << " (" << currentRequest->m_cuisine << ")\n";
                
                    // Simulate cooking time (1-3 seconds)
                    int cooking_time = 1 + (rand() % 3);
                    std::this_thread::sleep_for(std::chrono::seconds(cooking_time));
                    
                    // CRITICAL: Cook sets the result for customer
                    food = "Delicious " + currentRequest->m_cuisine + " #" + std::to_string(currentRequest->m_orderId);
                    std::cout << "DONE: Worker " << worker_id << " finished order " << food << "\n";
                    currentRequest->m_isExecuted = true;
                }
                currentRequest->m_promise.set_value(food);
            }
        }
    }
public:
    ExecutorPool(int totalWorkers) {
        for (int index = 0; index < totalWorkers; index++) {
            m_threadPool.emplace_back(&ExecutorPool::executeOrder, this, index);
        }
    }

    void placeOrderToExecute(std::shared_ptr<OrderRequest> request) {
        // If the shutdown is called, don't accept new order
        if (m_shutdownExecutor.load()) {
            return;
        }
        std::lock_guard<std::mutex> lock(m_queueMutex);
        m_orderQueue.push(request); // Add the order to the queue
        m_doorbell.notify_one(); // Wake up a sleeping thread. You don't need to wake up all threads
    }

    void shutdownExecutor() {
        m_shutdownExecutor = true;
        m_doorbell.notify_all(); // If the threads are working, it will catch the atomic signal. If not, this notify_all will wake up threads.

        // cleanup all threads
        for (auto& thread : m_threadPool) {
            thread.join(); 
        }
        m_threadPool.clear();
    }
};

/**
 * Workflow - 
 *    1. 1 scheduler thread checking if the top element's execution time >= current time. If yes, placeOrderToExecute. If no, sleep for (top order execution time - current time)
 *    2. If somebody schedules an order, wake up the scheduler thread and repeat step #1
 *    3. If somebody cancels an order, verify if the order is executed. If not, cancel the order and return successfully cancelled else return false.
 */
class Scheduler {
private:
    std::thread m_schedulerThread;
    std::condition_variable m_schedulingDoorbell;
    std::unique_ptr<ExecutorPool> m_executor;
    std::mutex m_heapMutex;
    std::priority_queue<std::shared_ptr<OrderRequest>, std::vector<std::shared_ptr<OrderRequest>>, Comparator> m_minHeap;
    std::atomic<bool> m_shutdownScheduler{false};

    long getCurrentTimeInSec() {
        return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    void periodicCheck() {
        while (!m_shutdownScheduler.load()) {
            std::unique_lock<std::mutex> lock(m_heapMutex);
            
            // If the heap is empty (during initialization) or shutdown signal is false, sleep indefinitely.
            // When someone adds to the minHeap, notify_one will wake up the thread
            if (m_minHeap.empty()) {
                m_schedulingDoorbell.wait(lock, [this]{ 
                    return !m_minHeap.empty() || m_shutdownScheduler.load();
                });
                continue;
            }

            // Thread woke up because either it has a new element or the shutdown signal is activated.
            if (m_shutdownScheduler.load()) {
                // Exit the loop. Wait for the main thread to cleanup this spawned thread.
                break; 
            }

            // If it comes here, we know thread woke up because it has a new element. Retrieve the execution time of the top request.
            auto current_top = m_minHeap.top();
            auto timeToWakeUp = std::chrono::system_clock::from_time_t(current_top->m_executionTime);
            
            // If the request is yet to be executed, wait till its execution time.
            // At the same time, we will also monitor if a more urgent request enters the heap.
            // If it enters, notify_one will ping the thread and thread will check if m_minHeap.top() != current_top.
            auto status = m_schedulingDoorbell.wait_until(lock, timeToWakeUp, [this, current_top]{ 
                return m_minHeap.empty() || m_minHeap.top() != current_top; 
            });
            
            // If the thread woke up because of urgent job, it will check the top element and see if we need to execute it.
            // IF not, in the next iteration, it will calculate timeToWakeUp of the urgent request and sleep till that time.
            if (!m_minHeap.empty() && getCurrentTimeInSec() >= m_minHeap.top()->m_executionTime) {
                m_executor->placeOrderToExecute(m_minHeap.top());  // Execute current top
                m_minHeap.pop();
            }
        }
    }

public:
    Scheduler(int totalWorkers) {
        m_executor = std::make_unique<ExecutorPool>(totalWorkers);
        m_schedulerThread = std::thread(&Scheduler::periodicCheck, this);
    }

    std::pair<std::shared_ptr<OrderRequest>, std::future<string>> scheduleOrder(int orderId, string cuisine, long executionTime) {
        // If the shutdown is called, don't schedule new order and return a future with exception.
        if (m_shutdownScheduler.load()) {
            std::promise<string> promise;
            promise.set_exception(std::make_exception_ptr(
                std::runtime_error("Scheduler is shutting down - no new orders accepted")
            ));
            auto future = promise.get_future();
            
            // Return pair with nullptr order and exception future
            return std::make_pair(nullptr, std::move(future));
        }

        // Schedule Orders
        std::shared_ptr<OrderRequest> order = std::make_shared<OrderRequest>(orderId, cuisine, executionTime);
        std::future<string> future = order->m_promise.get_future();  // Customer gets receipt
        {
            std::lock_guard<std::mutex> lock(m_heapMutex);
            m_minHeap.push(order); // Add an order to min Heap
            // Wake up sleeping threads to re-check the condition of !m_minHeap.empty() or m_minHeap.top() != current.
            m_schedulingDoorbell.notify_all(); 

        }
        return std::make_pair(order, std::move(future));
    }

    bool cancelAnOrder(std::shared_ptr<OrderRequest> request) {
        // If the shutdown is called, don't modify existing order
        if (m_shutdownScheduler.load()) {
            std::cout << "Scheduler is shutting down - Order " << request->m_orderId << " can't be modified." << std::endl;
            return false;
        }
        if (request->m_isExecuted) {
            std::cout << "Order " << request->m_orderId << " is already executed." << std::endl;
            return false;
        }
        if (request->m_isCancelled) {
            std::cout << "Order " << request->m_orderId << " is already cancelled." << std::endl;
            return false;
        }

        // Cancel orders. If the orders are already being executed, it cannot be cancelled but if its is not, execution checks if the order is cancelled.
        request->m_isCancelled = true;
        std::cout << "Order " << request->m_orderId << " is cancelled" << std::endl;
        return true;
    }

    void shutDownScheduler() {
        m_shutdownScheduler = true;
        m_schedulingDoorbell.notify_all(); // If the threads are working, it will catch the atomic signal. If not, this notify_all will wake up threads.
        m_schedulerThread.join(); // Cleanup the thread;
        m_executor->shutdownExecutor(); // Call Executor to shut down its own threads
    }
};

// ===== BEHAVIORAL TESTS =====

void testBasicScheduling() {
    std::cout << "\n Test 1: Basic Scheduling Behavior\n";
    
    Scheduler scheduler(2);
    auto currentTime = std::time(nullptr);
    
    // Schedule job 3 seconds in future
    auto [order, future] = scheduler.scheduleOrder(1, "Pizza", currentTime + 3);
    
    std::cout << "Scheduled Pizza for " << (currentTime + 3) << " (current: " << currentTime << ")\n";
    
    // Wait for result
    std::string result = future.get();
    std::cout << "Result: " << result << "\n";
    
    scheduler.shutDownScheduler();
    std::cout << "Test 1 PASSED: Basic scheduling works!\n";
}

void testPriorityOrdering() {
    std::cout << "\n Test 2: Priority Ordering Behavior\n";
    
    Scheduler scheduler(2);
    auto currentTime = std::time(nullptr);
    
    // Schedule in reverse priority order
    auto [orderA, futureA] = scheduler.scheduleOrder(1, "Pizza", currentTime + 6);    // Last
    auto [orderB, futureB] = scheduler.scheduleOrder(2, "Burger", currentTime + 2);   // First  
    auto [orderC, futureC] = scheduler.scheduleOrder(3, "Salad", currentTime + 4);    // Middle
    
    std::cout << "Scheduled: Pizza(t+6), Burger(t+2), Salad(t+4)\n";
    std::cout << "Expected execution order: Burger → Salad → Pizza\n";
    
    // Get results (should be in priority order due to timing)
    std::string resultB = futureB.get();
    std::cout << "First result: " << resultB << "\n";
    
    std::string resultC = futureC.get();
    std::cout << "Second result: " << resultC << "\n";
    
    std::string resultA = futureA.get();
    std::cout << "Third result: " << resultA << "\n";
    
    scheduler.shutDownScheduler();
    std::cout << "Test 2 PASSED: Priority ordering works!\n";
}

void testUrgentInterruption() {
    std::cout << "\n Test 3: Urgent Job Interruption Behavior\n";
    
    Scheduler scheduler(2);
    auto currentTime = std::time(nullptr);
    
    // Schedule job far in future
    auto [orderA, futureA] = scheduler.scheduleOrder(1, "Pizza", currentTime + 8);
    std::cout << "Scheduled Pizza for t+8 seconds\n";
    
    // Wait a bit, then add urgent job
    std::this_thread::sleep_for(std::chrono::seconds(2));
    auto [orderB, futureB] = scheduler.scheduleOrder(2, "UrgentBurger", currentTime + 4);
    std::cout << "Added urgent Burger for t+4 seconds (should interrupt Pizza wait)\n";
    
    // Burger should execute first despite being added later
    std::string resultB = futureB.get();
    std::cout << "Urgent result: " << resultB << "\n";
    
    std::string resultA = futureA.get();
    std::cout << "Original result: " << resultA << "\n";
    
    scheduler.shutDownScheduler();
    std::cout << " Test 3 PASSED: Urgent interruption works!\n";
}

void testCancellation() {
    std::cout << "\n Test 4: Cancellation Behavior\n";
    
    Scheduler scheduler(2);
    auto currentTime = std::time(nullptr);
    
    // Schedule job in future
    auto [order, future] = scheduler.scheduleOrder(1, "Pizza", currentTime + 5);
    std::cout << "Scheduled Pizza for t+5 seconds\n";
    
    // Cancel it immediately
    bool cancelled = scheduler.cancelAnOrder(order);
    std::cout << "Cancellation result: " << (cancelled ? "SUCCESS" : "FAILED") << "\n";
    
    // Get result (should be cancellation message)
    std::string result = future.get();
    std::cout << "Result: " << result << "\n";
    
    // Try to cancel again (should fail)
    bool cancelledAgain = scheduler.cancelAnOrder(order);
    std::cout << "Cancel again result: " << (cancelledAgain ? "SUCCESS" : "FAILED") << "\n";
    
    scheduler.shutDownScheduler();
    std::cout << " Test 4 PASSED: Cancellation works!\n";
}

void testShutdown() {
    std::cout << "\n Test 5: Shutdown Behavior\n";
    
    Scheduler scheduler(2);
    auto currentTime = std::time(nullptr);
    
    // Schedule normal job
    auto [order1, future1] = scheduler.scheduleOrder(1, "Pizza", currentTime + 3);
    std::cout << "Scheduled Pizza normally\n";
    
    // Shutdown scheduler
    scheduler.shutDownScheduler();
    std::cout << "Scheduler shut down\n";
    
    // Try to schedule after shutdown (should fail)
    auto [order2, future2] = scheduler.scheduleOrder(2, "Burger", currentTime + 2);
    
    if (order2 == nullptr) {
        std::cout << " New order correctly rejected after shutdown\n";
        try {
            future2.get();
        } catch (const std::exception& e) {
            std::cout << "Exception received: " << e.what() << "\n";
        }
    }
    
    std::cout << " Test 5 PASSED: Shutdown behavior works!\n";
}

int main() {
    srand(time(nullptr));  // Initialize random for cooking times
    
    std::cout << " ===== JOB SCHEDULER BEHAVIORAL TESTS =====\n";
    
    try {
        testBasicScheduling();
        testPriorityOrdering();
        testUrgentInterruption();
        testCancellation();
        testShutdown();
        
        std::cout << "\n ===== ALL TESTS PASSED! =====\n";
        
    } catch (const std::exception& e) {
        std::cout << "Test failed with exception: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
