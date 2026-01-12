#include <iostream>
#include <map>
#include <stack>

using namespace std;

std::vector<int> nextWarmerDay(std::vector<int> array) {

    std::stack<int> indexStack;
    std::vector<int> nextWarmerDayWaitingList (array.size(), 0);

    for (int i = 0; i < array.size(); i++) {
        while (!indexStack.empty() && array.at(i) > array.at(indexStack.top())) {
            int indexPopped = indexStack.top();
            indexStack.pop();
            nextWarmerDayWaitingList[indexPopped] = i - indexPopped;
       }
       indexStack.push(i);
    }

    return nextWarmerDayWaitingList;
}

int main() {
    std::vector<int> array {65, 70, 68, 60, 55, 75, 80, 74};
    std::vector<int> ans = nextWarmerDay(array);
    for (auto a : ans) {
        std::cout << a << ", " << std::endl;
    }
}