#include <iostream>
#include <vector>
#include <unordered_set>

using namespace std;

void dfsHelper(vector<vector<int>>& grid, std::string& movement, int row, int column, std::string direction) {
    // Base Validation - row, column should be within matrix and cell should not be water
    if (row < 0 || row >= grid.size() || column < 0 || column >= grid[0].size() || !grid[row][column]) {
        return;
    }

    // Mark the cell as visited by turning island to water (1 to 0)
    grid[row][column] = 0;

    // Track the movement
    movement += direction;

    // Explore neighbors by passing the direction of ancestors
    dfsHelper(grid, movement, row-1, column, direction+"u"); // UP
    dfsHelper(grid, movement, row+1, column, direction+"d"); // DOWN
    dfsHelper(grid, movement, row, column-1, direction+"l"); // LEFT
    dfsHelper(grid, movement, row, column+1, direction+"r"); // RIGHT
}

/**
 * To identify identical islands, we need to traverse all island in the same DFS path.
 * To track the DFS movement, we need to pass the direction of parents (ancestors) along with the current direction.
 * 
 * For example - "s" (found 1 initially) + "sr" (found 1 on right) + "srd" (found 1 on right and then down) + "srr" (found 1 on right and then right)
 * So, complete movement of island with 4 1s is "s sr srd srr".
 * Hence, an identical island will also have the same movement (verify with unordered set)
 * 
 */
int numDistinctIslands(vector<vector<int>>& grid) {
    std::unordered_set<string> distinctIslands;

    for (int row = 0; row < grid.size(); row++) {
        for (int column = 0; column < grid[0].size(); column++) {
            // Create a string to track the DFS movement while finding the cells with value 1.
            // For every identical islands, the string to track movement will be same
            // as we traversing DFS in same direction
            if (grid[row][column] == 1) {
                std::string movement = "";
                dfsHelper(grid, movement, row, column, "s");
                if (!movement.empty() && !distinctIslands.count(movement)) {
                    distinctIslands.insert(movement);
                }
            }
        }
    }
    return distinctIslands.size();
}

int main() {
    vector<vector<int>> matrix = {
        {1,1,0},
        {0,1,1},
        {0,0,0},
        {1,1,1},
        {0,1,0}
    };
    std::cout << numDistinctIslands(matrix) << std::endl;
}