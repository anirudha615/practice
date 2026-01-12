#include <iostream>
#include <vector>
#include <unordered_set>
#include <optional>

using namespace std;

void dfs_helper(
    vector<vector<int>>& grid, 
    std::unordered_set<string>& visited_cell, 
    std::unordered_set<string>& cellsReachableToOcean, 
    int row, int column, int parent) {
    // Check if the row and column is within the matrix boundary
    if (!(row >= 0 && row < grid.size() && column >= 0 && column < grid[0].size())) {
        return;
    }

    // check whether the cell is visited or not.
    string key = std::to_string(row) + "," + std::to_string(column);
    if (visited_cell.count(key)) {
        return;
    }

    // Mark the cell as visited 
    visited_cell.insert(key);

    // Add the cell to the reachable list only if the current cell is greater or equal to parent
    if (grid[row][column] >= parent) {
        cellsReachableToOcean.insert(key);

        // Traverse the matrix in directions only if the current cell is >= parent
        dfs_helper(grid, visited_cell, cellsReachableToOcean, row - 1, column, grid[row][column]); // UP
        dfs_helper(grid, visited_cell, cellsReachableToOcean, row + 1, column, grid[row][column]); // DOWN
        dfs_helper(grid, visited_cell, cellsReachableToOcean, row, column + 1, grid[row][column]); // RIGHT
        dfs_helper(grid, visited_cell, cellsReachableToOcean, row, column - 1, grid[row][column]); // LEFT
    }
}

/**
 * Brute Force method -
 *   1. Start DFS traversal from Pacific boundaries and see if there exists a connected cell in Atlantic boundaries.
 *   2. Here you have to trigger DFS traversal from every boundary. A cell at worse can be traversed at every DFS traversal.
 * 
 * Optimization - 
 *    1. Start DFS traversal from Pacific boundaries and obtain the coordinates which can flow to that specific boundary - 
 *       Collect all reachable cells which can flow to that particular cell where we started DFS traversal.
 *    2. Similarly for Atlantic boundaries to know which cell can flow into the cell where we started DFS.
 *    3. Intersection of those sets would provide us the list of cells which can flow into each other.
 */
vector<vector<int>> pacific_atlantic_flow(vector<vector<int>>& heights) {
    std::unordered_set<string> cellsReachableToPacific;
    std::unordered_set<string> cellsReachableToAtlantic;
    for (int i = 0; i < heights.size(); i++) {
        for (int j = 0; j < heights[0].size(); j++) {
            if (i == 0 || j == 0) {
                // Have new visited list for every DFS traversal
                std::unordered_set<string> visited_cell;
                dfs_helper(heights, visited_cell, cellsReachableToPacific, i, j, -1);
            } 
            if (i == heights.size() - 1 || j == heights[0].size() - 1) {
                std::unordered_set<string> visited_cell;
                dfs_helper(heights, visited_cell, cellsReachableToAtlantic, i, j, -1);
            }
        }
    }

    // Find intersection and convert back to coordinates
    vector<vector<int>> result;
    for (const string& key : cellsReachableToPacific) {
        if (cellsReachableToAtlantic.count(key)) {
            // Parse "row,col" back to coordinates
            size_t commaPos = key.find(',');
            int row = stoi(key.substr(0, commaPos));
            int col = stoi(key.substr(commaPos + 1));
            result.push_back({row, col});
        }
    }
    
    return result;
}

int main() {
    vector<vector<int>> heights = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };
    pacific_atlantic_flow(heights);
}