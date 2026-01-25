#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <future>
#include <memory>
#include <chrono>

// Order structure that contains the promise for results
struct Order {
    int order_id;
    std::string dish_type;
    std::promise<std::string> result_promise;  // Cook sets this when done
    
    Order(int id, std::string dish) : order_id(id), dish_type(dish) {}
};

class Restaurant {
private:
    // Core thread pool components
    std::queue<std::unique_ptr<Order>> order_queue;    // Work queue
    std::mutex kitchen_lock;                           // Protects the queue
    std::condition_variable doorbell;                  // Wake up mechanism
    bool restaurant_closed;                            // Shutdown flag
    std::vector<std::thread> cooks;                    // Worker threads

    // Worker function - what each cook does
    void cook_worker(int cook_id) {
        std::cout << "🧑‍🍳 Cook " << cook_id << " started working\n";
        
        while (true) {
            std::unique_ptr<Order> order;
            
            // THE CONDITION VARIABLE MAGIC ✨
            {
                std::unique_lock<std::mutex> lock(kitchen_lock);
                
                // Sleep until work available OR time to go home
                doorbell.wait(lock, [this] { 
                    return !order_queue.empty() || restaurant_closed; 
                });
                
                // Check why we woke up
                if (restaurant_closed) {
                    break;  // Time to go home
                }
                
                // Grab the order (with its promise!)
                if (!order_queue.empty()) {
                    order = std::move(order_queue.front());
                    order_queue.pop();
                }
            }
            
            // Cook the food (outside the lock)
            if (order) {
                std::cout << "🧑‍🍳 Cook " << cook_id << " cooking order " 
                          << order->order_id << " (" << order->dish_type << ")\n";
                
                // Simulate cooking time (1-3 seconds)
                int cooking_time = 1 + (rand() % 3);
                std::this_thread::sleep_for(std::chrono::seconds(cooking_time));
                
                // CRITICAL: Cook sets the result for customer
                std::string food = "🍽️ Delicious " + order->dish_type + " #" + std::to_string(order->order_id);
                order->result_promise.set_value(food);  // Customer gets notification!
                
                std::cout << "✅ Cook " << cook_id << " finished order " << order->order_id << "\n";
            }
        }
        
        std::cout << "👋 Cook " << cook_id << " went home\n";
    }

public:
    // Constructor: Open restaurant with N cooks
    Restaurant(int num_cooks) : restaurant_closed(false) {
        std::cout << "🏪 Opening restaurant with " << num_cooks << " cooks\n";
        
        for (int i = 0; i < num_cooks; i++) {
            cooks.emplace_back(&Restaurant::cook_worker, this, i + 1);
        }
    }
    
    // Customer places order and gets future (receipt)
    std::future<std::string> place_order(int order_id, const std::string& dish_type) {
        auto order = std::make_unique<Order>(order_id, dish_type);
        
        // Get the future BEFORE moving the order
        auto future = order->result_promise.get_future();  // 🎫 Customer gets receipt
        
        // Add order to queue
        {
            std::lock_guard<std::mutex> lock(kitchen_lock);
            order_queue.push(std::move(order));
            std::cout << "📝 Customer placed order " << order_id << " (" << dish_type << ")\n";
        }
        
        doorbell.notify_one();  // 🔔 Wake up ONE cook
        
        return future;  // Customer leaves with receipt
    }
    
    // Shutdown the restaurant
    void shutdown() {
        std::cout << "\n🔐 === CLOSING RESTAURANT ===\n";
        
        // Step 1: Signal shutdown
        {
            std::lock_guard<std::mutex> lock(kitchen_lock);
            restaurant_closed = true;
        }
        
        // Step 2: Wake everyone up
        doorbell.notify_all();  // 🔔🔔🔔 Wake up ALL cooks
        
        // Step 3: Wait for everyone to leave (CRITICAL!)
        for (auto& cook : cooks) {
            cook.join();
        }
        
        std::cout << "✅ Restaurant closed! All cooks went home.\n";
    }
    
    // Destructor: Auto-shutdown if not done manually
    ~Restaurant() {
        if (!restaurant_closed) {
            shutdown();
        }
    }
};

// Demo showing different ways customers can get results
void demonstrate_customer_patterns(Restaurant& restaurant) {
    std::vector<std::future<std::string>> customer_receipts;
    std::vector<std::string> dish_types = {"Pizza", "Burger", "Pasta", "Salad", "Steak"};
    
    std::cout << "\n👥 === CUSTOMERS PLACING ORDERS ===\n";
    
    // Customers place orders and collect receipts
    for (int i = 1; i <= 10; i++) {
        std::string dish = dish_types[i % dish_types.size()];
        auto receipt = restaurant.place_order(i, dish);
        customer_receipts.push_back(std::move(receipt));
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
    
    std::cout << "\n⏰ === CUSTOMERS WAITING FOR FOOD ===\n";
    
    // Demonstrate different waiting patterns
    for (int i = 0; i < customer_receipts.size(); i++) {
        if (i < 3) {
            // Pattern 1: Blocking wait (most common)
            std::cout << "👤 Customer " << (i+1) << " waiting patiently...\n";
            std::string food = customer_receipts[i].get();  // 🍔 Blocks until ready
            std::cout << "😋 Customer " << (i+1) << " received: " << food << "\n";
            
        } else if (i < 6) {
            // Pattern 2: Check with timeout
            std::cout << "👤 Customer " << (i+1) << " will wait max 5 seconds...\n";
            if (customer_receipts[i].wait_for(std::chrono::seconds(5)) == std::future_status::ready) {
                std::string food = customer_receipts[i].get();
                std::cout << "😋 Customer " << (i+1) << " received: " << food << "\n";
            } else {
                std::cout << "😤 Customer " << (i+1) << " got impatient and left!\n";
            }
            
        } else {
            // Pattern 3: Periodic checking
            std::cout << "👤 Customer " << (i+1) << " checking periodically...\n";
            while (true) {
                if (customer_receipts[i].wait_for(std::chrono::milliseconds(500)) == std::future_status::ready) {
                    std::string food = customer_receipts[i].get();
                    std::cout << "😋 Customer " << (i+1) << " received: " << food << "\n";
                    break;
                } else {
                    std::cout << "📱 Customer " << (i+1) << " checking phone while waiting...\n";
                }
            }
        }
    }
    
    std::cout << "\n🎉 === ALL CUSTOMERS SERVED! ===\n";
}

int main() {
    srand(time(nullptr));  // Random cooking times
    
    // Create restaurant with 3 cooks (thread pool size = 3)
    Restaurant restaurant(3);
    
    std::cout << "\n🚀 Starting restaurant simulation...\n";
    
    // Demo: Show different customer waiting patterns
    demonstrate_customer_patterns(restaurant);
    
    // Demo: Stress test with many orders
    std::cout << "\n💪 === STRESS TEST: 100 ORDERS ===\n";
    std::vector<std::future<std::string>> stress_receipts;
    
    for (int i = 1; i <= 100; i++) {
        auto receipt = restaurant.place_order(i + 1000, "FastFood");
        stress_receipts.push_back(std::move(receipt));
    }
    
    std::cout << "📊 Processing 100 orders with 3 cooks...\n";
    int completed = 0;
    for (auto& receipt : stress_receipts) {
        receipt.get();  // Wait for completion
        completed++;
        if (completed % 20 == 0) {
            std::cout << "✅ Completed " << completed << "/100 orders\n";
        }
    }
    
    std::cout << "\n🏆 All orders completed! Restaurant was efficient!\n";
    
    // Restaurant automatically shuts down when going out of scope
    return 0;
}
