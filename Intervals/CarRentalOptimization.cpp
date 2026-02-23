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
};

struct Cmp {
    bool operator()(const std::shared_ptr<Car> CarA, const std::shared_ptr<Car> CarB) {
        return CarA->m_pickupTime > CarB->m_pickupTime; // ascending
    }
};

/**
 * 1. As per interval pattern of overlapping intervals, sort them by start time.
 * 2. Create a minHeap where you sort by pickup time in ascending.
 * 3. Since you have been given total cars, we are asked to utilize as less as possible.
 * 4. So, we will populate the minHeap with 1 car for the first interval.
 * 5. We will add new car only if - 
 *      1. The current interval start time < top car's pick up time. Hence, we need a new car.
 *      2. If a new car is allowed to be picked based on total cars.
 * 6. If neither of those conditions met, we will mention that interval can't be processed.
 * 
 * Link: https://www.hellointerview.com/community/questions/rental-car-assignment/cm6jwxse800j9ui4biheeyqm2
 */
std::vector<std::pair<int, int>> getRentals(std::vector<std::tuple<int, int, int>> carPickupTimes, int totalCars) {
    std::vector<std::pair<int, int>> rentals;

    // Sort by start timestamp
    std::sort(carPickupTimes.begin(), carPickupTimes.end(), [](const std::tuple<int, int, int>& a, const std::tuple<int, int, int>& b) {
        return std::get<1>(a) < std::get<1>(b); // Sort by start time O(K LogK)
    });

    // populate the heap with the first car
    std::priority_queue<std::shared_ptr<Car>, std::vector<std::shared_ptr<Car>>, Cmp> carList;
    carList.push(std::make_shared<Car>(carList.size(), std::get<2>(carPickupTimes.at(0))));
    rentals.push_back({0, carList.top()->m_index});

    for (int currentIndex = 1; currentIndex < carPickupTimes.size(); currentIndex++) {
        std::shared_ptr<Car> car = carList.top();
        
        // Current interval's start time >= top car's pickup time, take the car and update the car.
        if (std::get<1>(carPickupTimes.at(currentIndex)) >= car->m_pickupTime) {
            carList.pop();
            car->m_pickupTime = std::get<2>(carPickupTimes.at(currentIndex));
            carList.push(car);
            rentals.push_back({currentIndex, car->m_index});
        } else if (carList.size() < totalCars) {
            // If the top car's pickup time is > current interval's start time, you can pickup a new car if allowed.
            std::shared_ptr<Car> newCar = std::make_shared<Car>(carList.size(), std::get<2>(carPickupTimes.at(currentIndex)));
            carList.push(newCar);
            rentals.push_back({currentIndex, newCar->m_index});
        } else {
            // If the top car's pickup time is > current interval's start time, there is no point in scanning
            // other car as their pick up time is > than top car's pick up time.
            // Mention that rental can't be processed.
            rentals.push_back({currentIndex, -1});
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
