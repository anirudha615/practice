#include <iostream>
#include <vector>
#include <unordered_set>

using namespace std;

void dfsHelper(vector<vector<int>>& grid, int& islandArea, int row, int column) {
    // Base Validation - row, column should be within matrix and cell should not be water
    if (row < 0 || row >= grid.size() || column < 0 || column >= grid[0].size() || !grid[row][column]) {
        return;
    }

    // Mark the cell as visited by turning island to water (1 to 0)
    grid[row][column] = 0;

    // Increment islandArea by 1
    ++islandArea;

    // Explore neighbors
    dfsHelper(grid, islandArea, row-1, column); // UP
    dfsHelper(grid, islandArea, row+1, column); // DOWN
    dfsHelper(grid, islandArea, row, column-1); // LEFT
    dfsHelper(grid, islandArea, row, column+1); // RIGHT
}

/**
 * This problem is exactly similar to number of islands.
 * The only difference is along with counting the number of islands, we need to count the area of islands - 
 *      which is the number of 1s in the island;
 * Once the area of an island is calcualted, we need to maintain the max area
 */
int maxAreaOfIsland(vector<vector<int>>& grid) {
    int totalRows = grid.size();
    int totalColumns = grid[0].size();
    int maxArea = 0;

    for (int row = 0; row < totalRows; row++) {
        for (int column = 0; column < totalColumns; column++) {
            if (grid[row][column] == 1) {
                int islandArea = 0;
                dfsHelper(grid, islandArea, row, column);
                maxArea = std::max(islandArea, maxArea);
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
    std::cout << maxAreaOfIsland(matrix) << std::endl;
}