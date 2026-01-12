#include <iostream>

using namespace std;

/**
 * Top layer is the 'top row of a layer' starting from column 0 till the last column. 
 * We will place the values in the last column starting from top row to the end row.
 */
void rotateTopLayer(int startingRowId, int startingColumnId, int endingRowId, int endingColumnId, vector<vector<int>> Mat, vector<vector<int>>& Res)
{
    int rowIncrement = 0;
    for (int column = startingColumnId; column <= endingColumnId; column++)
    {
        if (startingRowId + rowIncrement <= endingRowId)
        {
            cout << " Element " << Mat[startingRowId][column] << " is in top part of layer" << endl;
            Res[startingRowId + rowIncrement][endingColumnId] = Mat[startingRowId][column];
            rowIncrement++;
        } else {
            cout << " Went wrong in top part of layer " << endl;
        }
    }
}

/**
 * Right layer is the 'right column of a layer' starting from top row to the last row
 * We will place the values in the last row starting from last column to the first column.
 */
void rotateRightLayer(int startingRowId, int startingColumnId, int endingRowId, int endingColumnId, vector<vector<int>> Mat, vector<vector<int>>& Res)
{
    int columnDecrement = 0;
    for (int row = startingRowId; row <= endingRowId; row++)
    {
        if (endingColumnId - columnDecrement >= startingColumnId)
        {
            cout << " Element " << Mat[row][endingColumnId] << " is in right part of layer " << endl;
            Res[endingRowId][endingColumnId - columnDecrement] = Mat[row][endingColumnId];
            columnDecrement++;
        } else {
            cout << " Went wrong in right part of layer " << endl;
        }
    }
}

/**
 * Bottom layer is the 'bottom row of a layer' starting from last column to first column
 * We will place the values in the first column starting from last row to the first row.
 */
void rotateBottomLayer(int startingRowId, int startingColumnId, int endingRowId, int endingColumnId, vector<vector<int>> Mat, vector<vector<int>>& Res)
{
    int rowDecrement = 0;
    for (int column = endingColumnId; column >= startingColumnId; column--)
    {
        if (endingRowId - rowDecrement >= startingRowId)
        {
            cout << " Element " << Mat[endingRowId][column] << " is in bottom part of layer " << endl;
            Res[endingRowId - rowDecrement][startingColumnId] = Mat[endingRowId][column];
            rowDecrement++;
        } else {
            cout << " Went wrong in bottom part of layer " << endl;
        }
    }
}

/**
 * Left layer is the 'left column of a layer' starting from last row to first row
 * We will place the values in the top row starting from first column to the last column.
 */
void rotateLeftLayer(int startingRowId, int startingColumnId, int endingRowId, int endingColumnId, vector<vector<int>> Mat, vector<vector<int>>& Res)
{
    int columnIncrement = 0;
    for (int row = endingRowId; row >= startingRowId; row--)
    {
        if (startingColumnId + columnIncrement <= endingColumnId)
        {
            cout << " Element " << Mat[row][startingColumnId] << " is in left part of layer " << endl;
            Res[startingRowId][startingColumnId + columnIncrement] = Mat[row][startingColumnId];
            columnIncrement++;
        } else {
            cout << " Went wrong in left part of layer " << endl;
        }
    }
}

/**
 * Total layers = R/2 or R/2+1
 * 1. For every layer which is not a straight line, rotate top, right, bottom and left layer
 * 2. If it is a straight line, the values will remain unchanged.
 * 
 * 3. If the layer is over, we will continue the next layer by incrementing the starting row and starting column
 *    and decrementing the ending row and ending column.,
 */
void rotateBruteForce(vector<vector<int>>& Mat) 
{
    int totalRows = Mat.size();
    int totalColumns = Mat.at(0).size();
    if (totalRows != totalColumns) {
        cout << "terminate " << endl;
        return;
    }

    // initialize a vector and default value is -1
    vector<vector<int>> Resultant(totalRows, vector<int>(totalColumns, -1));

    int totalLayers =  (totalRows % 2) == 0 ? totalRows/2 : totalRows/2 + 1;
    int startingRowId = 0;
    int startingColumnId = 0;
    int endingRowId = totalRows - 1;
    int endingColumnId = totalColumns - 1;

    cout << "total layers " << totalLayers << endl;

    for (int layer = 0; layer < totalLayers; layer++)
    {
        if (startingRowId != endingRowId) {
            rotateTopLayer(startingRowId, startingColumnId, endingRowId, endingColumnId, Mat, Resultant);
            rotateRightLayer(startingRowId, startingColumnId, endingRowId, endingColumnId, Mat, Resultant);
            rotateBottomLayer(startingRowId, startingColumnId, endingRowId, endingColumnId, Mat, Resultant);
            rotateLeftLayer(startingRowId, startingColumnId, endingRowId, endingColumnId, Mat, Resultant);
        } 
        else 
        {
            for (int currentRow = startingRowId; currentRow <= endingRowId; currentRow++)
            {
                for (int currentColumn = startingColumnId; currentColumn <= endingColumnId; currentColumn++)
                {
                    Resultant[currentRow][currentColumn] = Mat[currentRow][currentColumn];
                    cout << " Element " << Mat[currentRow][currentColumn] << " is in staright line of layer " << endl;
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
    Mat = Resultant;
}


/**
 * Puzzle: 
 *    1. Transpose the matrix - Swap rows and columns
 *    2. For every row, reverse the columns.
 */
void rotateWithTrick(vector<vector<int>>& Mat) {

    // O(M*N)
    for (int row = 0; row < Mat.size(); row++) {
        for (int column = row; column < Mat.at(row).size(); column++) {
            std::swap(Mat[row][column], Mat[column][row]);
        }
    }

    // O(N2)
    for (int row = 0; row < Mat.size(); row++) {
        std::reverse(Mat.at(row).begin(), (Mat.at(row).end())); // O(N)
    } 
}

int main() 
{
    int R = 4;
    int C = 4;
    vector<vector<int>> Mat {{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}, {13, 14, 15, 16}};
    rotateWithTrick(Mat);
    for (int i = 0; i < R; i++) {
        for (int j = 0; j < C; j++) cout << Mat[i][j] << " ";
        cout << "\n";
    }
    cout << "~" << "\n";
}

// } Driver Code Ends
