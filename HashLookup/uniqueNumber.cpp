#include <iostream>

using namespace std;

struct Node {
    int key;
    int frequency;
    Node* left;
    Node* right;
    Node(int k): key(k), frequency(1), left(nullptr), right(nullptr) {}
};

/**
 * 1. Keep a linked list of unique numbers in proper insertion order.
 * 2. If the first unique number frequency > 1, look up at the head of unique number and retrieve the key, Else -1
 * 
 * https://www.hellointerview.com/community/questions/first-unique-number/cm5eguhae0343838om5opedua?level=MID_LEVEL
 */
class FirstUnique {
private:
    int firstUniqueNumber = -1;
    std::unordered_map<int, Node*> m_state;
    Node* m_tail = nullptr;
    Node* m_head = nullptr;

    void updateDLL(Node* node) {
        // Add to DLL if frequency is 1
        if (node->frequency == 1) {
            if (m_tail) {
                m_tail->left = node;
                node->right = m_tail;
            }
            m_tail = node;
            // Update head also
            if (m_tail && !m_head) {
                m_head = node;
            }
        } else if (node->frequency > 1) {
            // Earlier the frequency was 1, but now it has exceeded. So, it cannot stay in this chain
            if (node->left && node->right) {
                // node in the middle
                node->left->right = node->right;
                node->right->left = node->left;
            } else if (!node->left && node->right) {
                m_tail = node->right;
                m_tail->left = nullptr;
                node->right = nullptr;
            } else if (node->left && !node->right) {
                m_head = node->left;
                m_head->right = nullptr;
                node->left = nullptr;
            } else {
                // node is the only element
                m_tail = nullptr;
                m_head = nullptr;
            }
        }
    }

    void updateIndividualElement(int element) {
        Node* node;
        if (m_state.count(element)) {
            node = m_state[element];
            node->frequency++;
        } else {
            node = new Node(element);
            m_state[element] = node;
        }

        updateDLL(node);

        // Update firstUniqueNumber
        if (firstUniqueNumber == -1 && node->frequency == 1) {
            firstUniqueNumber = element;
        } else if (firstUniqueNumber == element && node->frequency > 1) {
            firstUniqueNumber = m_head ? m_head->key : -1;
        }
    }

public:
    FirstUnique(vector<int> nums) {
        for (int element : nums) {
            updateIndividualElement(element);
        }
    }
    
    int showFirstUnique() {
        return firstUniqueNumber;
    }
    
    void add(int value) {
        updateIndividualElement(value);
    }
};


int main() {
    FirstUnique sol ({2,3,5});
    std::cout << sol.showFirstUnique() << std::endl;
    sol.add(5);
    std::cout << sol.showFirstUnique() << std::endl;
    sol.add(2);
    std::cout << sol.showFirstUnique() << std::endl;
    sol.add(3);
    std::cout << sol.showFirstUnique() << std::endl;
    sol.add(6);
    sol.add(7);
    sol.add(8);
    sol.add(6);
    std::cout << sol.showFirstUnique() << std::endl;
}
