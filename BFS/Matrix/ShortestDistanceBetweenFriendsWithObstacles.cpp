#include <iostream>
using namespace std;

/**
 * https://leetcode.com/problems/shortest-distance-from-all-buildings/
 * 
 * Logic - 
 *    1. We need to run BFS from every friend's house and accumulate the steps/distance for every step. Avoid the obstacle.
 *    2. Lastly, we need to find the minimum cell value (distance_from_friend) where all the friends can reach (reachout_from_friend)
 * 
 * It is different from the one from 'No obstacles' - There you can run BFS with O(K * MN) but the simplest approach
 * is to find the median from all of friend's coordinates which will be shortest distance but not equidistant.
 * 
 * However, median won't work with obstacles as the median coordinate can be an obstacle.
 */
int shortestDistance(vector<vector<int>>& grid) {
    int totalRows = grid.size();
    int totalColumns = grid.at(0).size();
    std::vector<std::pair<int, int>> friendHouse;
    for (int row = 0; row < totalRows; row++) {
        for (int column = 0; column < totalColumns; column++) {
            if (grid[row][column] == 1) {
                friendHouse.push_back({row, column});
            }
        }
    }

    std::vector<std::vector<int>> distance_from_friend(totalRows, std::vector<int>(totalColumns, 0));
    std::vector<std::vector<int>> reachout_from_friend(totalRows, std::vector<int>(totalColumns, 0));

    for (std::pair<int, int> friend1 : friendHouse) {
        std::vector<std::vector<bool>> visited_cell(totalRows, std::vector<bool>(totalColumns, false));
        std::queue<std::tuple<int, int, int>> bfs_queue;
        bfs_queue.push({friend1.first, friend1.second, 0});

        while (!bfs_queue.empty()) {
            int row = std::get<0>(bfs_queue.front());
            int column = std::get<1>(bfs_queue.front());
            int distance = std::get<2>(bfs_queue.front());
            bfs_queue.pop();

            // Explore the neighbors
            // UP
            if (row-1>=0 && !visited_cell[row-1][column] && !grid[row-1][column]) {
                visited_cell[row-1][column] = true;
                distance_from_friend[row-1][column] += (distance+1);
                reachout_from_friend[row-1][column] += 1;
                bfs_queue.push({row-1, column, distance+1});
            }

            // DOWN
            if (row+1<totalRows && !visited_cell[row+1][column] && !grid[row+1][column]) {
                visited_cell[row+1][column] = true;
                distance_from_friend[row+1][column] += (distance+1);
                reachout_from_friend[row+1][column] += 1;
                bfs_queue.push({row+1, column, distance+1});
            }

            // LEFT
            if (column-1>=0 && !visited_cell[row][column-1] && !grid[row][column-1]) {
                visited_cell[row][column-1] = true;
                distance_from_friend[row][column-1] += (distance+1);
                reachout_from_friend[row][column-1] += 1;
                bfs_queue.push({row, column-1, distance+1});
            }

            // RIGHT
            if (column+1<totalColumns && !visited_cell[row][column+1] && !grid[row][column+1]) {
                visited_cell[row][column+1] = true;
                distance_from_friend[row][column+1] += (distance+1);
                reachout_from_friend[row][column+1] += 1;
                bfs_queue.push({row, column+1, distance+1});
            }
        }
    }

    int minimumDistance = INT_MAX;
    for (int row = 0; row < totalRows; row++) {
        for (int column = 0; column < totalColumns; column++) {
            if (distance_from_friend[row][column] && (reachout_from_friend[row][column] == friendHouse.size())) {
                minimumDistance = std::min(minimumDistance, distance_from_friend[row][column]);
            }
        }
    }

    return minimumDistance == INT_MAX ? -1 : minimumDistance;
}