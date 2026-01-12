#include <iostream>
#include <unordered_map>
#include <cmath>
using namespace std;

/**
 * I made the mistake of seeing this problem at an angle of monotonically increasing stack
 * 
 * MISTAKE APPROACH
 * 1. The approach is to push the prices >= top of the current element of the stack and wait.
 * 2. Once you encounter a price lower than the top of the current element, pop out the elements until
 *    the top of the element is < current price.
 * 3. Since all the elements are monotonically increasing, profit will always be top - last item Popped.
 * 4. Push the current price and repeat from Step 1.
 * 
 * 1. The problem with the monotically increasing approach is that we make smaller transaction - 
 * 2. So, if we see a stock price lesser than top of the element of the stack, we start calculating profit
 *    but stops if the current stock price is greater than other elements in the stack, which is wrong.
 * 3. You need to empty the entire stack to calculate the profit between monotically increasing stack.
 * 4. If the requirement is to find the next tentative drop in stock price so that you could sell, then we could use the concept.
 * 
 * 
 * CORRECTED APPROACH - 
 *   1. Keep a track of minimum price observed
 *   2. Simulate the profit by selling the stock at every index 
 */
int maxProfit(vector<int>& prices) {
    int minimumPrice = INT16_MAX;
    int maxProfit = 0;

    for (int index = 0; index < prices.size(); index++) {
        minimumPrice = std::min(minimumPrice, prices.at(index)); // Minimum price observed
        maxProfit = std::max(maxProfit, prices.at(index) - minimumPrice); // Sell stock everyday and calculate profit.
    }
    
    return maxProfit;
}

int main() {
    std::vector<int> stockPrices {7,1,5,3,6,4};
    std::cout << maxProfit(stockPrices) << std::endl;
}

