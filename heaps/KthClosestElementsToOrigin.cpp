#include <iostream>
#include <unordered_map>
#include <cmath>
using namespace std;

struct Element {
    int _distance;
    int _index;
    Element(int distance, int index): _distance(distance) , _index(index) {}

    const bool operator<(const Element otherElement) const {
        return _distance > otherElement._distance;
    }
};

/**
 * Find the K closest element to the origin. O(KLogN)
 * 
 * 1. We will calculate the distance using (x1 - x2)^2 + (y1 - y2)^2
 * 2. Push the distance and index to the min heap
 * 3. Pop the first K elements from the min heap and return the results
 */
std::vector<vector<int>> kClosest(vector<vector<int>> points, int k) {
    std::vector<vector<int>> results;
    std::priority_queue<Element> minHeap;
    std::vector<int> origin {0,0};

    // O(N) for building a minHeap
    for (int i = 0; i < points.size(); i++) {
        int distance = pow((points[i].at(0) - origin.at(0)), 2) + pow((points[i].at(1) - origin.at(1)), 2);
        minHeap.push({distance, i});
    }

    // O(KLogN) for Heapify when popping
    for (int i = 0; i < k; i++) {
        results.push_back(points.at(minHeap.top()._index));
        minHeap.pop();
    }

    return results;
}

int main() {
    vector<vector<int>> points = { {1, 3}, {2, -2}, {-2, 2}};
    vector<vector<int>> res = kClosest(points, 2);
    
    for (const auto coordinate : res) {
        std::cout << coordinate.at(0) << " ," << coordinate.at(1) << std::endl;
    }
}

