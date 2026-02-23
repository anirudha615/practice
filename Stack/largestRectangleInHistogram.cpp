#include <iostream>
#include <map>
#include <stack>

using namespace std;

/**
 * 1. We will keep on pushing bars to the stack if the bar height >= top bar in the stack. All the elements in the stack will be increasing.
 * 2. Once we reach a bar whose height is < top bar in the stack, we will start calculating area by popping out the top bar. (Assessment phase)
 *      a. The reason is the popped bar will be greater among the currently evaluated bar and the top bar in the stack. (stack is increasing and the condition for which we pop)
 *      b. So, the area occupied by it = height of popped bar * width of all bars in between the currently evaluated bar and top bar in the stack (excluding both of them)
 * 3. If all the bars are increasing, we may never reach a condition where bar height < top bar in the stack. (Cleanup Phase)
 *      a. In that case, we will pop out the top bar and it will be greater between the top bar in the stack till the end of array
 *      b. This is because unlike Assessment phase, there is no currently evaluated bar which required us to pop the top bar before reaching the end of array.
 *         We have just 1 stoper which is top bar of the stack.
 *      c. So, area = height of popped bar * width of all bars in between the top bar in the stack and end of array.
 * 4. The difference between assessment and cleanup phase is width calculation
 *      a. Assessment - the popped bar is maximum between top bar in the stack and currently evaluated bar.
 *      b. Cleanup - the popper bar is maximum between top bar in the stack and end of array.
 */
int largestRectangleInHistogram(std::vector<int> heights) {

    std::stack<std::pair<int, int>> minStack;
    int largestRectangleInHistogram = 0;

    for (int i = 0; i < heights.size(); i++) {
        while (!minStack.empty() && heights.at(i) < minStack.top().first) {
            std::pair<int, int> topElement = minStack.top();
            minStack.pop();
            // width = width of all bars excluding excluding currently evaluated bar and top bar in the stack
            int width = minStack.empty() ? i : (i - minStack.top().second - 1);
            // calculate area = height of bar * width
            int area = topElement.first * width;
            largestRectangleInHistogram = max(area, largestRectangleInHistogram);
        }
        minStack.push({heights.at(i), i});
    }

    int rightMostBoundary = heights.size() -1 ;
    while (!minStack.empty()) {
        std::pair<int, int> topElement = minStack.top();
        minStack.pop();
        // width = width of all bars excluding the top bar in the stack
        int width = minStack.empty() ? (rightMostBoundary + 1) : (rightMostBoundary - minStack.top().second);
        // calculate area = height of bar * width
        int area = topElement.first * width;
        largestRectangleInHistogram = max(area, largestRectangleInHistogram);
    }

    return largestRectangleInHistogram;
}

int main() {
    std::vector<int> heights {2, 8, 5, 6, 2, 3};
    std::cout << largestRectangleInHistogram(heights) << std::endl;
}