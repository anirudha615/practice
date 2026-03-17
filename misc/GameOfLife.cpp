#include <iostream>
using namespace std;

/**
 * https://www.hellointerview.com/community/questions/leetcode-289-game-life/cm5eh7nrh04v1838o88t7v4wr?level=STAFF
 * 
 * 1. Collect Alive Neighbors
 * 2. If 1 rule matches, apply them.
 * 
 * NOTE: You should not change the board as soon as the rule matches as other cells needs to read
 * the original value of the board.
 * 
 * So, remember to apply them once the first pass of the matrix is over.
 */
void gameOfLife(vector<vector<int>>& board) {
    int totalRows = board.size();
    int totalColumns = board.at(0).size();
    vector<vector<int>> copy = board;
    for (int row = 0; row < totalRows; row++) {
        for (int column = 0; column < totalColumns; column++) {
            bool isCurrentCellAlive = board[row][column] == 1;
            // Collect all neighbors
            int aliveNeighbors = 0;
            if (row-1>=0) {
                aliveNeighbors = board[row-1][column] == 1 ? aliveNeighbors + 1 : aliveNeighbors;
            }
            if (row+1 < totalRows) {
                aliveNeighbors = board[row+1][column] == 1 ? aliveNeighbors + 1 : aliveNeighbors;
            }
            if (column-1>=0) {
                aliveNeighbors = board[row][column-1] == 1 ? aliveNeighbors + 1 : aliveNeighbors;
            }
            if (column+1 < totalColumns) {
                aliveNeighbors = board[row][column+1] == 1 ? aliveNeighbors + 1 : aliveNeighbors;
            }
            if (row-1>=0 && column-1>=0) {
                aliveNeighbors = board[row-1][column-1] == 1 ? aliveNeighbors + 1 : aliveNeighbors;
            }
            if (row-1>=0 && column+1<totalColumns) {
                aliveNeighbors = board[row-1][column+1] == 1 ? aliveNeighbors + 1 : aliveNeighbors;
            }
            if (row+1<totalRows && column+1<totalColumns) {
                aliveNeighbors = board[row+1][column+1] == 1 ? aliveNeighbors + 1 : aliveNeighbors;
            }
            if (row+1<totalRows && column-1>=0) {
                aliveNeighbors = board[row+1][column-1] == 1 ? aliveNeighbors + 1 : aliveNeighbors;
            }


            // Explore all neighbors as per the rule
            if (isCurrentCellAlive && aliveNeighbors < 2) {
                copy[row][column] = 0; // under-population
                continue;
            }

            if (isCurrentCellAlive && (aliveNeighbors == 2 || aliveNeighbors == 3)) {
                continue; // lives
            }

            if (isCurrentCellAlive && aliveNeighbors > 3) {
                copy[row][column] = 0; // over-population
                continue;
            }

            if (!isCurrentCellAlive && aliveNeighbors == 3) {
                copy[row][column] = 1; // reproduction
                continue;
            }
        }
    }

    board = copy;
}