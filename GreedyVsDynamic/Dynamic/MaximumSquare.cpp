#include <iostream>
#include <unordered_map>
#include <cmath>
using namespace std;

/**
 * This looks tempting for a DFS but DFS won't help with geometrical shapes.
 * 
 * In this problem, every cell with value 1 is a square and every cell with value 0 is not a square.
 * So, we will store the number of squares each cell can store.
 * 
 * In order for a matrix to support 2*2 square, the up, left and diagonal also needs to be 1 (which means they are square themselves)
 * This gives birth to the logic that if (i, j) == 1, then DP[i, j] = min(DP[i-1,j], DP[i, j-1], DP[i-1, j-1]) + 1
 * else DP[i,j] = 0
 */
int maximalSquare(vector<vector<int>> matrix) {
    int maxArea = 0;
    std::vector<std::vector<int>> dp (matrix.size(), std::vector<int>(matrix[0].size(), 0));

    for (int row = 0; row < matrix.size(); row++) {
        for (int column = 0; column < matrix[row].size(); column++) {
            if (matrix[row][column]) {
                // If it is 1, just assign the 1 square every cell with value 1 can hold
                dp[row][column] = 1;
                // Then assess, if the up, left and diagonal are also 1 by asking their dp. If they are also 1, their DP will have value apart from 0.
                if (row-1>=0 && column-1>=0) {
                    dp[row][column] = std::min({dp[row-1][column], dp[row][column-1], dp[row-1][column-1]}) + 1; // 1 for each cell
                }
                maxArea = std::max(maxArea, dp[row][column] * dp[row][column]);
            }
        }
    }
    return maxArea;
}

int main() {
    vector<vector<int>> matrix = {
        {0, 0, 1, 0, 0},
        {1, 1, 1, 0, 1},
        {0, 1, 1, 0, 0}
    };
    std::cout << maximalSquare(matrix) << std::endl;
}

