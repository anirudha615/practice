#include <iostream>
#include <unordered_map>
#include <cmath>
using namespace std;


int orangesRotting(vector<vector<int>>& grid) {
  std::queue<std::pair<int, int>> bfs_queue;

  // Add all the rotten oranges into the queue
  for (int row = 0; row < grid.size(); row++) {
    for (int column = 0; column < grid[0].size(); column++) {
      if (grid[row][column] == 2) {
        bfs_queue.push({row, column});
      }
    }
  }

  int timeToRot = -1;

  // Every time a level changes, timeToRot increases
  while (!bfs_queue.empty()) {
    timeToRot++;
    int levelSize = bfs_queue.size();
    for (int level = 0; level < levelSize; level++) {

      // Extract rotten tomatoes
      std::pair<int, int> cell = bfs_queue.front();
      bfs_queue.pop();
      int row = cell.first;
      int column = cell.second;

      // Assess the neighbor, rot them if the orange is 1 and push to the queue
      if (row - 1 >=0 && grid[row - 1][column] == 1) { // UP
        grid[row - 1][column] = 2;
        bfs_queue.push({row - 1, column});
      }

      if (row + 1 < grid.size() && grid[row + 1][column] == 1) { // DOWN
        grid[row + 1][column] = 2;
        bfs_queue.push({row + 1, column});
      }

      if (column - 1 >=0 && grid[row][column - 1] == 1) { // LEFT
        grid[row][column - 1] = 2;
        bfs_queue.push({row, column - 1});
      }

      if (column + 1 < grid[0].size() && grid[row][column + 1] == 1) { // RIGHT
        grid[row][column + 1] = 2;
        bfs_queue.push({row, column + 1});
      }
    }
  }

  // Assess of any fresh oranges, If found timeToRot = -1
  for (int row = 0; row < grid.size(); row++) {
    for (int column = 0; column < grid[0].size(); column++) {
      if (grid[row][column] == 1) {
        return - 1;
      }
    }
  }

  return timeToRot;  
} 

int main()
{
    vector<vector<int>> grid = {{2, 1, 1}, {1, 1, 1}, {0, 1, 2}};
    auto res = orangesRotting(grid);
    std::cout << res << std::endl;   
}

/**
 * You are given a 2D grid representing a simplified layout of people in a social network:

    0 → No person in this cell (empty)

    1 → A person who has not heard the rumor

    2 → A person who has already heard the rumor

Every minute, the rumor spreads from each person who knows it to their adjacent neighbors — up, down, left, and right.

Your goal is to determine the minimum number of minutes required for everyone to hear the rumor.

If it is impossible for some people to hear the rumor (i.e., they are isolated), return -1.
 */


 /**
  * 
# Output = 4
grid = [
  [2, 1, 1],
  [1, 1, 0],
  [0, 1, 1]
]

#output = -1
grid = [
  [2, 1, 1],
  [0, 0, 1],
  [1, 0, 1]
]

# Output = 0 (everyone already heard the rumor)
grid = [
  [0, 2],
  [2, 0]
]

#Output = 0
grid = [
  [0, 0],
  [0, 0]
]

#Output = 0
grid = [
  [2]
]

#Output = -1
grid = [
  [1]
]


# Output = 2
grid = [
  [0, 2],
  [1, 1]
]

# Output = -1
grid = [
  [0, 2],
  [1, 0]
]


  * 
  * 
  * 
  */
