#include <iostream>
#include <unordered_map>
#include <cmath>
using namespace std;

/**
 * Find max money you can rob without alerting police.
 * You will only alert police if you rob 2 houses adjacent to each other
 */
int rob(vector<int>& houses) {
    std::vector<int> dp (houses.size(), 0);
    // Get the base case
    if (houses.empty()) {
        return 0;
    } else if (houses.size() == 1) {
        return houses.at(0);
    } else if (houses.size() == 2) {
        return std::max(houses.at(0), houses.at(1));
    }
    dp[0] = houses.at(0);// Robbing the 0th house
    dp[1] = std::max(houses.at(0), houses.at(1)); // Either rob the 1st house or be content with whatever you have from the 0th house

    for (int house = 2; house < houses.size(); house++) {
        // Compare the money robbed from previous house (house - 1) and money robbed from current and previous to previous house (house - 2)
        // Whichever is the max, will become the money for the current house. 
        // Here, you made the decision of either robbing the current house or skipping the current house as money robbed from here will be not much with a risk of alerting the police
        int moneyForCurrentHouse = std::max(dp[house - 1], dp[house - 2] + houses.at(house)); 
        dp[house] = moneyForCurrentHouse;
    }

    return dp[houses.size() - 1];  
}

int main() {
    vector<int> houses {1,2,3,1};
    std::cout << rob(houses) << std::endl;
}

