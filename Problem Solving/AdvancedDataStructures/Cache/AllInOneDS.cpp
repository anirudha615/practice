#include <iostream>
#include <set>
using namespace std;

struct Node {
    string key;
    int frequency;
    Node* left = nullptr;
    Node* right = nullptr;
    Node(string k) : key(k), frequency(1), left(nullptr), right(nullptr) {}
};

class AllOne {
private:
    std::unordered_map<string, Node*> m_state; // Key --> Node Frequency/Value
    std::unordered_map<int, Node*> m_frequencyState; // Frequency --> DLL
    std::set<int> m_activeFreqs; // This is O(LOGN)

    void updateFrequencyState(Node* keyNode, int previousFrequency) {
        int currentFrequency = keyNode->frequency;
        // STEP 1: Remove the Node from previous Frequency
        if (previousFrequency) {
            Node* tail = m_frequencyState[previousFrequency];
            if (keyNode->left && keyNode->right) {
                // Node in Middle
                keyNode->left->right = keyNode->right;
                keyNode->right->left = keyNode->left;
                keyNode->left = nullptr;
                keyNode->right = nullptr;
            } else if (keyNode->left && !keyNode->right) {
                // Node is at far right
                keyNode->left->right = nullptr;
                keyNode->left = nullptr;
            } else if (!keyNode->left && keyNode->right) {
                // Node is at far left
                tail = keyNode->right;
                tail->left = nullptr;
                keyNode->right = nullptr;
                m_frequencyState[previousFrequency] = tail;
            } else if (!keyNode->left && !keyNode->right) {
                // Node is the only element. Let's delete this frequency from the state.
                m_frequencyState.erase(previousFrequency);
                m_activeFreqs.erase(previousFrequency);
            }
        }

        // STEP 2: Add the Node to the current Frequency
        if (currentFrequency > 0) {
            if (m_frequencyState.count(currentFrequency)) {
                // If the frequency state already has some elements, lets create link between them
                Node* tail = m_frequencyState[currentFrequency];
                tail->left = keyNode;
                keyNode->right = tail;
            }
            // Irrespective of frequency state has this current frequency, add the node as tail.
            m_frequencyState[currentFrequency] = keyNode;

            // STEP 3: Update Min/Max Frequency
            m_activeFreqs.insert(currentFrequency);
        }
    }

public:
    AllOne() {}
    
    void inc(string key) {
        Node* keyNode;
        if (m_state.count(key)) {
            keyNode = m_state[key];
            keyNode->frequency++;
        } else {
            keyNode = new Node(key);
            m_state[key] = keyNode;
        }
        updateFrequencyState(keyNode, keyNode->frequency - 1);
    }
    
    void dec(string key) {
        if (!m_state.count(key)) {
            return;
        }
        Node* keyNode = m_state[key];
        keyNode->frequency--;
        if (!keyNode->frequency) {
            m_state.erase(key);
        }
        updateFrequencyState(keyNode, keyNode->frequency + 1);
    }
    
    string getMaxKey() {
        return m_activeFreqs.empty() ? "" : m_frequencyState[*m_activeFreqs.rbegin()]->key;
    }
    
    string getMinKey() {
        return m_activeFreqs.empty() ? "" : m_frequencyState[*m_activeFreqs.begin()]->key;
    }
};

int main() {
    AllOne obj;

    // --- Scenario 1: LeetCode Example ---
    cout << "=== Scenario 1: LeetCode Example ===\n";
    obj.inc("hello");
    obj.inc("hello");
    cout << "min=" << obj.getMinKey() << " max=" << obj.getMaxKey() << "\n"; // min=hello, max=hello

    obj.inc("leet");
    cout << "min=" << obj.getMinKey() << " max=" << obj.getMaxKey() << "\n"; // min=leet, max=hello

    // --- Scenario 2: Multiple Keys ---
    cout << "\n=== Scenario 2: Multiple Keys ===\n";
    AllOne obj2;
    obj2.inc("a");   // a:1
    obj2.inc("b");   // a:1, b:1
    obj2.inc("b");   // a:1, b:2
    obj2.inc("c");   // a:1, b:2, c:1
    obj2.inc("c");   // a:1, b:2, c:2
    obj2.inc("c");   // a:1, b:2, c:3
    cout << "min=" << obj2.getMinKey() << " max=" << obj2.getMaxKey() << "\n"; // min=a, max=c

    obj2.dec("c");   // a:1, b:2, c:2
    cout << "min=" << obj2.getMinKey() << " max=" << obj2.getMaxKey() << "\n"; // min=a, max=b or c

    obj2.dec("a");   // a removed; b:2, c:2
    cout << "min=" << obj2.getMinKey() << " max=" << obj2.getMaxKey() << "\n"; // min=b or c, max=b or c

    // --- Scenario 3: Empty Edge Case ---
    cout << "\n=== Scenario 3: Empty ===\n";
    AllOne obj3;
    cout << "min=\"" << obj3.getMinKey() << "\" max=\"" << obj3.getMaxKey() << "\"\n"; // min="", max=""

    obj3.inc("only");
    cout << "min=" << obj3.getMinKey() << " max=" << obj3.getMaxKey() << "\n"; // min=only, max=only

    obj3.dec("only");
    cout << "min=\"" << obj3.getMinKey() << "\" max=\"" << obj3.getMaxKey() << "\"\n"; // min="", max=""

    return 0;
}
