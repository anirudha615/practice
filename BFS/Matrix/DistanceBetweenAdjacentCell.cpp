#include <iostream>
#include <unordered_map>
#include <cmath>
using namespace std;

/**
 * Instead of finding the distance from every 1 to nearest 0, try to find the distance from 0 to nearest 1.
 *     1. For simplicity, convert all 1s to -1s as 1 can also be a distance. We should replace it something which is not a correct distance value
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
    std::queue<std::tuple<int, int, int>> bfs_queue;

    for (int row = 0; row < rows; row++) {
        for (int column = 0; column < columns; column++) {
            if (matrix[row][column] == 0) {
                result[row][column] = 0;
                bfs_queue.push({row, column, 0});
            }
        }
    }

    while (!bfs_queue.empty()) {
        int row = std::get<0>(bfs_queue.front());
        int column = std::get<1>(bfs_queue.front());
        int distance = std::get<2>(bfs_queue.front());
        bfs_queue.pop();

        // Assess the neighbor,
        if (row - 1 >= 0 && result[row - 1][column] == -1) { // UP
            result[row - 1][column] = distance + 1;
            bfs_queue.push({row - 1, column, distance + 1});
        }

        if (row + 1 < result.size() && result[row + 1][column] == -1) { // DOWN
            result[row + 1][column] = distance + 1;
            bfs_queue.push({row + 1, column, distance + 1});
        }

        if (column - 1 >= 0 && result[row][column - 1] == -1) { // LEFT
            result[row][column - 1] = distance + 1;
            bfs_queue.push({row, column - 1, distance + 1});
        }

        if (column + 1 < result[0].size() && result[row][column + 1] == -1) { // RIGHT
            result[row][column + 1] = distance + 1;
            bfs_queue.push({row, column + 1, distance + 1});
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

