#include <iostream>
using namespace std;

int shortestPathBinaryMatrix(vector<vector<int>>& grid) {
    int totalRows = grid.size();
    int totalColumns = grid.at(0).size();
    std::queue<std::tuple<int, int, int>> bfs;
    if (!grid[0][0]) {
        bfs.push({0, 0, 1});
        grid[0][0] = 1; // Mark as visited
    }
    
    while (!bfs.empty()) {
        int row = std::get<0>(bfs.front());
        int column = std::get<1>(bfs.front());
        int distance = std::get<2>(bfs.front());
        bfs.pop();

        if (row == totalRows-1 && column == totalColumns-1) {
            return distance;
        }

        // Explore the neighbors
        if (row-1>=0 && !grid[row-1][column]) {
            grid[row-1][column] = 1;
            bfs.push({row-1, column, distance + 1});
        }
        if (row+1 < totalRows && !grid[row+1][column]) {
            grid[row+1][column] = 1;
            bfs.push({row+1, column, distance + 1});
        }
        if (column-1>=0 && !grid[row][column-1]) {
            grid[row][column-1] = 1;
            bfs.push({row, column-1, distance + 1});
        }
        if (column+1 < totalColumns && !grid[row][column+1]) {
            grid[row][column+1] = 1;
            bfs.push({row, column+1, distance + 1});
        }
        if (row-1>=0 && column-1>=0 && !grid[row-1][column-1]) {
            grid[row-1][column-1] = 1;
            bfs.push({row-1, column-1, distance + 1});
        }
        if (row-1>=0 && column+1<totalColumns && !grid[row-1][column+1]) {
            grid[row-1][column+1] = 1;
            bfs.push({row-1, column+1, distance + 1});
        }
        if (row+1<totalRows && column+1<totalColumns && !grid[row+1][column+1]) {
            grid[row+1][column+1] = 1;
            bfs.push({row+1, column+1, distance + 1});
        }
        if (row+1<totalRows && column-1>=0 && !grid[row+1][column-1]) {
            grid[row+1][column-1] = 1;
            bfs.push({row+1, column-1, distance + 1});
        }
    }

    return -1;
}

int main() {
    vector<vector<int>> grid = {
        {0, 0, 0},
        {1, 1, 0},
        {1, 1, 0}
    };
    std::cout << shortestPathBinaryMatrix(grid) << std::endl;
}