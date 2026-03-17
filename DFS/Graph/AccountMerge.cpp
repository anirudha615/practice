#include <iostream>
#include <unordered_set>
#include <set>
using namespace std;

void dfsHelper(std::unordered_map<string, std::vector<string>>& emailAdjacencyList, string email, std::set<string>& sortedEmailList, std::unordered_set<string>& visited_email) {

    // Process the email
    visited_email.insert(email);
    sortedEmailList.insert(email);

    // Explore the neighbors
    for (string linkedEmail : emailAdjacencyList[email]) {
        if (!visited_email.count(linkedEmail)) {
            dfsHelper(emailAdjacencyList, linkedEmail, sortedEmailList, visited_email);
        }
    }
}

/**
 * https://www.hellointerview.com/community/questions/accounts-merge/cm5eguhac02kf838ox3fg55a5?level=SENIOR
 * 
 * 1. Create an adjancency graph of Email to other Email.
 *     - Keep 1 primary email and attach bi-directionally other emails to the primary email.
 * 
 * 2. For every keyValue, do a DFS - One set of connected components will be completed DFS'ed - store it in the result.
 */
vector<vector<string>> accountsMerge(vector<vector<string>>& accounts) {
    std::unordered_map<string, std::vector<string>> emailAdjacencyList;
    std::unordered_map<string, string> emailToName;

    // Build adjancency graph
    for (std::vector<string> account : accounts) {
        string accountName = account.at(0);
        string primaryEmail = account.at(1);
        emailToName[primaryEmail] = accountName;
        if (account.size() < 3) {
            emailAdjacencyList[primaryEmail] = {};
            continue;
        }
        for (int index = 2; index < account.size(); index++) {
            string email = account.at(index);
            emailAdjacencyList[primaryEmail].push_back(email);
            emailAdjacencyList[email].push_back(primaryEmail);
            emailToName[email] = accountName;
        }
    }

    std::vector<std::vector<string>> results;
    std::unordered_set<string> visited_email;
    for (std::pair<string, std::vector<string>> keyValue : emailAdjacencyList) {
        if (!visited_email.count(keyValue.first)) {
            // New Connected component - DFS it                
            std::set<string> sortedEmailList;
            dfsHelper(emailAdjacencyList, keyValue.first, sortedEmailList, visited_email);

            // Once the connected component is DFSed, add to the results and start another one.
            std::vector<string> account {emailToName[keyValue.first]};
            account.insert(account.end(), sortedEmailList.begin(), sortedEmailList.end());
            results.push_back(account);
        }
    }

    return results;
}

int main() {
    vector<vector<string>> accounts = {
        {"David", "David0@m.co", "David1@m.co"},
        {"David", "David3@m.co", "David4@m.co"},
        {"David", "David4@m.co", "David5@m.co"},
        {"David", "David2@m.co", "David3@m.co"},
        {"David", "David1@m.co", "David2@m.co"},
        {"David", "David10@m.co"}
    };

    vector<vector<string>> results = accountsMerge(accounts);

    for (const auto& account : results) {
        cout << "[";
        for (int i = 0; i < account.size(); i++) {
            cout << account[i];
            if (i < account.size() - 1) cout << ", ";
        }
        cout << "]" << endl;
    }

    return 0;
}
