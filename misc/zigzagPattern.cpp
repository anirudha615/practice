#include <iostream>
using namespace std;

/**
 * Approach:
 *    1. Keep adding the characters to the current row's state.
 *    2. If the current row hit the max row, change the direction to UP. If the current row hit 0, change direction to DOWN.
 *    3. For UP, --currentRow. For Down: ++currentRow
 * 
 * https://leetcode.com/problems/zigzag-conversion/description/
 */
string convert(string s, int numRows) {
    std::unordered_map<int, string> rowState;
    int currentRow = 0;
    string direction = "DOWN";
    for (char character : s) {
        // Check if the row exists in map. If not, add to map or append to map
        rowState[currentRow] += character;

        if (numRows == 1) {
            continue;
        }

        if (currentRow == numRows - 1) {
            direction = "UP";
        } else if (!currentRow) {
            direction = "DOWN";
        }

        if (direction == "DOWN") {
            ++currentRow;
        } else {
            --currentRow;
        }
    }

    string result = "";
    for (int row = 0; row < numRows; row++) {
        result += rowState[row];
    }
    return result;
}

int main() {
    std::cout << convert("PAYPALISHIRING", 3) << std::endl;
}