#include <iostream>
#include <map>
#include <vector>
#include <set>
#include <algorithm>

using namespace std;

struct Car {
    int m_pickupTime;
    int m_index;
    Car(int index, int pickupTime) : m_pickupTime(pickupTime), m_index(index) {}
    bool operator<(const Car& otherCar) const {
        if (m_pickupTime != otherCar.m_pickupTime) {
            return m_pickupTime < otherCar.m_pickupTime;
        }
        return m_index < otherCar.m_index;
    }

};

/**
 * 1. As per interval pattern of overlapping intervals, sort them by start time.
 * 2. On the other hand, create a self-balancing BST of TotalCars you have.
 *     a. First thought of minHeap but we need to find a car whose pickup time is just <= interval's starting time.
 *     b. If we use minHeap, we need to pop out the elements until we find the max element 
 *        (from the popped ones) which is just <= interval's starting time, which could be O(N LogN)
 *     c. Hence, went with self-balancing BST where we can find the same in O(LogN)
 * 
 * 3. Loop for all intervals (K) and find the car whose pickup time is just <= interval's starting time in O(LogN)
 *    Total complexity is O(K LogN)
 * 
 * Link: https://www.hellointerview.com/community/questions/rental-car-assignment/cm6jwxse800j9ui4biheeyqm2
 */
std::vector<std::pair<int, int>> getRentals(std::vector<std::tuple<int, int, int>> carPickupTimes, int totalCars) {
    std::vector<std::pair<int, int>> rentals;

    // Create a self balancing Binary Search Tree. 
    std::set<Car> carBST;
    for (int index = 0; index < totalCars; index++) {
        carBST.insert(Car(index, -1)); // O(N LogN)
    }

    std::sort(carPickupTimes.begin(), carPickupTimes.end(), [](const std::tuple<int, int, int>& a, const std::tuple<int, int, int>& b) {
        return std::get<1>(a) < std::get<1>(b); // Sort by start time O(K LogK)
    });

    // O(K LogN) = K intervals
    for (std::tuple<int, int, int> request : carPickupTimes) {
        // STEP 1: Find me a car whose pickup time is <= interval's starting time
        Car dummyCarRequest (INT_MAX, std::get<1>(request));
        // Find a car whose pickup time is just > interval's starting time
        auto itr = carBST.upper_bound(dummyCarRequest); // O(Log N)
        if (itr != carBST.begin()) {
            --itr; // and then decrement to get '<='. 
            auto carToBeRented = *(itr);
            rentals.push_back({std::get<0>(request), carToBeRented.m_index});
            carBST.erase(carToBeRented); // O(Log N)
            carToBeRented.m_pickupTime = std::get<2>(request);
            carBST.insert(carToBeRented); // O(LogN)
        } else {
            // couldn't find any car
            rentals.push_back({std::get<0>(request), -1});
            continue;
        }
    }
    return rentals;
}

int main() {
    std::vector<std::tuple<int, int, int>> intervals {{1,0,5},{2,2,7},{3,5,9}};
    std::vector<std::pair<int, int>> rentals = getRentals(intervals, 3); // O(K LogN)
    for (auto rental : rentals) {
        std::cout << "Request ID: " << rental.first << " and Car Number : " << rental.second << std::endl;  
    }
}
