#include <iostream>

using namespace std;

/**
 * Keep on removing the elements when examined as it will be helpful to loop in next time as you don't have to 
 * play with rows and columns.
 */
std::vector<int> getSpiralMatrix(vector<vector<int>> Mat) {
    std::vector<int> result;

    while (!Mat.empty()) {
        // Extract the top row where row is constant (first) but column increments
        if (!Mat.empty()) {
            vector<int> topRow = Mat.front();
            for (int column = 0; column < topRow.size(); column++) {
                result.push_back(topRow.at(column));
            }
            // Remove the top row as it will make it easy for the next time to extract 1st row and subsequent.
            // As always you would need to extract 0th row.
            Mat.erase(Mat.begin());
        }

        if (!Mat.empty()) {
            // Extract the right column where column is constant (last) but row increments
            for (int row = 0; row < Mat.size(); row++) {
                if (!Mat.at(row).empty()) {
                    result.push_back(Mat.at(row).back());
                    // Remove the right columne where you are assessing each row (Similar principle)
                    Mat.at(row).pop_back();
                } else {
                    // Remove the row as columns are empty
                    Mat.erase(Mat.begin() + row);
                }
                
            }
        }

        if (!Mat.empty()) {
            // Extract the bottom row where row is constant (last) but column decrements
            vector<int> lastRow = Mat.back();
            if (!lastRow.empty()) {
                for (int column = lastRow.size() - 1; column >= 0; column--) {
                    result.push_back(lastRow.at(column));
                }
            }
            // Delete the last row
            Mat.pop_back();
        }

        // Extract the left column where column is constant (first) but row decrements
        if (!Mat.empty()) {
            for (int row = Mat.size() - 1; row >=0; row--) {
                if (!Mat.at(row).empty()) {
                    result.push_back(Mat.at(row).front());
                    // Remove the right columne where you are assessing each row (Similar principle)
                    Mat.at(row).erase(Mat.at(row).begin());
                } else {
                    // Remove the row as columns are empty
                    Mat.erase(Mat.begin() + row);
                }
            }
        }

        // Repeat
    }
    return result;
}

int main() {
    vector<vector<int>> Mat {{7}, {8}, {9}};
    std::vector<int> res = getSpiralMatrix(Mat);
    for (auto ele : res) {
        std::cout << ele << ", " << std::endl;
    }
}