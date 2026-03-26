#include <iostream>
using namespace std;

/**
 * https://prachub.com/interview-questions/design-a-nested-transaction-store
 * 
 * Nested Transaction demands a stack 
 * 1. Authoritative Source
 * 2. Stack to keep track of ongoing transactions - similar to SQL.
 */
class NestedTransaction {
    private:
        std::unordered_map<string, int> m_authoritativeSource;
        std::vector<std::unordered_map<string, int>> m_nestedTransaction;
    public:
        void set(string key, int value) {
            if (m_nestedTransaction.empty()) {
                // If there is no transaction in progress, you can simply modify the authoritative source.
                m_authoritativeSource[key] = value;
            } else {
                // If there is transaction in progress (stack is not empty), add key/value to the top of the stack
                std::unordered_map<string, int>& stackTop = m_nestedTransaction[m_nestedTransaction.size() - 1];
                stackTop[key] = value;
            }
        }

        int get(string key) {
            // We need to return the most recent value. Hence, we need to loop through the vector.
            // This is the main reason we didn't proceed with stack and switched to vector.
            for (int index = m_nestedTransaction.size() - 1; index >= 0; --index) {
                if (m_nestedTransaction.at(index).count(key)) {
                    return m_nestedTransaction.at(index)[key];
                }
            }
            // If you came here, that means the key is not present in the ongoing transaction.
            // Check the authoritative source
            if (!m_authoritativeSource.count(key)) {
                return -1;
            }
            return m_authoritativeSource[key];
        }

        void startTransaction(){
            std::unordered_map<string, int> newTransaction;
            newTransaction["BEGIN"] = 1;
            m_nestedTransaction.push_back(newTransaction);
        }

        void endTransaction(string endTransactionType) {
            std::unordered_map<string, int> stackTop = m_nestedTransaction[m_nestedTransaction.size() - 1];
            m_nestedTransaction.pop_back();
            if (!stackTop.count("BEGIN")) {
                std::cout << "Something went wrong!!" << std::endl;

            }
            if (endTransactionType == "APPLY") {
                // If the transaction ends with "APPLY", 
                // pop out the top of the stack, validate if BEGIN exists.
                // Call SET for every K/V -- if the stack is empty, authoritative source will be modified
                // else top of the stack is modified.
                for (std::pair<string, int> keyValue : stackTop) {
                    if (keyValue.first != "BEGIN") {
                        // If the previous transaction already has keys set, they will get overriden as the current
                        // transaction takes precendence.
                        set(keyValue.first, keyValue.second);
                    }
                }
                std::cout << endTransactionType << std::endl;
            } else {
                // If the transactions ends with "DISCARD", don't do anything.
                std::cout << endTransactionType << std::endl;
            }
        }

        void endAllTransaction(string endTransactionType) {
            if (endTransactionType == "APPLY") {
                // If the transaction ends with "APPLY", 
                // loop from bottom of the stack to top to persist information from oldest to newest
                // where newest will automatically gain precendence. 
                // Once traversed, clear all trnasaction in progress.
                int index = 0;
                while (index < m_nestedTransaction.size()) {
                    std::unordered_map<string, int> stackTop = m_nestedTransaction[index];
                    for (std::pair<string, int> keyValue : stackTop) {
                        if (keyValue.first != "BEGIN") {
                            m_authoritativeSource[keyValue.first] = keyValue.second;
                        }
                    }
                    index++;
                }
                m_nestedTransaction.clear();
                std::cout << endTransactionType << std::endl;
            } else {
                // If the transactions ends with "DISCARD", discard the entire stack
                m_nestedTransaction.clear();
                std::cout << endTransactionType << std::endl;
            }
        }
};

int main() {
    NestedTransaction nt;
    nt.set("X", 220);
    nt.set("Y", 320);
    nt.startTransaction();
    nt.startTransaction();
    nt.startTransaction();
    nt.set("X", 10);
    nt.set("Y", 15);
    cout << "[T3] RETURN X (Expected 10): " << nt.get("X") << endl;
    cout << "[T3] RETURN Y (Expected 15): " << nt.get("Y") << endl;
    nt.endTransaction("APPLY");
    nt.startTransaction();
    nt.set("X", 30);
    nt.set("Y", 45);
    cout << "[T4] RETURN X (Expected 30): " << nt.get("X") << endl;
    cout << "[T4] RETURN Y (Expected 45): " << nt.get("Y") << endl;
    nt.endTransaction("APPLY");
    cout << "[T2] RETURN X (Expected 30): " << nt.get("X") << endl;
    cout << "[T2] RETURN Y (Expected 45): " << nt.get("Y") << endl;
    nt.set("X", -2);
    nt.set("Y", -8);
    cout << "[T2] RETURN X (Expected -2): " << nt.get("X") << endl;
    cout << "[T2] RETURN Y (Expected -8): " << nt.get("Y") << endl;
    nt.endTransaction("DISCARD");
    cout << "[T1] RETURN X (Expected 220): " << nt.get("X") << endl;
    cout << "[T1] RETURN Y (Expected 320): " << nt.get("Y") << endl;
    nt.set("X", 80);
    nt.set("Y", 100);
    cout << "[T1] RETURN X (Expected 80): " << nt.get("X") << endl;
    cout << "[T1] RETURN Y (Expected 100): " << nt.get("Y") << endl;
    nt.endTransaction("DISCARD");
    cout << "[T1] RETURN X (Expected 220): " << nt.get("X") << endl;
    cout << "[T1] RETURN Y (Expected 320): " << nt.get("Y") << endl;


    std::cout << "With END ALL" << std::endl;
    nt.set("X", 220);
    nt.set("Y", 320);
    nt.startTransaction();
    nt.startTransaction();
    nt.startTransaction();
    nt.set("X", 10);
    nt.set("Y", 15);
    cout << "[T3] RETURN X (Expected 10): " << nt.get("X") << endl;
    cout << "[T3] RETURN Y (Expected 15): " << nt.get("Y") << endl;
    nt.endTransaction("APPLY");
    nt.set("X", 30);
    cout << "[T4] RETURN X (Expected 30): " << nt.get("X") << endl;
    cout << "[T4] RETURN Y (Expected 15): " << nt.get("Y") << endl;
    nt.endAllTransaction("APPLY");
    cout << "RETURN X (Expected 30): " << nt.get("X") << endl;
    cout << "RETURN Y (Expected 15): " << nt.get("Y") << endl;

    return 0;
}