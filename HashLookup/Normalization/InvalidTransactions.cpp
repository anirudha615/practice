#include <iostream>
#include <set>
using namespace std;

struct Transaction {
    int counter;
    string name;
    int timestamp;
    int amount;
    string purchaseCity;
};

Transaction convertStringToTransaction(string trans, int counter) {
    Transaction t;
    t.counter = counter;
    size_t start = 0;
    size_t found = trans.find_first_of(",", start);
    if (found != std::string::npos) {
        t.name = trans.substr(start, found-start);
        start = found + 1; //Move to the next index after ","
    }
    found = trans.find_first_of(",", start);
    if (found != std::string::npos) {
        t.timestamp = std::stoi(trans.substr(start, found-start));
        start = found + 1; //Move to the next index after ","
    }
    found = trans.find_first_of(",", start);
    if (found != std::string::npos) {
        t.amount = std::stoi(trans.substr(start, found-start));
        start = found + 1; //Move to the next index after ","
    }
    t.purchaseCity = trans.substr(start, trans.size() - start);
    return t;
}

/**
 * https://www.hellointerview.com/community/questions/invalid-transactions/cm5eguhad02wv838odqhswty0?level=SENIOR
 * 
 * Check both rules and keep an ordered set to mark invalid transactions
 */
vector<string> invalidTransactions(vector<string>& transactions) {
    std::unordered_map<string, std::vector<Transaction>> state;
    std::set<int> invalid;

    for (int index = 0; index < transactions.size(); index++) {
        string currentTrans = transactions.at(index);
        Transaction currentTransaction = convertStringToTransaction(currentTrans, index);
        // Rule --> Amount Exceeds 1000
        if (currentTransaction.amount > 1000) {
            invalid.insert(index);
        }

        // Rule --> Within 60 minutes with same name but different city
        std::vector<Transaction> pastTransactions = state[currentTransaction.name];
        for (Transaction past : pastTransactions) {
            if (std::abs(past.timestamp - currentTransaction.timestamp) <= 60 && past.purchaseCity != currentTransaction.purchaseCity) {
                // Rule broken
                invalid.insert(index);
                invalid.insert(past.counter);
            }
        }
        state[currentTransaction.name].push_back(currentTransaction);
    }

    std::vector<string> results;
    for (int index : invalid) {
        results.push_back(transactions.at(index));
    }
    return results;
}

int main() {
    vector<string> transactions {"bob,627,1973,amsterdam","alex,387,885,bangkok","alex,355,1029,barcelona","alex,587,402,bangkok","chalicefy,973,830,barcelona","alex,932,86,bangkok","bob,188,989,amsterdam"};
    vector<string> results = invalidTransactions(transactions);
    for (string transaction : results) {
        std::cout << transaction << std::endl;
    }
}