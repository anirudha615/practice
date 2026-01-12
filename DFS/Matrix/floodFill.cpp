#include <iostream>
#include <vector>

using namespace std;

void dfs_helper(vector<vector<int>>& image, int row, int column, int newColor, int oldColor) {
    // Check if the row and column is within the matrix boundary
    if (!(row >= 0 && row < image.size() && column >= 0 && column < image[0].size())) {
        return;
    }

    // check if image[row][column] is not oldColor. If its not, just return (this is basically checking what visited list would do)
    if (image[row][column] != oldColor) {
        return;
    }

    // Mark the cell as visited (by coloring them)
    image[row][column] = newColor;

    // Traverse the matrix in directions
    dfs_helper(image, row - 1, column, newColor, oldColor); // UP
    dfs_helper(image, row + 1, column, newColor, oldColor); // DOWN
    dfs_helper(image, row, column + 1, newColor, oldColor); // LEFT
    dfs_helper(image, row, column - 1, newColor, oldColor); // RIGHT
}

/**
 * Don't run DFS if old and new color is same.
 */
vector<vector<int>> flood_fill(vector<vector<int>>& image, int sr, int sc, int newColor) {
    if (image.empty()) {
        return image;
    }

    int oldColor = image[sr][sc];
    if (oldColor == newColor) {
        return image;
    }

    dfs_helper(image, sr, sc, newColor, oldColor);
    return image;
}

int main() {
    vector<vector<int>> image = {{1,0,1}, {1,0,0}, {0,0,1}};
    flood_fill(image, 1, 1, 2);
}