#include <iostream>
#include <map>
#include <stack>

using namespace std;

int getLongestParenthesis(string parenthesis) {

    std::stack<int> indexStack;
    int longestParenthesisCount = 0;
    int currentParenthesisCount = 0;
    indexStack.push(-1);

    for (int i = 0 ; i < parenthesis.size(); i++) {
        if (parenthesis.at(i) == ')') {

            // Since we encountered a closing bracket, the top of the stack must be '('.
            // If top of stack is '(', we calculate the count by current index - index at top of stack. Index at top of stack means it does not have an closing pair yet.
            // Till a closing pair is not found, we will consider the distance between current index and index at the top of stack having valid pair.
            // If the top of the stack was not '(', that means closing bracket does not have a pair and push it into the stack.
            indexStack.pop(); // stack will atleast have -1;
            if (indexStack.empty()){
                indexStack.push(i);
            } else {
                currentParenthesisCount = i - indexStack.top();
                longestParenthesisCount = max(longestParenthesisCount, currentParenthesisCount);
            }
        } else {
            indexStack.push(i);
        }
    }

    return longestParenthesisCount;
}

int main() {
    string parenthesis = "()(())";
    std::cout << getLongestParenthesis(parenthesis) << std::endl;
}