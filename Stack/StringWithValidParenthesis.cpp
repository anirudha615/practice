#include <iostream>
#include <unordered_set>
#include <stack>

using namespace std;

/**
 * https://www.hellointerview.com/community/questions/minimum-remove-parentheses/cm5eguhad02z3838o4mixbevj?level=MID_LEVEL
 * 
 * 1. If encounter ')', pop the top element. If stack is empty, there is no pair of this. Hence, needs to be removed.
 * 2. If after looping the entire string, stack has elements. That means '(' has no pair. Hence, needs to be removed.
 */
string minRemoveToMakeValid(string s) {
    std::stack<int> state;
    std::unordered_set<int> indexToRemove;
    for (int index = 0; index < s.size(); index++) {
        char c = s.at(index);
        if (c != '(' && c != ')') {
            continue;
        }

        if (c == '(') {
            state.push(index);
        } else {
            if (state.empty()) {
                indexToRemove.insert(index);
            } else {
                state.pop(); // blindly pop as it is valid
            }
        }
    }

    while (!state.empty()){
        indexToRemove.insert(state.top());
        state.pop();
    }

    string result = "";
    for (int index = 0; index < s.size(); index++) {
        if (!indexToRemove.count(index)) {
            result += s.at(index);
        }
    }
    return result;
}

int main() {
    std::cout << minRemoveToMakeValid("lee(t(c)o)de)") << std::endl;
}