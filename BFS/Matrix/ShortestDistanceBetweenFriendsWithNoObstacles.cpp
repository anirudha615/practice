#include <iostream>
using namespace std;

/**
 * https://leetcode.com/problems/best-meeting-point/
 * 
 * Logic - 
 *   1. Shortest Distance between any 2 friends in a straight line is the median coordinate of their distances. 
 *      It is not equal distance but shortest distance.
 *   2. Now if the coordinates are in 2D, we can still apply the same concept to find short distance by collecting all the 
 *      row and column coordinates of all friends and then find the median. 
 *   3. Median is the meeting point and hence we will find the distance between all friends to the meeting point
 * 
 * Another Approach - 
 *    1. Collect all K friends. 
 *    2. Run BFS for every K friend and accumulate the distance.
 *    3. Lastly, traverse the grid and find the cell with minimum distance. (Look with Obstacles problem)
 */
int minTotalDistance(vector<vector<int>>& grid) {
    std::vector<int> rowCoordinateList;
    std::vector<int> columnCoordinateList;
    int totalRows = grid.size();
    int totalColumns = grid.at(0).size();
    for (int row = 0; row < totalRows; row++) {
        for (int column = 0; column < totalColumns; column++) {
            if (grid[row][column]) {
                rowCoordinateList.push_back(row);
                columnCoordinateList.push_back(column);
            }
        }
    }

    std::sort(rowCoordinateList.begin(), rowCoordinateList.end());
    std::sort(columnCoordinateList.begin(), columnCoordinateList.end());

    int totalFriends = rowCoordinateList.size();
    int medianIndex = -1;
    if (totalFriends % 2) {
        medianIndex = totalFriends/2;
    } else {
        medianIndex = (totalFriends-1)/2;
    }

    int meetingPointRow = rowCoordinateList.at(medianIndex);
    int meetingPointColumn = columnCoordinateList.at(medianIndex);
    int totalDistance = 0;
    for (int index = 0; index<rowCoordinateList.size(); index++) {
        totalDistance += std::abs(rowCoordinateList.at(index) - meetingPointRow) + std::abs(columnCoordinateList.at(index) - meetingPointColumn);
    }

    return totalDistance;
}