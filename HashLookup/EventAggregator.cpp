#include <iostream>
#include <iterator>
#include <set>

using namespace std;

bool evaluateFrequencyBreach(std::multiset<int>& timestamp) {
        int hourWindow = 99;
        // For every timestamp, go back an hour and see how many access requests exists. 
        // If at any time, access attempts >=3, break and return true.
        for (auto itr = timestamp.begin(); itr != timestamp.end(); ++itr) {
            auto lowerBoundItr = timestamp.lower_bound(*itr - hourWindow);
            // distance is inclusive of FIRST but exclusive of LAST, so +1.
            int frequency = (int)std::distance<std::set<int>::const_iterator>(lowerBoundItr, itr) + 1;
            if (frequency >= 3) {
                return true;
            }
        }
        return false;
    }

/**
 * https://www.hellointerview.com/community/questions/high-access-employees/cm5eguhai049v838o1wpr6reg?level=STAFF
 */
vector<string> findHighAccessEmployees(vector<vector<string>>& access_times) {
    std::unordered_map<string, std::multiset<int>> m_employeeToTimestamp; // <Employee, TimeStamp>
    
    // Collect the data in the map
    for (const vector<string>& access : access_times) {
        string employeeName = access.at(0);
        int accessTime = std::stoi(access.at(1));
        m_employeeToTimestamp[employeeName].insert(accessTime);
    }

    // Process the map
    std::vector<string> results;
    for (std::pair<string, std::multiset<int>> employee : m_employeeToTimestamp) {
        if (evaluateFrequencyBreach(employee.second)) {
            results.push_back(employee.first);
        }
    }

    return results;
}

int main() {
    vector<vector<string>> logs = {
        {"A",  "1945"},
        {"A",  "1855"},
        {"C","1859"},
        {"B", "1940"},
        {"B", "1831"},
        {"C","1841"},
        {"B", "1918"},
        {"C","1941"},
        {"B", "1852"}
    };

    std::vector<string> results = findHighAccessEmployees(logs);
    for (auto res : results) {
        std::cout << res << std::endl;
    }

}