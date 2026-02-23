#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
using namespace std;

bool canCompleteBFS(vector<vector<int>>& grid, int thresholdValue) {
    int totalRows = grid.size();
    int totalColumns = grid[0].size();
    
    std::queue<std::pair<int, int>> bfs_queue;
    vector<vector<bool>> visited_cell (totalRows, vector<bool>(totalColumns, false));
    
    // Base source node
    bfs_queue.push({0, 0});
    visited_cell[0][0] = true;

    while (!bfs_queue.empty()) {
        std::pair<int, int> poppedElement = bfs_queue.front();
        bfs_queue.pop();

        int row = poppedElement.first;
        int column = poppedElement.second;
        if (row == totalRows - 1 && column == totalColumns - 1) {
            return true;
        }

        // Explore the neighbors and only push the if their cell value >= threshold value
        // UP
        if (row - 1 >= 0 && !visited_cell[row-1][column] && grid[row-1][column] >= thresholdValue) {
            bfs_queue.push({row-1, column});
            visited_cell[row-1][column] = true;
        }

        // DOWN
        if (row + 1 < totalRows && !visited_cell[row+1][column] && grid[row+1][column] >= thresholdValue) {
            bfs_queue.push({row+1, column});
            visited_cell[row+1][column] = true;
        }

        // LEFT
        if (column - 1 >= 0 && !visited_cell[row][column-1] && grid[row][column-1] >= thresholdValue) {
            bfs_queue.push({row, column-1});
            visited_cell[row][column-1] = true;
        }

        // RIGHT
        if (column + 1 < totalColumns  && !visited_cell[row][column+1] && grid[row][column+1] >= thresholdValue) {
            bfs_queue.push({row, column+1});
            visited_cell[row][column+1] = true;
        }
    }

    // BFS completed but we couldn't reach the target cell
    return false;
}

/**
 * Question: https://leetcode.com/problems/path-with-maximum-minimum-value/description/
 * 
 * The questions asks to find the path with the maximum score and in that path find the minimum value.
 * 
 * Puzzle - 
 *   1. We will check if there exists any path if I only process the cell with value >= threshold value.
 *   2. If we are able to complete BFS using threshold, we will need to go higher to find the maximum minimum value.
 * 
 * the goal is to just output the maximum minimum value and not provide the path with maximum score.
 */
int maximumMinimumPath(vector<vector<int>>& grid) {
    int left = 0;
    int right = 0;
    for (int row = 0; row < grid.size(); row++) {
        for (int column = 0; column < grid[0].size(); column++) {
            right = std::max(right, grid[row][column]);
        }
    }

    // Max Value in the grid is the highest cell value.
    // Explore Binary Search
    int maximumValue = left;
    while (left <= right) {
        int valueToExplore = (left+right)/2;
        if (canCompleteBFS(grid, valueToExplore)) {
            // If using valueToExplore, we are able to complete BFS, lets try for a bigger value
            left = valueToExplore + 1;
            maximumValue = std::max(maximumValue, valueToExplore);
        } else {
            // We were not able to complete BFS. Let's try a smaller value
            right = valueToExplore - 1;
        }
    }

    return maximumValue;
}

int main() {
    vector<vector<int>> grid = {{5,4,5}, {1,2,6}, {7,4,6}};
    std::cout << maximumMinimumPath(grid) << std::endl;
}