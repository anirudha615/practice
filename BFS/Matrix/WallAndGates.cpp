#include <iostream>
#include <unordered_map>
#include <cmath>
using namespace std;

/**
 * Similar concept to Adjacent cells but with obstacles (Walls) - 
 *     1. Push all 0s (Gates) to do BFS and as soon as to come across INT32_MAX, increment the distance + 1
 *     2. Then push those to BFS_QUEUE and as soon as to come across INT32_MAX, increment again.
 *     3. Only insert row,column whose initial value is INT32_MAX and not 1 (which is obstacle/wall)
 * 
 * https://www.hellointerview.com/community/questions/walls-gates/cm5eh7nrh04uy838ome63sg1p?level=MID_LEVEL
 */
void wallsAndGates(vector<vector<int>>& rooms) {
    int totalRows = rooms.size();
    int totalColumns = rooms[0].size();
    std::queue<std::tuple<int, int, int>> bfs_queue;
    for (int row = 0; row < totalRows; row++) {
        for (int column = 0; column < totalColumns; column++) {
            if (!rooms[row][column]) {
                bfs_queue.push({row, column, 0});
            }
        }
    }

    while (!bfs_queue.empty()){
        int row = std::get<0>(bfs_queue.front());
        int column = std::get<1>(bfs_queue.front());
        int distance = std::get<2>(bfs_queue.front());
        bfs_queue.pop();

        // Explore neighbors and assign distance
        // UP
        if (row-1 >= 0 && rooms[row-1][column] == INT32_MAX) {
            rooms[row-1][column] = distance+1;
            bfs_queue.push({row-1, column, distance+1});
        }

        // DOWN
        if (row+1 < totalRows && rooms[row+1][column] == INT32_MAX) {
            rooms[row+1][column] = distance+1;
            bfs_queue.push({row+1, column, distance+1});
        }

        // LEFT
        if (column-1 >= 0 && rooms[row][column-1] == INT32_MAX) {
            rooms[row][column-1] = distance+1;
            bfs_queue.push({row, column-1, distance+1});   
        }

        // RIGHT
        if (column+1 < totalColumns && rooms[row][column+1] == INT32_MAX) {
            rooms[row][column+1] = distance+1;
            bfs_queue.push({row, column+1, distance+1});
        }
    }
}

int main() {
    vector<vector<int>> grid = {
        {INT32_MAX, -1,  0,   INT32_MAX},
        {INT32_MAX, INT32_MAX, INT32_MAX, -1 },
        {INT32_MAX, -1,  INT32_MAX, -1 },
        {0,   -1,  INT32_MAX, INT32_MAX}
    };
    wallsAndGates(grid);
    for (const auto coordinate : grid) {
        for (const auto ele : coordinate) {
            std::cout << ele << ", ";   
        }
        std::cout << std::endl;
    }
}

