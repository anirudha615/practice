#include <iostream>
using namespace std;

int team(vector <int> & skill, int n)
{
    int pairs = 0;
    for (int i = 0; i < skill.size(); i++)
    {
        if (skill.at(i) > 2)
        {
            for (int j = i; j < skill.size(); j++)
            {
                if (skill.at(i) > 2 * skill.at(j))
                {
                    pairs++;
                }
            }
        }
    }
    return pairs;
}


int main() 
{
    int R = 3;
    int C = 5;
    vector<int> Mat {3,2,1,4};
    auto ans = team(Mat, Mat.size());
    cout << "ans : " << ans << "\n";
}