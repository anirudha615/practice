#include <iostream>
using namespace std;

/**
 * Puzzle logic - 
 *   1. In order to gain score, I have to spend my minimum token which will reduce the power.
 *   2. In order to gain power, I have to sacrifice by score by utilizing the maximum token.
 * 
 * This is a greedy problem because in each step, we know the decision to be made (First priority - gain score, second priority - gain power).
 * This could become a DP problem if at every step, we need the previous step decision to make the better decision for the current step.
 * 
 * So, logic - 
 *    1. First priority is to keep gaining score by spending minimum token to sacrifice power.
 *    2. Once you cannot sacrifice power, try to gain power by utilizing your maximum token.
 * 
 * So, 2 pointer where left = minimum and right = maximum
 */
int bagOfTokensScore(vector<int>& tokens, int power) {
    if (tokens.empty()) {
        return 0;
    }
    std::sort(tokens.begin(), tokens.end());
    int left = 0;
    int right = tokens.size()-1;
    int maxScore = 0;
    int score = 0;
    while (left < right) {
        // To gain score, sacrifice lowest power.
        // Your first priority should always be gaining score. So, look for that opportunity.
        if (tokens[left] <= power) {
            power -= tokens[left];
            maxScore = std::max(maxScore, ++score);
            ++left;
        } else if (score >= 1) {
            // To gain power, utilize maximum token.
            // If you cannot spend power to gain score, check right to gain maximum power by losing score
            power += tokens[right];
            --score;
            --right;
        } else {
            // If you tried both left and right and nothing works
            ++left;
            --right;
        }
    }
    // Our last effort should only be to gain score as there is no point in gaining power.
    if (left == right) {
        if (tokens[left] <= power) {
            power -= tokens[left];
            maxScore = std::max(maxScore, ++score);
        }
    }
    return maxScore;
}

int main() {
    std::vector<int> tokens {100,200,300,400};
    std::cout << bagOfTokensScore(tokens, 200) << std::endl;
}