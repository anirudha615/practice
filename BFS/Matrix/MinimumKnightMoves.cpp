#include <iostream>
#include <algorithm>
#include <queue>
#include <unordered_map>

/**
 * This is a problem of shortest path with rules - hence apply BFS.
 * 
 * Similar to move in Matrix via up, down, left and right, in this problem we will move -  
 *      a. LEFT, LEFT and (UP or DOWN) --> (-1, -2) and (1, -2)
 *      b. RIGHT, RIGHT and (UP or DOWN) --> (-1, 2) and (1, 2)
 *      c. UP, UP and (LEFT or RIGHT) --> (-2, -1) and (-2, 1)
 *      d. DOWN, DOWN and (LEFT or RIGHT) --> (2, -1) and (2, 1)
 */
int minimumKnightMoves(int x, int y) {
    int levels = 0;
    std::queue<std::pair<int, int>> bfs_queue;
    bfs_queue.push({0, 0});
    std::unordered_map<std::string, int> visited;

    while (!bfs_queue.empty()) {
        int levelSize = bfs_queue.size();
        for (int level = 0; level < levelSize; level++) {

            // Extract the node
            std::pair<int, int> cell = bfs_queue.front();
            bfs_queue.pop();
            int row = cell.first;
            int column =  cell.second;

            // Mark them as visited
            std::string key = std::to_string(row) + "," + std::to_string(column);
            visited[key] = 1;
            
            if (row == x and column == y) {
                return levels;
            }


            // We need to add 8 possible neighbors to the queue
            if (!visited.count(std::to_string(row - 1) + "," + std::to_string(column - 2))) { // LEFT, LEFT and UP
                bfs_queue.push({row - 1, column - 2});
            }
            if (!visited.count(std::to_string(row + 1) + "," + std::to_string(column - 2))) { // LEFT, LEFT and DOWN
                bfs_queue.push({row + 1, column - 2});
            }
            if (!visited.count(std::to_string(row - 1) + "," + std::to_string(column + 2))) { // RIGHT, RIGHT and UP
                bfs_queue.push({row - 1, column + 2});
            }
            if (!visited.count(std::to_string(row + 1) + "," + std::to_string(column + 2))) { // RIGHT, RIGHT and DOWN
                bfs_queue.push({row + 1, column + 2});
            }
            if (!visited.count(std::to_string(row - 2) + "," + std::to_string(column - 1))) { // UP, UP and LEFT
                bfs_queue.push({row - 2, column - 1});
            }
            if (!visited.count(std::to_string(row - 2) + "," + std::to_string(column + 1))) { // UP, UP and RIGHT
                bfs_queue.push({row - 1, column + 1});
            }
            if (!visited.count(std::to_string(row + 2) + "," + std::to_string(column - 1))) { // DOWN, DOWN and LEFT
                bfs_queue.push({row + 2, column - 1});
            }
            if (!visited.count(std::to_string(row + 2) + "," + std::to_string(column + 1))) { // DOWN, DOWN and RIGHT
                bfs_queue.push({row + 2, column + 1});
            }
        }
        levels++;
    }
    return 0;
}

int main() {
    std::cout << minimumKnightMoves(4, 4) << std::endl;
}
