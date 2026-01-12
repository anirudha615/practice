#include <iostream>
#include <map>
#include <stack>

using namespace std;

std::vector<int> nextGreaterElement(std::vector<int> array) {

    std::stack<int> indexStack;
    std::vector<int> nextGreaterElementList (array.size(), -1);

    for (int i = 0; i < array.size(); i++) {
        while (!indexStack.empty() && array.at(i) > array.at(indexStack.top())) {
            int indexPopped = indexStack.top();
            indexStack.pop();
            nextGreaterElementList[indexPopped] = array.at(i);
       }
       indexStack.push(i);
    }

    return nextGreaterElementList;
}

int main() {
    std::vector<int> array {2, 1, 3, 2, 4, 3};
    std::vector<int> ans = nextGreaterElement(array);
    for (auto a : ans) {
        std::cout << a << ", " << std::endl;
    }
}