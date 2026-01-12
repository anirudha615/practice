#include <iostream>

using namespace std;

bool elementBelongsToLayer(int currentRow, int currentColumn, int startingRowId, int startingColumnId, int endingRowId, int endingColumnId)
{
    return !((currentRow >= (startingRowId + 1) && currentRow <= endingRowId - 1) && 
                (currentColumn >= (startingColumnId + 1) && currentColumn <= endingColumnId - 1));
}

/**
 * Top layer is the 'top row of a layer' starting from column 0 till the second last column. 
 * Note: The last column falls under the right layer.
 */
bool isTopPartOfLayer(int currentRow, int currentColumn, int startingRowId, int startingColumnId, int endingRowId, int endingColumnId) {
    return currentRow == startingRowId && currentColumn < endingColumnId && (startingRowId != endingRowId);
}

/**
 * Right layer is the 'right column of a layer' which is the last column and it ranges from top row to the second last row of the layer.
 * Note: The last row falls under the bottom layer.
 */
bool isRightPartOfLayer(int currentRow, int currentColumn, int startingRowId, int startingColumnId, int endingRowId, int endingColumnId) {
    return currentRow < endingRowId && currentColumn == endingColumnId && (startingColumnId != endingColumnId);
}

/**
 * Bottom layer is the 'bottom row of a layer' starting from last column to the second column.
 * Note: The first column falls under the left layer.
 */
bool isBottomPartOfLayer(int currentRow, int currentColumn, int startingRowId, int startingColumnId, int endingRowId, int endingColumnId) {
    return currentRow == endingRowId && currentColumn != startingColumnId && (startingRowId != endingRowId);
}

/**
 * Left layer is the 'left column of a layer' starting from bottom row to the second row.
 * Note: The first row falls under the top layer.
 */
bool isLeftPartOfLayer(int currentRow, int currentColumn, int startingRowId, int startingColumnId, int endingRowId, int endingColumnId) {
    return currentRow > startingRowId && currentRow <= endingRowId && currentColumn == startingColumnId && (startingColumnId != endingColumnId);
}


/**
 * Total layers = R/2 or R/2+1
 * 1. If top layer, move the value to the next column while in the same row.
 * 2. If right layer, move the value to the next row while in the same column.
 * 3. If bottom layer, move the value to the previous column while in the same row
 * 4. If left layer, move the value to the previous row while in the same column
 * 
 * 5. If the layer is over, we will continue the next layer by incrementing the starting row and starting column
 *    and decrementing the ending row and ending column.,
 */
vector<vector<int>> rotateMatrix(int M, int N, vector<vector<int > > Mat) 
{
    // initialize a vector and default value is -1
    vector<vector<int>> Resultant(M, vector<int>(N, -1));

    int totalLayers =  std::round(M/2) + 1;
    int startingRowId = 0;
    int startingColumnId = 0;
    int endingRowId = M - 1;
    int endingColumnId = N - 1;

    cout << "total layers " << totalLayers << endl;

    for (int layer = 0; layer < totalLayers; layer++)
    {
        for (int currentRow = startingRowId; currentRow <= endingRowId; currentRow++) 
        {
            for (int currentColumn = startingColumnId; currentColumn <= endingColumnId; currentColumn++) 
            {
                if (elementBelongsToLayer(currentRow, currentColumn, startingRowId, startingColumnId, endingRowId, endingColumnId))
                {
                    cout << " Element " << Mat[currentRow][currentColumn] << " belongs to layer " << layer << endl;
                    if (isTopPartOfLayer(currentRow, currentColumn, startingRowId, startingColumnId, endingRowId, endingColumnId))
                    {
                        Resultant[currentRow][currentColumn + 1] = Mat[currentRow][currentColumn];
                        cout << " Element " << Mat[currentRow][currentColumn] << " is in top part of layer " << endl;
                    } 
                    else if (isRightPartOfLayer(currentRow, currentColumn, startingRowId, startingColumnId, endingRowId, endingColumnId))
                    {
                        Resultant[currentRow + 1][currentColumn] = Mat[currentRow][currentColumn];
                        cout << " Element " << Mat[currentRow][currentColumn] << " is in right part of layer " << endl;
                    }
                    else if (isBottomPartOfLayer(currentRow, currentColumn, startingRowId, startingColumnId, endingRowId, endingColumnId))
                    {
                        Resultant[currentRow][currentColumn - 1] = Mat[currentRow][currentColumn];
                        cout << " Element " << Mat[currentRow][currentColumn] << " is in bottom part of layer " << endl;
                    }
                    else if (isLeftPartOfLayer(currentRow, currentColumn, startingRowId, startingColumnId, endingRowId, endingColumnId))
                    {
                        Resultant[currentRow - 1][currentColumn] = Mat[currentRow][currentColumn];
                        cout << " Element " << Mat[currentRow][currentColumn] << " is in left part of layer " << endl;
                    } 
                    else
                    {
                        Resultant[currentRow][currentColumn] = Mat[currentRow][currentColumn];
                        cout << " Element " << Mat[currentRow][currentColumn] << " is in staright line of layer " << endl;
                    } 
                } else 
                {
                    cout << " Element " << Mat[currentRow][currentColumn] << " does not belong to layer " << layer << endl;
                }
            }
        }

        startingRowId = startingRowId + 1;
        endingRowId = endingRowId - 1;
        startingColumnId = startingColumnId +1;
        endingColumnId = endingColumnId - 1;
        cout << " new startingRowId " << startingRowId << " new endingRowId " << endingRowId << endl;
        cout << " new startingColumnId " << startingColumnId << " new endingColumnId " << endingColumnId << endl;
    }
    return Resultant;
}

int main() 
{
    int R = 5;
    int C = 5;
    vector<vector<int>> Mat {{1, 2, 3, 4, 5}, {6, 7, 8, 9, 10}, {11, 12, 13, 14, 15}, {16, 17, 18, 19, 20}, {21, 22, 23, 24, 25}};
    vector<vector<int>> ans = rotateMatrix(R, C, Mat);
    for (int i = 0; i < R; i++) {
        for (int j = 0; j < C; j++) cout << ans[i][j] << " ";
        cout << "\n";
    }
    cout << "~" << "\n";
}

// } Driver Code Ends
