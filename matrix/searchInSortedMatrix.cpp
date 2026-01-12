#include <iostream>
using namespace std;

/**
 * Inspect the nested loop if the target is greater than the first column and less than the last column
 */
bool searchMatrix(vector<vector<int>>& mat, int target) 
{
    int totalRows = mat.size();
    int totalColumns = mat.at(0).size();

    for (int row = 0; row < totalRows; row++)
    {
        if (target >= mat.at(row).at(0) && target <=  mat.at(row).at(totalColumns - 1))
        {
            for (int column = 0; column < totalColumns; column++)
            {
                if (target == mat[row][column]) 
                {
                    return true;
                }
            }
        }
    }
    return false; 
}


int main() 
{
    int R = 3;
    int C = 5;
    vector<vector<int>> Mat {{1, 2, 3, 4, 5}, {6, 7, 8, 9, 10}, {11, 12, 13, 14, 15}};
    auto ans = searchMatrix(Mat, 8);
    cout << "ans : " << ans << "\n";
}