#include <iostream>
#include <unordered_map>
#include <set>
#include <memory>
#include <thread>
#include <mutex>
#include <chrono>
#include <algorithm>
#include <cmath>

using namespace std;

class SimpleIterator {
private:
    int m_totalPosition = 0;
    int m_currentPosition = 0;
public:

    SimpleIterator(int totalPosition): m_totalPosition(totalPosition) {}
    
    void next() {
        if (m_currentPosition < m_totalPosition) {
            ++m_currentPosition;
        }
    }

    int getPosition() {
        return m_currentPosition;
    }

    void setPosition(int position) {
        m_currentPosition = position;
    }
};

int main() {
    SimpleIterator iterator (1000);
    for (int i = 0; i < 555; i++) {
        iterator.next();
    }
    std::cout << iterator.getPosition() << " should match 555" << std::endl;
    iterator.setPosition(2);
    for (int i = 0; i < 900; i++) {
        iterator.next();
    }
    std::cout << iterator.getPosition() << " should match 902" << std::endl;
}