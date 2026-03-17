#include <iostream>
#include <unordered_set>
using namespace std;

class Solution {
private:
    vector<string> directions {"N", "E", "S", "W"};
    int getEuclidianDistance(int x, int y) {
        return x*x + y*y;
    }

    int moveClockwise(int currentIndex) {
        return (currentIndex + 1) % directions.size(); // moving forward in a circular array 
    }

    int moveAntiClockwise(int currentIndex) {
        return (currentIndex - 1 + directions.size()) % directions.size(); // moving backward in a circular array 
    }

    string encodeObstacle(int x, int y) {
        return to_string(x) + "," + to_string(y);
    }


public:
    /**
     * Use circular array concept to move clockwise and anticlockwise.
     * 
     * For every command, increase 1 step at a time and check if there's an obstacle. If yes, don't update X and Y.
     * https://www.hellointerview.com/community/questions/walking-robot-simulation/cm5eguhac02oo838oxjb7sxoi?level=SENIOR
     */
    int robotSim(vector<int>& commands, vector<vector<int>>& obstacles) {
        int directionIndex = 0;
        int x = 0; int y = 0;
        int maxDistance = 0;
        unordered_set<string> obstacleSet;
        // Encode the coordinates
        for (auto& obs : obstacles) {
            obstacleSet.insert(encodeObstacle(obs[0], obs[1]));
        }
        for (int command : commands) {
            
            // React to commands
            if (command == -1) {
                directionIndex = moveClockwise(directionIndex);
                continue;
            } 
            
            if (command == -2) {
                directionIndex = moveAntiClockwise(directionIndex);
                continue;
            }
            
            // Move 1 step at a time and validate if there is an obstacle. If yes, don't update X and Y.
            for (int step = 0; step < command; step++) {
                int newX = x; int newY = y;
                if (directions.at(directionIndex) == "N") {
                    newY++;
                } else if (directions.at(directionIndex) == "E") {
                    newX++;
                } else if (directions.at(directionIndex) == "S") {
                    newY--;
                } else {
                    newX--;
                }

                if (obstacleSet.count(encodeObstacle(newX, newY))) {
                    break;
                }

                x = newX;
                y = newY;

                // Calculate maxDistance 
                maxDistance = std::max(maxDistance, getEuclidianDistance(x, y));
            }
        }

        return maxDistance;
    }
};

int main() {
    Solution s;
    vector<int> commands {7,-2,-2,7,5};
    vector<vector<int>> obstacles = {
        {-3,  2},
        {-2,  1},
        { 0,  1},
        {-2,  4},
        {-1,  0},
        {-2, -3},
        { 0, -3},
        { 4,  4},
        {-3,  3},
        { 2,  2}
    };

    std::cout << s.robotSim(commands, obstacles) << std::endl;
}