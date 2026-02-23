#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
using namespace std;


bool canCompleteBFS(vector<vector<int>>& heights, int effort) {
    int totalRows = heights.size();
    int totalColumns = heights[0].size();
    std::queue<std::pair<int, int>> bfs_queue;
    vector<vector<bool>> visited_cell (totalRows, vector<bool>(totalColumns, false));
    bfs_queue.push({0, 0});
    visited_cell[0][0] = true;

    while (!bfs_queue.empty()) {
        std::pair<int, int> poppedElement = bfs_queue.front();
        bfs_queue.pop();

        //Mark the cell as visited
        int row = poppedElement.first;
        int column = poppedElement.second;
        if (row == totalRows - 1 && column == totalColumns - 1) {
            return true;
        }

        // Explore the neighbors
        // UP
        if (row - 1 >= 0 && !visited_cell[row-1][column] && std::abs(heights[row][column] - heights[row-1][column]) <= effort) {
            bfs_queue.push({row-1, column});
            visited_cell[row-1][column] = true;
        }

        // DOWN
        if (row + 1 < totalRows && !visited_cell[row+1][column] && std::abs(heights[row][column] - heights[row+1][column]) <= effort) {
            bfs_queue.push({row+1, column});
            visited_cell[row+1][column] = true;
        }

        // LEFT
        if (column - 1 >= 0 && !visited_cell[row][column-1] && std::abs(heights[row][column] - heights[row][column-1]) <= effort) {
            bfs_queue.push({row, column-1});
            visited_cell[row][column-1] = true;
        }

        // RIGHT
        if (column + 1 < totalColumns  && !visited_cell[row][column+1] && std::abs(heights[row][column] - heights[row][column+1]) <= effort) {
            bfs_queue.push({row, column+1});
            visited_cell[row][column+1] = true;
        }
    }

    // BFS completed but we couldn't reach the target cell
    return false;
}

/**
 * The max effort is the distance between source and any cell. Hence the minimum effort will be within (0, max)
 * Hence, we need binary search to find the right effort. For every effort, we will check if 
 * we can reach to destination using BFS only if the distance between current cell and neighbor cell is <= effort.
 */
int minimumEffortPathViaBinarySearch(vector<vector<int>>& heights) {
    int left = 0;
    int right = 0;
    for (int row = 0; row < heights.size(); row++) {
        for (int column = 0; column < heights[0].size(); column++) {
            right = std::max(right, std::abs(heights[0][0] - heights[row][column]));
        }
    }

    // Max Effort to reach row-1 and column -1 would be 7
    // Explore Binary Search
    int minimumEffort = right;
    while (left <= right) {
        int midEffort = (left+right)/2;
        if (canCompleteBFS(heights, midEffort)) {
            // If using midEffort, we are able to complete BFS, lets try for a shorter/minimum effort value
            right = midEffort - 1;
            minimumEffort = std::min(minimumEffort, midEffort);
        } else {
            // We were not able to complete BFS. Let's try a higher effort value
            left = midEffort + 1;
        }
    }

    return minimumEffort;
}

struct CellDistance {
    int row;
    int col;
    int effort;
};

struct Cmp {
    bool operator()(const CellDistance a, const CellDistance b) {
        return a.effort > b.effort; // Min-heap
    }
};

int minimumEffortPathViaDjikstra(vector<vector<int>>& heights) {
    int totalRows = heights.size();
    int totalColumns = heights[0].size();
    
    priority_queue<CellDistance, vector<CellDistance>, Cmp> minHeap;
    vector<vector<int>> minEffort(totalRows, vector<int>(totalColumns, INT_MAX));
    
    minHeap.push({0, 0, 0});  // row, col, effort
    minEffort[0][0] = 0;
    
    vector<pair<int,int>> dirs{{-1,0}, {1,0}, {0,-1}, {0,1}};
    
    while (!minHeap.empty()) {
        CellDistance cell = minHeap.top(); minHeap.pop();
        int row = cell.row, col = cell.col, effort = cell.effort;
        
        // CRITICAL: Skip if stale (better path found)
        if (effort > minEffort[row][col]) continue;
        
        for (auto& d : dirs) {
            int nr = row + d.first;
            int nc = col + d.second;
            if (nr >= 0 && nr < totalRows && nc >= 0 && nc < totalColumns) {
                int diff = abs(heights[row][col] - heights[nr][nc]);
                int newEffort = max(effort, diff);
                
                // FIXED: newEffort < current (not <=, not diff only)
                if (newEffort < minEffort[nr][nc]) {
                    minEffort[nr][nc] = newEffort;
                    minHeap.push({nr, nc, newEffort});
                }
            }
        }
    }
    
    return minEffort[totalRows-1][totalColumns-1];
}

int main() {
    vector<vector<int>> grid = {{1,2,2}, {3,8,2}, {5,3,5}};
    std::cout << minimumEffortPathViaDjikstra(grid) << std::endl;
}