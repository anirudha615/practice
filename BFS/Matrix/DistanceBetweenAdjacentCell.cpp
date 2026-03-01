#include <iostream>
#include <unordered_map>
#include <cmath>
using namespace std;

/**
 * Instead of finding the distance from every 1 to nearest 0, try to find the distance from 0 to nearest 1.
 *     1. For simplicity, convert all 1s to -1s
 *     2. Push all 0s to do BFS and as soon as find -1, increment the distance+1 and then add new updated rows,column to queue.
 *     3. Repeat to find new -1 and increment the distance+1 
 */
vector<vector<int>> updateMatrix(vector<vector<int>> matrix) {
    if (matrix.empty()) {
        return {};
    }

    int rows = matrix.size();
    int columns = matrix[0].size();
    std::vector<std::vector<int>> result (rows, std::vector<int>(columns, -1));
    std::queue<std::pair<int, int>> bfs_queue;

    for (int row = 0; row < rows; row++) {
        for (int column = 0; column < columns; column++) {
            if (matrix[row][column] == 0) {
                result[row][column] = 0;
                bfs_queue.push({row, column});
            }
        }
    }

    int levelNumber = 0;

    // Every time a level changes, distance between 0 and -1 increases
    while (!bfs_queue.empty()) {
        levelNumber++;
        int levelSize = bfs_queue.size();
        for (int level = 0; level < levelSize; level++) {

            // Extract 0s
            std::pair<int, int> cell = bfs_queue.front();
            bfs_queue.pop();
            int row = cell.first;
            int column = cell.second;

            // Assess the neighbor,
            if (row - 1 >= 0 && result[row - 1][column] == -1) { // UP
               result[row - 1][column] = levelNumber;
               bfs_queue.push({row - 1, column});
            }

            if (row + 1 < result.size() && result[row + 1][column] == -1) { // DOWN
                result[row + 1][column] = levelNumber;
                bfs_queue.push({row + 1, column});
            }

            if (column - 1 >= 0 && result[row][column - 1] == -1) { // LEFT
                result[row][column - 1] = levelNumber;
                bfs_queue.push({row, column - 1});
            }

            if (column + 1 < result[0].size() && result[row][column + 1] == -1) { // RIGHT
                result[row][column + 1] = levelNumber;
                bfs_queue.push({row, column + 1});
            }
        }
    }

    return result;
}

int main() {
    vector<vector<int>> matrix = {{0, 0, 0}, {0, 1, 0}, {1, 1, 1}};
    matrix = updateMatrix(matrix);
    for (const auto coordinate : matrix)
    {
        for (const auto ele : coordinate)
        {
            std::cout << ele << std::endl;   
        }
    }
}

