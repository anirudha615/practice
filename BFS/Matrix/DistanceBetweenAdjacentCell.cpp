#include <iostream>
#include <unordered_map>
#include <cmath>
using namespace std;

/**
 * Approach - 
 *      a. Mark the result matrix as -1 except the 0s.
 *      b. Enqueue all 0s from input matrix into the queue.
 *      c. At Level 1, all the -1s which are neighbors of 0s in the queue, update the distance to 1 and add them to the queue.
 *      d. At Level 2, all the -1s which are neighbors of 1s in the queue, update the distance to 2 and add them to the queue.
 *      e. And So on.
 * 
 * 
 * Mistake I made - I enqueue a single 1 and perform BFS on that. If there are m*n 1s, worst case would be m^2 * n^2 BFS calls.
 * This is because I need to remember the distance between that single 1 and any 0.
 * 
 * With the above approach, I don't have to remember the distance between 0 and specific 1. As soon as I reach any -1,
 * I will update the distance with the level where that -1 is being found.
 * 
 * In summary, the approach helps to update the distance as soon as level traversal is over. With my approach,
 * I had to collect the distance with level traversal and then backtrack from where it started.
 * 
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

