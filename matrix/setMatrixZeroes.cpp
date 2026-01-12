#include <iostream>

using namespace std;

void setMatrixZeroes(vector<vector<int>>& Mat) {
    std::vector<int> rowIndex;
    std::vector<int> columnIndex;
    // O(M*N)
    for (int row = 0; row < Mat.size(); row++) {
        for (int column = 0; column < Mat.at(row).size(); column++) {
            if (!Mat[row][column]) {
                rowIndex.push_back(row);
                columnIndex.push_back(column);
            }
        }
    }

    //Make all the columns of that RowIndex zero
    for (int row : rowIndex) {
        for (int column = 0; column < Mat.at(row).size(); column++) {
            Mat.at(row)[column] = 0;
        }
    }

    // Make all the rows of that columnIndex zero
    for (int column : columnIndex) {
        for (int row = 0; row < Mat.size(); row++) {
            Mat.at(row)[column] = 0;
        }
    }
}

int main() {
    int R = 3;
    int C = 3;
    vector<vector<int>> Mat {{1, 1, 1}, {1, 0, 1}, {1, 1, 1}};
    setMatrixZeroes(Mat);
    for (int i = 0; i < R; i++) {
        for (int j = 0; j < C; j++) cout << Mat[i][j] << " ";
        cout << "\n";
    }
    cout << "~" << "\n";
    
}