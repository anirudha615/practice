#include <iostream>
#include <vector>
#include <unordered_set>
#include <optional>

using namespace std;

void dfs_helper(vector<vector<char>>& grid, int row, int column) {
    // Check if the row and column is within the matrix boundary
    if (!(row >= 0 && row < grid.size() && column >= 0 && column < grid[0].size())) {
        return;
    }

    // Check whether the cell is O, X or S. If it is X or S, return else traverse DFS.
    if (grid[row][column] == 'X' || grid[row][column] == 'S') {
        return;
    }

    // Modify O to S
    grid[row][column] = 'S';

    // Traverse the matrix in directions
    dfs_helper(grid, row - 1, column); // UP
    dfs_helper(grid, row + 1, column); // DOWN
    dfs_helper(grid, row, column + 1); // RIGHT
    dfs_helper(grid, row, column - 1); // LEFT
}

/**
* We need to find 'O's which are surrounded by 'X' and modify it.
* However, we should not modify 'O's which are reachable from grid edge or which are present at the edge.
* 
* This is also a problem of disconnected components - 
*   a. We should start a DFS traversal from O which is at the edge and see which Os are reachable and mark them S.
*   b. Once that is done, whatever Os are present can be blindly modified  by X
* 
* Below is the mistake I did - 
*    a. I tried to perform DFS traversal on Os which are not starting at the edge and as a result, eventually I reached an edge which has O
*       but failed to propagate to its child and as a result, there are edge cases which I missed.
*    b. Hence, the solution is to perform DFS from the boundary and mark them S. Whichever Os are left, mark them X.
*  
*/ 
vector<vector<char>> surrounded_regions(vector<vector<char>>& grid) {
    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid[0].size(); j++) {
            string key = std::to_string(i) + "," + std::to_string(j);
            // Start DFS from the edges of the matrix
            if (grid[i][j] == 'O' && (i == 0 || i == grid.size() - 1 || j == 0 || j == grid[0].size() - 1)) {
                dfs_helper(grid, i, j);
                //Once you complete a DFS traversal, that means there is a group of 'O's and that group size is at least 1
                // and we have marked them as S.
            }
        }
    }

    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid[0].size(); j++) {
            if (grid[i][j] == 'O') {
                grid[i][j] = 'X';
            } else if (grid[i][j] == 'S') {
                grid[i][j] = 'O';
            }
        }
    }

    return grid;
}

int main() {
    vector<vector<char>> grid = {
        {'X','X','X','X'},
        {'X','O','O','X'},
        {'X','X','O','X'},
        {'X','O','X','X'}
    };
    surrounded_regions(grid);
}