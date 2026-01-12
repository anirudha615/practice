#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <algorithm>
#include <stack>
#include <chrono>

using namespace std;

// Every spot will have a vehicle completely occupied
struct ParkingSpot {
    int m_spotId;
    int m_ticketId = -1;
    long m_timeEntered = -1;
    int m_spotSpace;
    ParkingSpot() {}
    ParkingSpot(int spotId, int spotSpace) : m_spotId(spotId), m_spotSpace(spotSpace) {}

    const bool operator<(const ParkingSpot& otherSpot) const {
        return m_spotSpace < otherSpot.m_spotSpace; //Sort them ascending
    }
};

// Space every vehicle Occupies
struct Vehicle {
    string m_vehicleType;
    int m_spaceOccupy;
    ParkingSpot m_parkingSpot;
    Vehicle(string vehicleType, int spaceOccupy) : m_vehicleType(vehicleType), m_spaceOccupy(spaceOccupy) {}
};

/**
 * Requirements:
    1. System supports three vehicle types: Motorcycle, Car, Large Vehicle
    2. When a vehicle enters, system automatically assigns an available compatible spot
    3. System issues a ticket at entry.
    4. When a vehicle exits, user provides ticket ID
    - System validates the ticket
    - Calculates fee based on time spent (hourly, rounded up)
    - Frees the spot for next use
    5. Pricing is hourly with same rate for all vehicles
    6. System rejects entry if no compatible spot is available
    7. System rejects exit if ticket is invalid or already used
 */
/**
 * Best fit problem similar to Amazon locker - find the spot which just fits the car and don't give a larger spot to a small car
 */
class ParkingManagement {
private:
    std::set<ParkingSpot> m_availableSpots;
    const int HOURLY_FEE = 10;

    long getCurrentTimeInHours() {
        return std::chrono::duration_cast<std::chrono::hours>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    int calculateFee(ParkingSpot m_parkingSpot) {
        return HOURLY_FEE * (getCurrentTimeInHours() - m_parkingSpot.m_timeEntered);
    }

public: 

    ParkingManagement(std::set<ParkingSpot> availableSpots) : m_availableSpots(availableSpots) {}

    /**
     * Workflow - 
     *    1. Find if there any spot left which rightly fits the vehicle.
     *    2. Once spot found, assign a ticket to the spot and remove it from available spots. 
     *    3. If nothing is found, return null
     */
    ParkingSpot getAvailableParkingSpot(Vehicle& vehicle) {
        ParkingSpot spot;
        // If the vehicle already has a parking spot, return it immediately.
        if (vehicle.m_parkingSpot.m_ticketId != -1) {
            return vehicle.m_parkingSpot;
        }

        ParkingSpot dummySpot (-1, vehicle.m_spaceOccupy);
        auto itr = m_availableSpots.lower_bound(dummySpot); // O(LogN)
        if (itr == m_availableSpots.end()) {
            // Nothing found
            return spot;
        }

        // Reserve the spot
        spot = *itr;
        spot.m_ticketId = rand() % 100; // assign a random ticketID
        spot.m_timeEntered = getCurrentTimeInHours();
        vehicle.m_parkingSpot = spot; // Link Spot and Vehicle
        //Remove the spot from available sets
        m_availableSpots.erase(itr); // O(LogN)

        return spot;
    }

    /**
     * Workflow - 
     *    1. If the exitSpot is called for vehicle which does have parking spot, return -1
     *    2. If the vehicle has a parking spot, retrieve the timeEntered and calculate the fee.
     *    3. Reassign the vehicle with parkingSpot as nullptr
     *    4. Add the freed up spot to the available spots
     */
    int exitSpot(Vehicle& vehicle) {
        if (vehicle.m_parkingSpot.m_ticketId == -1) {
            return -1;
        }

        // Calculate fee and break the connection between spot and vehicle
        ParkingSpot spot = vehicle.m_parkingSpot;
        int fee = calculateFee(spot);
        vehicle.m_parkingSpot = ParkingSpot();

        // Cleanup Parking Spot and to available sets
        spot.m_ticketId = -1;
        spot.m_timeEntered = -1;
        m_availableSpots.insert(spot); // O(LogN)

        return fee;
    }
};

int main() {
    std::set<ParkingSpot> availableSpots = {
        ParkingSpot(1, 10), ParkingSpot(2, 10),  // Motorcycle spots
        ParkingSpot(3, 20), ParkingSpot(4, 20),  // Car spots  
        ParkingSpot(5, 30)                       // Large vehicle spot
    };
    ParkingManagement management(availableSpots);

    cout << "=== Parking Lot Management System Test ===" << endl;
    cout << "Available spots: 2 motorcycle (10), 2 car (20), 1 large (30)" << endl;
    
    // Test 1: Different vehicle types
    cout << "\n1. Testing different vehicle types:" << endl;
    Vehicle motorcycle("Motorcycle", 8);
    Vehicle car("Car", 15);
    Vehicle largeVehicle("Large Vehicle", 25);
    
    // Test entry for each vehicle type
    ParkingSpot motorcycleSpot = management.getAvailableParkingSpot(motorcycle);
    cout << "   Motorcycle parked at spot " << motorcycleSpot.m_spotId 
         << " (space=" << motorcycleSpot.m_spotSpace << ", ticket=" << motorcycleSpot.m_ticketId << ")" << endl;
    
    ParkingSpot carSpot = management.getAvailableParkingSpot(car);
    cout << "   Car parked at spot " << carSpot.m_spotId 
         << " (space=" << carSpot.m_spotSpace << ", ticket=" << carSpot.m_ticketId << ")" << endl;
    
    ParkingSpot largeSpot = management.getAvailableParkingSpot(largeVehicle);
    cout << "   Large vehicle parked at spot " << largeSpot.m_spotId 
         << " (space=" << largeSpot.m_spotSpace << ", ticket=" << largeSpot.m_ticketId << ")" << endl;
    
    // Test 2: No suitable spot available
    cout << "\n2. Testing no suitable spot scenario:" << endl;
    Vehicle anotherLarge("Another Large", 25);
    ParkingSpot noSpot = management.getAvailableParkingSpot(anotherLarge);
    cout << "   Large vehicle #2: " << (noSpot.m_ticketId == -1 ? "No spot available" : "Got spot") << endl;
    
    // Test 3: Vehicle exit and fee calculation
    cout << "\n3. Testing vehicle exit and fees:" << endl;
    int motorcycleFee = management.exitSpot(motorcycle);
    cout << "   Motorcycle exit fee: $" << motorcycleFee << endl;
    
    int carFee = management.exitSpot(car);  
    cout << "   Car exit fee: $" << carFee << endl;
    
    // Test 4: Re-entry after exit (spot should be available again)
    cout << "\n4. Testing re-entry after exit:" << endl;
    Vehicle newMotorcycle("New Motorcycle", 9);
    ParkingSpot reentrySpot = management.getAvailableParkingSpot(newMotorcycle);
    cout << "   New motorcycle parked at spot " << reentrySpot.m_spotId 
         << " (should reuse freed spot)" << endl;
    
    // Test 5: Double exit attempt (should fail)
    cout << "\n5. Testing double exit attempt:" << endl;
    int invalidFee = management.exitSpot(motorcycle);
    cout << "   Motorcycle double exit: " << (invalidFee == -1 ? "Correctly rejected" : "ERROR: Allowed double exit") << endl;
    
    // Test 6: Vehicle trying to get spot when already parked
    cout << "\n6. Testing already parked vehicle:" << endl;
    ParkingSpot duplicateSpot = management.getAvailableParkingSpot(largeVehicle);
    cout << "   Large vehicle getting second spot: " << (duplicateSpot.m_ticketId != -1 ? "Returned existing spot" : "ERROR: Lost parking info") << endl;
    
    cout << "\n=== Parking System Test Completed ===" << endl;
    return 0;
}
