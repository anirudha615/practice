#include <iostream>
using namespace std;

/**
 * https://www.hellointerview.com/community/questions/design-underground-system/cm5eguhae0336838omvw050la?level=SENIOR
 */
class UndergroundSystem {
private:
    std::unordered_map<int, std::pair<string, int>> m_pendingCheckOut; // customerId --> StationName, time (During checkin)
    std::unordered_map<string, std::vector<int>> m_averageTime; // station_start_end : <Total Time list>

    string normalizeString(string startStation, string endStation) {
        return startStation + "_" + endStation;
    }
public:
    UndergroundSystem() {
        
    }
    
    void checkIn(int id, string stationName, int t) {
        m_pendingCheckOut[id] = {stationName, t};
    }
    
    void checkOut(int id, string stationName, int t) {
        if (!m_pendingCheckOut.count(id)) {
            return;
        }

        std::pair<string, int> checkIn = m_pendingCheckOut[id];
        int totalTime = t - checkIn.second;

        string normalizedStartEndStation = normalizeString(checkIn.first, stationName);
        m_averageTime[normalizedStartEndStation].push_back(totalTime);
    }
    
    double getAverageTime(string startStation, string endStation) {
        string normalizedStartEndStation = normalizeString(startStation, endStation);
        std::vector<int> averageTimes = m_averageTime[normalizedStartEndStation];
        int totalSum = 0;
        int datapoints = 0;
        if (averageTimes.empty()) {
            return totalSum;
        }

        for (int value : averageTimes) {
            totalSum += value;
            datapoints++;
        }

        return (double)totalSum/datapoints;
    }
};

int main() {
    UndergroundSystem undergroundSystem;
    undergroundSystem.checkIn(45, "Leyton", 3);
    undergroundSystem.checkIn(32, "Paradise", 8);
    undergroundSystem.checkIn(27, "Leyton", 10);
    undergroundSystem.checkOut(45, "Waterloo", 15);  // Customer 45 "Leyton" -> "Waterloo" in 15-3 = 12
    undergroundSystem.checkOut(27, "Waterloo", 20);  // Customer 27 "Leyton" -> "Waterloo" in 20-10 = 10
    undergroundSystem.checkOut(32, "Cambridge", 22); // Customer 32 "Paradise" -> "Cambridge" in 22-8 = 14
    std::cout << undergroundSystem.getAverageTime("Paradise", "Cambridge") << std::endl; // return 14.00000. One trip "Paradise" -> "Cambridge", (14) / 1 = 14
    std::cout << undergroundSystem.getAverageTime("Leyton", "Waterloo") << std::endl; // return 11.00000. Two trips "Leyton" -> "Waterloo", (10 + 12) / 2 = 11
    undergroundSystem.checkIn(10, "Leyton", 24);
    std::cout << undergroundSystem.getAverageTime("Leyton", "Waterloo") << std::endl;   // return 11.00000
    undergroundSystem.checkOut(10, "Waterloo", 38);  // Customer 10 "Leyton" -> "Waterloo" in 38-24 = 14
    std::cout << undergroundSystem.getAverageTime("Leyton", "Waterloo") << std::endl;    // return 12.00000. Three trips "Leyton" -> "Waterloo", (10 + 12 + 14) / 3 = 12
}