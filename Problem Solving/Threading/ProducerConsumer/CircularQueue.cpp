#include <iostream>
using namespace std;

class MyCircularQueue {
private:
    std::vector<int> queue;
    int m_enqueuePointer;
    int m_dequeuePointer;
    int m_totalCapacity;
    int m_currentCapacity;
public:
    MyCircularQueue(int k) {
        queue.resize(k, -1);
        m_totalCapacity = k;
        m_currentCapacity = 0;
        m_enqueuePointer = -1; // We will increment and then persist the value so that rear() can use enqueue_pointer
        m_dequeuePointer = 0; // We will remove the value and then increment so that front() can use dequeue_pointer
    }
    
    bool enQueue(int value) {
        if (m_currentCapacity == m_totalCapacity) {
            return false;
        }
        ++m_currentCapacity;
        m_enqueuePointer = (m_enqueuePointer + 1) % m_totalCapacity;
        queue[m_enqueuePointer] = value;
        return true;
    }
    
    bool deQueue() {
        if (!m_currentCapacity) {
            return false;
        }
        queue[m_dequeuePointer] = -1; // dequeue
        --m_currentCapacity;
        m_dequeuePointer = (m_dequeuePointer + 1) % m_totalCapacity;
        return true;
    }
    
    int Front() {
        return queue[m_dequeuePointer];
    }
    
    int Rear() {
        if (m_enqueuePointer < 0) {
            return -1;
        }
        return queue[m_enqueuePointer];
    }
    
    bool isEmpty() {
        return !m_currentCapacity;
    }
    
    bool isFull() {
        return m_currentCapacity == m_totalCapacity;
    }
};

int main() {
    MyCircularQueue queue (3);
    std::cout << "Rear (Expected -1) : " << queue.Rear() << std::endl;
    std::cout << "Enqueue (Expected 'true') : " << queue.enQueue(2) << std::endl;
    std::cout << "Enqueue (Expected 'true') : " << queue.enQueue(3) << std::endl;
    std::cout << "Enqueue (Expected 'true') : " << queue.enQueue(4) << std::endl;
    std::cout << "Rear (Expected 4) : " << queue.Rear() << std::endl;
    std::cout << "Front (Expected 2) : " << queue.Front() << std::endl;
    std::cout << "isFull (Expected true) : " << queue.isFull() << std::endl;
    std::cout << "Dequeue (Expected 'true') : " << queue.deQueue() << std::endl;
    std::cout << "Enqueue (Expected 'true') : " << queue.enQueue(4) << std::endl;
    std::cout << "Rear (Expected 4) : " << queue.Rear() << std::endl;
    std::cout << "Front (Expected 3) : " << queue.Front() << std::endl;
    std::cout << "Dequeue (Expected 'true') : " << queue.deQueue() << std::endl;
    std::cout << "Dequeue (Expected 'true') : " << queue.deQueue() << std::endl;
    std::cout << "Dequeue (Expected 'true') : " << queue.deQueue() << std::endl;
    std::cout << "Dequeue (Expected 'false') : " << queue.deQueue() << std::endl;
    std::cout << "isEmpty (Expected true) : " << queue.isEmpty() << std::endl;

}