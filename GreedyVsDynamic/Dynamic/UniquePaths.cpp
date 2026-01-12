#include <iostream>
#include <unordered_map>
#include <cmath>
using namespace std;

/**
 * Think of base cases such as if the starting cell is (0, 0), how can I reach(0, 1) and (1, 0)
 * 1. You can reach 0, 1 via left (1 way) and 1, 0 via up (1 way).
 * 2.  To reach (1,1), you can come via (0,1) and (1,0). Both of the base cases have only 1 way. Then to reach (1,1), you will have 2 ways.
 * 
 * Hence, (0,1) and (1,1) are your base cases with value as 1.
 * 
 * The logic stands as DP[i][j] = DP[i][j-1] (Coming from left) + DP[i-1][j] (Coming from UP)
 */
int uniquePaths(int row, int column) {
    std::vector<std::vector<int>> dp (row, std::vector<int>(column, -1));

    // Get the base cases
    dp[0][0] = 1;
    if (1 < column) {
        dp[0][1] = 1; // right from (0, 0)
    }
    if (1 < row) {
        dp[1][0] = 1; // down from (0,1)
    }

    for (int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) {
            if (dp[i][j] == -1) {
                dp[i][j] = ((j-1 >=0) ? dp[i][j-1] : 0) + (i-1>=0 ? dp[i-1][j] : 0);
            }
        }
    }

    return dp[row - 1][column - 1];
}


int main() {
    std::cout << uniquePaths(1,1) << std::endl;
}

