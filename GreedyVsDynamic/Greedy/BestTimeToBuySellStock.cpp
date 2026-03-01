#include <iostream>
#include <unordered_map>
#include <cmath>
using namespace std;

/**
 * 
 * https://leetcode.com/problems/best-time-to-buy-and-sell-stock/description/
 * 
 * Goal: We only want maximum profit by selling stock once. 
 * So, we track minimum price and simulate the profit by selling stock at every day.
 */
int maxProfitBySellingStockOnce(vector<int>& prices) {
    int minimumPrice = INT16_MAX;
    int maxProfit = 0;

    for (int index = 0; index < prices.size(); index++) {
        minimumPrice = std::min(minimumPrice, prices.at(index)); // Minimum price observed
        maxProfit = std::max(maxProfit, prices.at(index) - minimumPrice); // Sell stock everyday and calculate profit.
    }
    
    return maxProfit;
}

/**
 * https://leetcode.com/problems/best-time-to-buy-and-sell-stock-ii/description/
 * 
 * Goal: We want maximum total profit where we can buy/sell stocks multiple times but it has to be non-overlapping transaction.
 * 
 * 1. This is greedy because we always want the maximum profit in multiple transactions and that could only happen if 
 *    we sense the dip and sell it a day prior and then again rebuy the dip.
 * 2. If we don't sell before the dip, it would cost us some profit margin. Hence greedily selling it to gain max profit.
 * 
 * Approach:
 *   1. We will track the minimum price and as soon as the profit drops, we will sell a day prior.
 *   2. We will track a new minimum price and then repeat the process 
 */
int totalProfitBySellingStockMultipleTimes(vector<int>& prices) {
    int minimumPrice = INT16_MAX;
    int previousProfit = 0;
    int totalProfit = 0;

    for (int index = 0; index < prices.size(); index++) {
        minimumPrice = std::min(minimumPrice, prices.at(index)); // Minimum price observed
        int profit = prices.at(index) - minimumPrice; // Sell stock everyday and calculate profit.
        if (profit < previousProfit) {
            // If profit is < previousProfit, we did a mistake of not selling the stock earlier. 
            // So, let's sell the stock at index - 1 (i.e using previousProfit)
            totalProfit += previousProfit;

            // Since we sold the stock previous day, lets buy the dip and use it as a minimum price
            // and re-calculate profit
            minimumPrice = prices.at(index);
            profit = prices.at(index) - minimumPrice;
        }
        previousProfit = profit;
    }
    totalProfit += previousProfit;
    
    return totalProfit;
}

int main() {
    std::vector<int> stockPrices {6,1,3,2,4,7};
    std::cout << totalProfitBySellingStockMultipleTimes(stockPrices) << std::endl;
}

