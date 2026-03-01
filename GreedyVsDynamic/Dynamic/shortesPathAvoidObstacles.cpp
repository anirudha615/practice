#include <iostream>
#include <unordered_map>
#include <cmath>
using namespace std;

struct Element {
    int row;
    int column;
    int kRemains;
    int steps;
};

/**
 * This is a DP problem because at the current step to make a decision, I have to depend upon previous step's decision.
 * 
 * Puzzle logic - 
 *    1. Since this is a problem of distance, we will use BFS.
 *    2. Unlike, normal BFS where we use the visited_cell across all cells. We will use different instances of visited_cell
 *       pass on to their neighbors based on the current cell.
 *    
 *    3. So, here visited cell would depend on how many obstacles are allowed to be destroyed.
 *       For ex - A Cell with DOWN neighbor is an obstacle whereas RIGHT neighbor is not an obstacle.
 *       So, the current cell will pass visited_cell as [row+1][column][k-1] to DOWN 
 *       but it will pass visited_cell as [row-1][column][k] to RIGHT.
 */
int shortestPath(vector<vector<int>>& grid, int k) {
    if (grid.empty()) {
        return -1;
    }
    std::queue<Element> bfs_queue;
    std::vector<std::vector<std::vector<bool>>> visited_cell (grid.size(), 
        std::vector<std::vector<bool>>(grid[0].size(), std::vector<bool>(k+1, false)));
    bfs_queue.push({0, 0, k, 0});
    visited_cell[0][0][k] = true;

    while (!bfs_queue.empty()) {
        Element cellToBeProcessed = bfs_queue.front();
        bfs_queue.pop();

        int row = cellToBeProcessed.row;
        int column = cellToBeProcessed.column;
        int kRemains = cellToBeProcessed.kRemains;
        int steps = cellToBeProcessed.steps;

        if (row == grid.size() - 1 && column == grid[0].size() - 1) {
            return steps;
        }

        // Explore the neighbors
        if (row-1 >= 0) { // UP
            // If we come across a neighbor which is an obstacle and we have the opportunity
            // to destroy it (kRemains > 0), we will destroy it and push that cell to the queue 
            // mark it as visisted
            int kRemainsCopy = kRemains;
            if (grid[row-1][column] == 1 && kRemains > 0) {
                --kRemainsCopy;
                if (!visited_cell[row-1][column][kRemainsCopy]) {
                    visited_cell[row-1][column][kRemainsCopy] = true;
                    bfs_queue.push({row-1, column, kRemainsCopy, steps+1});
                }
            } else if (grid[row-1][column] == 0 && !visited_cell[row-1][column][kRemainsCopy]) {
                // If we come across a neighbor which is not an obstacle
                visited_cell[row-1][column][kRemainsCopy] = true;
                bfs_queue.push({row-1, column, kRemainsCopy, steps+1});
            }
        }  
        
        if (row+1 < grid.size()) { // DOWN 
            int kRemainsCopy = kRemains;
            if (grid[row+1][column] == 1 && kRemainsCopy > 0) {
                --kRemainsCopy;
                if (!visited_cell[row+1][column][kRemainsCopy]) {
                    visited_cell[row+1][column][kRemainsCopy] = true;
                    bfs_queue.push({row+1, column, kRemainsCopy, steps+1});
                }
            } else if (grid[row+1][column] == 0 && !visited_cell[row+1][column][kRemainsCopy]) {
                visited_cell[row+1][column][kRemainsCopy] = true;
                bfs_queue.push({row+1, column, kRemainsCopy, steps+1});
            }
        }  
        
        if (column-1 >= 0) { // LEFT 
            int kRemainsCopy = kRemains;
            if (grid[row][column-1] == 1 && kRemainsCopy > 0) {
                --kRemainsCopy;
                if (!visited_cell[row][column-1][kRemainsCopy]) {
                    visited_cell[row][column-1][kRemainsCopy] = true;
                    bfs_queue.push({row, column-1, kRemainsCopy, steps+1});
                }
            } else if (grid[row][column-1] == 0 && !visited_cell[row][column-1][kRemainsCopy]) {
                visited_cell[row][column-1][kRemainsCopy] = true;
                bfs_queue.push({row, column-1, kRemainsCopy, steps+1});
            }
        }  
        
        if (column+1 < grid[0].size()) { // RIGHT 
            int kRemainsCopy = kRemains;
            if (grid[row][column+1] == 1 && kRemainsCopy > 0) {
                --kRemainsCopy;
                if (!visited_cell[row][column+1][kRemainsCopy]) {
                    visited_cell[row][column+1][kRemainsCopy] = true;
                    bfs_queue.push({row, column+1, kRemainsCopy, steps+1});
                }
            } else if (grid[row][column+1] == 0 && !visited_cell[row][column+1][kRemainsCopy]) {
                visited_cell[row][column+1][kRemainsCopy] = true;
                bfs_queue.push({row, column+1, kRemainsCopy, steps+1});
            }
        } 
    }

    return -1;
}