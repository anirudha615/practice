#include <iostream>
#include <map>
#include <stack>

using namespace std;

bool isValid(string parenthesis) {
    std::unordered_map<char, char> map = {
        {')', '('}, {'}', '{'}, {']', '['}
    };

    std::stack<char> stack;

    for (char c : parenthesis) {
        if (map.count(c)) {
            if (!stack.empty() && stack.top() == map[c]) {
                stack.pop();
            } else {
                return false;
            }
        } else {
            stack.push(c);
        }
    }

    return !stack.size();
}

int main() {
    string parenthesis = "]";
    std::cout << isValid(parenthesis) << std::endl;
}