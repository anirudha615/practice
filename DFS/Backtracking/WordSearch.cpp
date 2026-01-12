#include <iostream>

/**
 * With every traversal, we aim to look for the next index. If the next index is not found, we won't explore any neighbors.
 * This is very similar to BFS where we check whether the next level has the next index of the word.
 * 
 * However, when we find the index in the next neighhbor, add the row,column in the visited list and keep exploring the neighbors with the next index.
 * 
 *  * 
 * Once the recursion of a node is over and we found that we haven't went the desired path (maybe a false route because few indexes match but the rest didn't), 
 * we pop back the node from the visited node list so that its parent can now explore the sibling (which it initially missed because of DFS.)
 * 
 * IMPORTANT: The only reason why BFS won't work is because when we go down a false route because indexToFind matches but the subsequent indexes doesn't,
 * it is impossible to backtrack. DFS helps here because if we go down a false route, we can come back and then find another route
 * 
 */
void dfsHelper(int row, int column, std::vector<std::vector<char>>& board,
    std::string word, int indexToFind, std::unordered_map<std::string, int>& visitedNode) {

    std::string key = std::to_string(row) + "," + std::to_string(column);
    if (visitedNode.count(key) && visitedNode[key] == 1) {
        return;
    }
        
    // Check if the node is out of bounds
    if (row < 0 || row >= board.size() || column < 0 || column >= board[0].size()) {
        return;
    }

    // If the path list is equal to word size, there is no point in traversing.
    // Cut short and return. 
    if (visitedNode.size() == word.size()) {
        return;
    }

    /**
     * If the character at row, column matches the char of the word,
     *   1. store it in the path list
     *   2. Increment the index
     *   3. Mark the node as visited
     * 
     * If it does not match, return.
     */
    char character = board[row][column];
    if (word.at(indexToFind) != character) {
        return;
    }

    // Mark the node as visited
    visitedNode[key] = 1;
    indexToFind++;
   
    dfsHelper(row - 1, column, board, word, indexToFind, visitedNode); // UP
    dfsHelper(row + 1, column, board, word, indexToFind, visitedNode); // DOWN
    dfsHelper(row, column - 1, board, word, indexToFind, visitedNode); // LEFT
    dfsHelper(row, column + 1, board, word, indexToFind, visitedNode); // RIGHT

    // Once the node's recursion stack is over and If the path list is less than word size, this was a false hope - 
    // we  pop the last element from path list so that the parent can explore the node's sibling instead.
    // Since recursion will happen, the current indexToFind will return to parent's indexToFind
    if (visitedNode.size() < word.size()) {
        visitedNode[key] = 0;
    }
}

bool searchWord(std::vector<std::vector<char>> board, std::string word) {
    std::unordered_map<std::string, int> visitedNode;
    int startingRow, startingCol = -1;
    for (int row = 0; row < board.size(); row++) {
        for (int col = 0; col < board[row].size(); col++) {
            // If there starting characters are similar and if starting the DFS from row, col didn't help,
            // flush the visisted node list and start fresh from another row, col
            if (board[row][col] == word[0]) {
                visitedNode.clear();
                startingRow = row;
                startingCol = col;
                // Start DFS on the starting row, col
                dfsHelper(startingRow, startingCol, board, word, 0, visitedNode);
                if (visitedNode.size() == word.size()) {
                    return true;
                }
            }
        }
    }
    return false;
}

int main() {
    std::vector<std::vector<char>> board = {
        {'A', 'B', 'C', 'E'},
        {'S', 'F', 'E', 'S'},
        {'A', 'D', 'E', 'E'}
    };
    std::string word = "ABCESEEEFS";
    std::cout << searchWord(board, word);
}