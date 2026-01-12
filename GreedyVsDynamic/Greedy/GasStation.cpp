#include <iostream>
#include <unordered_map>
#include <cmath>
#include <numeric> // Required for std::accumulate
using namespace std;

/**
 * NON-OPTIMIZED APPROACH - O(N2)
 * 1. Filter ths starting index to be where gas_filled[index] >= gas_utilized[index]
 * 2. For every potential candidates, try a simulation to complete the circuit by updating fuel
 */
int canCompleteCircuitNonOptimized(vector<int>& gas_filled, vector<int>& gas_utilized) {
    
    for (int station = 0; station < gas_filled.size(); station++) {
        if (gas_filled.at(station) >= gas_utilized.at(station)) {
            // start simulation whether starting at that station can complete the circuit
            int currentStation = station;
            int fuel = 0;
            do {
                // Check gas to move to the next station
                fuel += gas_filled.at(currentStation) - gas_utilized.at(currentStation);
                // If fuel is greater or equal to 0, we moved to the next station and hence increase current station
                if (fuel < 0) {
                    // If fuel is less than 0, break as the station does not help to complete the circuit.
                    break;
                }
                currentStation++;
                if (currentStation == gas_filled.size()) {
                    currentStation = 0;
                }
            } while (currentStation != station);
            // circuit completed
            if (currentStation == station && fuel >=0) {
                return station;
            }
        }
    }

    // If nothing found, return -1
    return -1;
}

/**
 * OPTIMIZED APPROACH - O(N)
 * 1. Calculate the sum of gas filled and gas utilized. If gas filled > gas utilized, there is a solution else no solution.
 * 2. Since there is a solution, mark the starting stattion as index 0 and loop through every station, if gas_filled[index] >= gas_utilized[index], update the fuel.
 * 3. If the conditon does not satisfy, the starting station would be next one 
 *    because any station that starts from next of previous starting station will fail at the same station (where the previous starting station failed)
 * 
 */
int canCompleteCircuitOptimized(vector<int>& gas_filled, vector<int>& gas_utilized) {
    int gas_filled_sum = std::accumulate(gas_filled.begin(), gas_filled.end(), 0);
    int gas_utilized_sum = std::accumulate(gas_utilized.begin(), gas_utilized.end(), 0);
    if (gas_filled_sum < gas_utilized_sum) {
        return -1;
    }

    // Now we know there is a solution
    int startingStation = 0;
    int fuel = 0;
    for (int currentStation = 0; currentStation < gas_filled.size(); currentStation++) {
        // Cannot move to the next station, then starting station would be the next station
        if (fuel + gas_filled.at(currentStation) - gas_utilized.at(currentStation) < 0) {
            startingStation = currentStation + 1;
            fuel = 0;
        } else {
            fuel += gas_filled.at(currentStation) - gas_utilized.at(currentStation);
        }
    }

    return startingStation;
}

int main() {
    std::vector<int> gas_filled {1,2,3,4,5};
    std::vector<int> gas_utilized {3,4,5,1,2};
    std::cout << canCompleteCircuitOptimized(gas_filled, gas_utilized) << std::endl;
}

