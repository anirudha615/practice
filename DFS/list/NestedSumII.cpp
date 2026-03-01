#include <iostream>
#include <vector>
using namespace std;

class NestedInteger {
private:
    bool m_isInteger;
    int m_value;
    vector<NestedInteger> m_list;

public:
    // Constructs an empty nested list
    NestedInteger() : m_isInteger(false), m_value(0) {}

    // Constructs a single integer
    NestedInteger(int value) : m_isInteger(true), m_value(value) {}

    // Returns true if holding a single integer
    bool isInteger() const {
        return m_isInteger;
    }

    // Returns the integer value — undefined behavior if holding a list
    int getInteger() const {
        return m_value;
    }

    // Sets this to hold a single integer
    void setInteger(int value) {
        m_isInteger = true;
        m_value = value;
        m_list.clear();
    }

    // Adds ni to the list and marks this as a list
    void add(const NestedInteger& ni) {
        m_isInteger = false;
        m_list.push_back(ni);
    }

    // Returns the nested list — undefined behavior if holding an integer
    const vector<NestedInteger>& getList() const {
        return m_list;
    }
};


/** Whenever this gets called, we only pass NestedInteger which has an inbuilt list */
/** For single integer, we simply add to flatList */
void traverseNestedList(int currentDepth, int& maxDepth, NestedInteger& nest, std::vector<std::pair<int, int>>& flatList) {
    vector<NestedInteger> nestedList = nest.getList();
    currentDepth++;
    maxDepth = std::max(maxDepth, currentDepth);
    for (NestedInteger& element : nestedList) {
        if (element.isInteger()) {
            flatList.push_back({element.getInteger(), currentDepth});
        } else {
            traverseNestedList(currentDepth, maxDepth, element, flatList);
        }
    }
}

/**
 * https://www.hellointerview.com/community/questions/nested-list-weight-sum/cm5eh7nri04x4838opzm4x7hx?level=SENIOR
 */
int depthSumInverse(vector<NestedInteger>& nestedList) {
    int totalSum = 0;
    if (nestedList.empty()) {
        return totalSum;
    }
    int maxDepth = 0;
    int currentDepth = 0;
    std::vector<std::pair<int, int>> flatList;
    /** First Iteration because we are receiving  vector<NestedInteger> instead of NestedInteger */
    ++currentDepth;
    maxDepth = std::max(maxDepth, currentDepth);
    for (NestedInteger& element : nestedList) {
        if (element.isInteger()) {
            flatList.push_back({element.getInteger(), currentDepth});
        } else {
            traverseNestedList(currentDepth, maxDepth, element, flatList);
        }
    }

    // Retrieve total sum using flatList
    for (const std::pair<int, int> elementWithWeights : flatList) {
        int weight = maxDepth - elementWithWeights.second + 1;
        totalSum += (weight * elementWithWeights.first);
    }

    return totalSum;
}




int main() {
    // Build [6]
    NestedInteger six(6);

    // Build [4, [6]]
    NestedInteger inner;
    inner.add(NestedInteger(4));
    inner.add(six);               // six is treated as a list wrapper here

    // Wait — [6] should be a list containing 6, not the integer 6 directly
    // Build [6] as a list
    NestedInteger listOf6;
    listOf6.add(NestedInteger(6));

    // Build [4, [6]]
    NestedInteger innerList;
    innerList.add(NestedInteger(4));
    innerList.add(listOf6);

    // Build [1, [4, [6]]]
    vector<NestedInteger> nestedList;
    nestedList.push_back(NestedInteger(1));
    nestedList.push_back(innerList);

    // nestedList now represents [1, [4, [6]]]
    // pass to your depthSumInverse function
    cout << depthSumInverse(nestedList) << "\n";

    return 0;
}
