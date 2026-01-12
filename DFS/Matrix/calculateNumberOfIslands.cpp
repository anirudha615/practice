#include <iostream>
#include <vector>
#include <unordered_set>

using namespace std;

void dfs_helper(vector<vector<char>>& grid, std::unordered_set<string>& visited_cell, int row, int column) {
    // Check if the row and column is within the matrix boundary
    if (!(row >= 0 && row < grid.size() && column >= 0 && column < grid[0].size())) {
        return;
    }

    // check whether the node is visited or not
    string key = std::to_string(row) + "," + std::to_string(column);
    if (visited_cell.count(key)) {
        return;
    }

    // Check whether the cell is water or island. If it is water (0), return else traverse DFS.
    if (grid[row][column] == '0') {
        return;
    }

    // Mark the cell as visited
    visited_cell.insert(key);

    // Traverse the matrix in directions
    dfs_helper(grid, visited_cell, row - 1, column); // UP
    dfs_helper(grid, visited_cell, row + 1, column); // DOWN
    dfs_helper(grid, visited_cell, row, column + 1); // LEFT
    dfs_helper(grid, visited_cell, row, column - 1); // RIGHT
}

/**
 * We need to find the islands which are surrounded by water.
 * Island definition is contiguous stream of 1s.
 * 
 * Therefore, islands in a grid is a 'DISCONNECTED' component.
 *    1. If there is just 1 island, all the 1s can be traversed recusrsively via DFS.
 *    2. If there are multiple islands, a single DFS traversal won't be enough. You need an outer loop also for disconnected islands.
 */
int number_of_islands(vector<vector<char>>& grid) {
    if (grid.empty()) {
        return 0;
    }
    std::unordered_set<string> visited_cell;
    int islands = 0;
    
    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid[0].size(); j++) {
            string key = std::to_string(i) + "," + std::to_string(j);
            if (!visited_cell.count(key) && grid[i][j] == '1') {
                dfs_helper(grid, visited_cell, i, j);
                islands++; // Once you complete a DFS traversal, that means there is an island consisting of 'atleast one 1'
            }
        }
    }
    return islands;
}

int main() {
    vector<vector<char>> grid = {{'1','1','0','1'}, {'1','1','0','1'}, {'1','1','0','0'}};
    std::cout << number_of_islands(grid) << std::endl;
}