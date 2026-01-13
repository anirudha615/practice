#include <iostream>
#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <cassert>

#include <map>
#include <unordered_set>

using namespace std;

struct generateUUID {
    static inline std::atomic<int64_t> m_counter{0};
    static int64_t getUUID() {
        return ++m_counter;
    }
};

// Account
class Account {
private:
    int64_t m_accountId;
    int64_t m_totalBalance = 0;
    int64_t m_creationTimestamp;
    int64_t m_lastModifiedTimestamp;
    bool m_deleted = false;

    const int64_t MAX_SIZE = 1000;
    std::map<int64_t, int64_t> m_balanceSnapshot; // Sorted by Ascending timestamp
    /**
     * We need this to prevent Map<Timestamp, MaxHeap> to update the same account ID as the complexity is O(NLogN)/
     * With this, we can blindly add it to the MaHeap for the same timestamp and same accountID multiple times.
     * When it is time to call maxHeap, we will just keep popping out until we fill K size of result set.
     */
    std::map<int64_t, int64_t> m_activitySnapshot; // Sorted by Ascending timestamp

    // Clean up the tracking maps if the size exceeds 1000
    void cleanup() {
        if (m_balanceSnapshot.size() > 1000) {
            m_balanceSnapshot.erase(m_balanceSnapshot.begin());
        }
         if (m_activitySnapshot.size() > 1000) {
            m_activitySnapshot.erase(m_activitySnapshot.begin());
        }
    }
public:
    std::mutex m_mtx;
    Account(int64_t accountId, int64_t creationTimestamp) 
    : m_accountId(accountId), m_creationTimestamp(creationTimestamp), m_lastModifiedTimestamp(creationTimestamp) {}

    void depositAmount(int64_t amount, int64_t timestamp) {
        // Update TotalBalance, BalanceSnapshot, ActivitySnapshot and LastModifiedTimestamp
        m_totalBalance += amount;
        m_balanceSnapshot[timestamp] = m_totalBalance;
        m_activitySnapshot[timestamp] += amount;
        m_lastModifiedTimestamp = timestamp;
        cleanup();
    }

    bool payAmount(int64_t amount, int64_t timestamp) {
        if (m_totalBalance < amount) {
            return false;
        }
        // Update TotalBalance, BalanceSnapshot, ActivitySnapshot and LastModifiedTimestamp
        m_totalBalance -= amount;
        m_balanceSnapshot[timestamp] = m_totalBalance;
        m_activitySnapshot[timestamp] += amount;
        m_lastModifiedTimestamp = timestamp;
        cleanup();
        return true;
    }

    bool initiateTransfer(int64_t amount, int64_t timestamp) {
        // Update TotalBalance, BalanceSnapshot and LastModifiedTimestamp
        // No ActivitySnapshot as per requirement
        if (m_totalBalance < amount) {
            return false;
        }
        m_totalBalance -= amount;
        m_balanceSnapshot[timestamp] = m_totalBalance;
        m_lastModifiedTimestamp = timestamp;
        cleanup();
        return true;
    }

    bool acceptTransfer(int64_t amount, int64_t timestamp) {
        // Update TotalBalance, BalanceSnapshot, ActivitySnapshot and LastModifiedTimestamp
        m_totalBalance += amount;
        m_balanceSnapshot[timestamp] = m_totalBalance;
        m_activitySnapshot[timestamp] += amount;
        m_lastModifiedTimestamp = timestamp;
        cleanup();
        return true;
    }

    bool initiateMerge(int64_t timestamp) {
        // Update TotalBalance, BalanceSnapshot, isDeleted and LastModifiedTimestamp
        // No ActivitySnapshot as per requirement
        m_totalBalance = 0;
        m_balanceSnapshot[timestamp] = m_totalBalance;
        m_lastModifiedTimestamp = timestamp;
        m_deleted =true;
        cleanup();
        return true;
    }

    bool acceptMerge(int64_t amount, int64_t timestamp) {
        // Update TotalBalance, BalanceSnapshot, ActivitySnapshot and LastModifiedTimestamp
        m_totalBalance += amount;
        m_balanceSnapshot[timestamp] = m_totalBalance;
        m_activitySnapshot[timestamp] += amount;
        m_lastModifiedTimestamp = timestamp;
        cleanup();
        return true;
    }

    int64_t getTotalBalance() {
        return m_totalBalance;
    }

    int64_t getSnapshotBalance(int64_t timestamp) {
        auto it = m_balanceSnapshot.upper_bound(timestamp); // Timestamp > requiredTimestamp
        if (it == m_balanceSnapshot.begin()) {
            return 0;  // Before any operations
        }
        --it; // Timestamp <= RequiredTimestamp
        return it->second;
    }

    int64_t getSnapshotActivity(int64_t timestamp) {
         if (!m_activitySnapshot.count(timestamp)) {
            return -1;
        }
        return m_activitySnapshot[timestamp];
    }

    int64_t getLastModifiedTimestamp() {
        return m_lastModifiedTimestamp;
    }

    int64_t getAccountId() {
        return m_accountId;
    }
};


struct Comparator{
    bool operator()(const std::pair<int64_t, int64_t> activitySnapshotAccountA, const std::pair<int64_t, int64_t> activitySnapshotAccountB) const {
        if (activitySnapshotAccountA.first == activitySnapshotAccountB.first) {
            return activitySnapshotAccountA.second > activitySnapshotAccountB.second; // Ascending based on accountId
        }
        return activitySnapshotAccountA.first < activitySnapshotAccountB.first; // Descending based on activity
    }
};

class AccountManager{
private:
    const int64_t MAX_SIZE = 1000;
    std::shared_timed_mutex m_sharedStateMtx;
    std::unordered_map<int64_t, std::shared_ptr<Account>> m_accountManagerCache;
    std::unordered_map<int64_t, std::shared_ptr<Account>> m_accountManagerDeletedCache; // For merged and closed accounts
    std::unordered_map<int64_t, std::pair<std::shared_ptr<Account>, int64_t>> m_pendingTransfers;// Pending Transfer
    // Max Heap Prepared for faster pre-compute as per timestamp
    // You can always insert without worrying about its previous entry because we have activity snapshot map
    std::map<int64_t, std::priority_queue<std::pair<int64_t, int64_t>, std::vector<std::pair<int64_t, int64_t>>, Comparator>> m_maxHeapOfAccounts;

    // Clean up the tracking maps if the size exceeds 1000
    void cleanup() {
        if (m_maxHeapOfAccounts.size() > 1000) {
            m_maxHeapOfAccounts.erase(m_maxHeapOfAccounts.begin());
        }
    }

public:

    bool createAccount(int64_t accountId, int64_t creationTimestamp) {
        {
            std::shared_lock<std::shared_timed_mutex> lock(m_sharedStateMtx); // READ
            if (m_accountManagerCache.count(accountId)) {
                std::cout << "Account with id : " << accountId << " already exists." << std::endl;
                return false;
            }
        }

        std::shared_ptr<Account> account = std::make_shared<Account>(accountId, creationTimestamp);
        {
            std::unique_lock<std::shared_timed_mutex> lock(m_sharedStateMtx); // WRITE
            m_accountManagerCache[accountId] = account;
        }
        return true;
    }

    bool depositFunds(int64_t accountId, int64_t amount, int64_t timestamp) {
        std::shared_ptr<Account> account;
        {
            std::shared_lock<std::shared_timed_mutex> lock(m_sharedStateMtx); // READ
            if (!m_accountManagerCache.count(accountId)) {
                std::cout << "Account with id : " << accountId << " does not exists." << std::endl;
                return false;
            }
            account = m_accountManagerCache[accountId];
        }
        bool isOperationSuccessful = false;
        int64_t currentActivityAmount = -1;;
        if (account) {
            std::lock_guard<std::mutex> lock(account->m_mtx);
            account->depositAmount(amount, timestamp);
            currentActivityAmount = account->getSnapshotActivity(timestamp);
            isOperationSuccessful = true;
        }
        if (currentActivityAmount != -1) {
            std::unique_lock<std::shared_timed_mutex> lock(m_sharedStateMtx); // WRITE
            m_maxHeapOfAccounts[timestamp].push(std::make_pair(currentActivityAmount, accountId));
            cleanup();
        }
        return isOperationSuccessful;
    }

    bool payAmount(int64_t accountId, int64_t amount, int64_t timestamp) {
        std::shared_ptr<Account> account;
        {
            std::shared_lock<std::shared_timed_mutex> lock(m_sharedStateMtx); // READ
            if (!m_accountManagerCache.count(accountId)) {
                std::cout << "Account with id : " << accountId << " does not exists." << std::endl;
                return false;
            }
            account = m_accountManagerCache[accountId];
        }
        bool isOperationSuccessful = false;
        int64_t currentActivityAmount = -1;
        if (account) {
            std::lock_guard<std::mutex> lock(account->m_mtx);
            isOperationSuccessful = account->payAmount(amount, timestamp);
            currentActivityAmount = account->getSnapshotActivity(timestamp);
        }
        if (isOperationSuccessful && currentActivityAmount != -1) {
            std::unique_lock<std::shared_timed_mutex> lock(m_sharedStateMtx); // WRITE
            m_maxHeapOfAccounts[timestamp].push(std::make_pair(currentActivityAmount, accountId));
            cleanup();
        }
        return isOperationSuccessful;
    }

    int64_t initiateTwoPhaseTransfer(int64_t sourceAccountId, int64_t dstAccountId, int64_t amount, int64_t timestamp) {
        std::shared_ptr<Account> sourceAccount;
        std::shared_ptr<Account> dstAccount;
        int64_t transferId = -1;
        {
            std::shared_lock<std::shared_timed_mutex> lock(m_sharedStateMtx); // READ
            if (!m_accountManagerCache.count(sourceAccountId) || !m_accountManagerCache.count(dstAccountId)) {
                std::cout << "Account with sourceId : " << sourceAccountId << " or dstId: " << dstAccountId << " does not exists." << std::endl;
                return transferId;
            }
            sourceAccount = m_accountManagerCache[sourceAccountId];
            dstAccount = m_accountManagerCache[dstAccountId];
        }

        // STEP 1: Generate a transferId when the src initiates a transfer and update m_pendingTransfers
        if (sourceAccount && dstAccount) {
            std::lock_guard<std::mutex> lockSrc(sourceAccount->m_mtx);
            if (sourceAccount->initiateTransfer(amount, timestamp)) {
                transferId = generateUUID::getUUID();
                m_pendingTransfers[transferId] = std::make_pair<>(dstAccount, amount);
            }
        }
        return transferId;
    }

    bool acceptTwoPhaseTransfer(int64_t transferId, int64_t timestamp) {
        std::shared_ptr<Account> dstAccount;
        int64_t amountToBeTransferred = -1;
        {
            std::shared_lock<std::shared_timed_mutex> lock(m_sharedStateMtx); // READ
            if (!m_pendingTransfers.count(transferId)) {
                std::cout << "No account for TransferId:  " << transferId << " exists." << std::endl;
                return false;
            }
            dstAccount = m_pendingTransfers[transferId].first;
            amountToBeTransferred = m_pendingTransfers[transferId].second;
            if (dstAccount && !m_accountManagerCache.count(dstAccount->getAccountId())) {
                std::cout << "Account with dstId: " << dstAccount->getAccountId() << " does not exists." << std::endl;
                return false;
            }
        }

        // STEP 1: If the account exists and amount to be transferred >=0, credit the dest
        bool isOperationSuccessfull = false;
        int64_t currentActivityAmount = -1;
        int64_t dstAccountId = 0;
        if (dstAccount && amountToBeTransferred != -1) {
            std::lock_guard<std::mutex> lock(dstAccount->m_mtx);
            isOperationSuccessfull = dstAccount->acceptTransfer(amountToBeTransferred, timestamp);
            dstAccountId = dstAccount->getAccountId();
            currentActivityAmount = dstAccount->getSnapshotActivity(timestamp);
        }

         // STEP 2: Erase from pending transfers and add it to the maxHeap
        if (isOperationSuccessfull && currentActivityAmount != -1) {
            std::unique_lock<std::shared_timed_mutex> lock(m_sharedStateMtx); // WRITE
            m_pendingTransfers.erase(transferId);
            // Initiating a transfer does not increment total activity but accepting a transfer does. So, only dstAccount.
            m_maxHeapOfAccounts[timestamp].push(std::make_pair(currentActivityAmount, dstAccountId));
            cleanup();
        }

        return isOperationSuccessfull;
    }

    bool mergeAccounts(int64_t sourceAccountId, int64_t dstAccountId, int64_t timestamp) {
        std::shared_ptr<Account> sourceAccount;
        std::shared_ptr<Account> dstAccount;
        {
            std::shared_lock<std::shared_timed_mutex> lock(m_sharedStateMtx); // READ
            if (!m_accountManagerCache.count(sourceAccountId) || !m_accountManagerCache.count(dstAccountId)) {
                std::cout << "Account with sourceId : " << sourceAccountId << " or dstId: " << dstAccountId << " does not exists." << std::endl;
                return false;
            }
            sourceAccount = m_accountManagerCache[sourceAccountId];
            dstAccount = m_accountManagerCache[dstAccountId];
        }
        bool isOperationSuccessfull = false;
        int currentActivityAccountDst = -1;
        if (sourceAccount && dstAccount) {
            std::lock(sourceAccount->m_mtx, dstAccount->m_mtx); // takes care of deadlock situations
            std::lock_guard<std::mutex> lockSrc(sourceAccount->m_mtx, std::adopt_lock);
            std::lock_guard<std::mutex> lockDst(dstAccount->m_mtx, std::adopt_lock);
            int amountToBeTransferred = sourceAccount->getTotalBalance();
            if (sourceAccount->initiateMerge(timestamp)) {
                isOperationSuccessfull = dstAccount->acceptMerge(amountToBeTransferred, timestamp);
                currentActivityAccountDst = dstAccount->getSnapshotActivity(timestamp);
            }
        }

        if (isOperationSuccessfull && currentActivityAccountDst != -1) {
            std::unique_lock<std::shared_timed_mutex> lock(m_sharedStateMtx); // WRITE
            m_accountManagerDeletedCache[sourceAccountId] = sourceAccount; // saving historical queries
            m_accountManagerCache.erase(sourceAccountId); // After deleting, you should be able to create another account with same account id
            // Initiating a merge does not increment total activity but accepting a merge does. So, only dstAccount.
            m_maxHeapOfAccounts[timestamp].push(std::make_pair(currentActivityAccountDst, dstAccountId));
            cleanup();
        }
        return isOperationSuccessfull;
    }

    int64_t getBalance(int64_t accountId, int64_t timestamp) {
        std::shared_ptr<Account> account = nullptr;
        int64_t balance = -1;
        {
            std::shared_lock<std::shared_timed_mutex> lock(m_sharedStateMtx); // READ
            if (!m_accountManagerCache.count(accountId)) {
                std::cout << "Account with id : " << accountId << " does not exists or is not active" << std::endl;
                if (!m_accountManagerDeletedCache.count(accountId)) {
                    std::cout << "Account with id : " << accountId << " does not exists at all" << std::endl;
                } else {
                    account = m_accountManagerDeletedCache[accountId];
                }
            } else {
                account = m_accountManagerCache[accountId];
            }
        }
        if (account) {
            std::lock_guard<std::mutex> lock(account->m_mtx);
            balance = account->getSnapshotBalance(timestamp); 
        }
        return balance;
    }

    std::unordered_set<int64_t> getAccountsWithTopKActivity(int maxSizeK, int64_t timestamp) {
        std::unordered_set<int64_t> results;
        std::shared_lock<std::shared_timed_mutex> lock(m_sharedStateMtx); // READ
        if (!m_maxHeapOfAccounts.count(timestamp)) {
            return results;
        }
        auto maxHeap = m_maxHeapOfAccounts[timestamp];
        while (results.size() < maxSizeK && !maxHeap.empty()) {
            std::pair<int64_t, int64_t> topAccount = maxHeap.top();
            maxHeap.pop();
            results.insert(topAccount.second);
        }
        return results;
    }
};

// Test Functions
void testBasicAccountOperations(AccountManager& manager) {
    // Test 1: Account creation
    assert(manager.createAccount(123, 100) == true);
    assert(manager.createAccount(123, 150) == false); // Already exists
    
    // Test 2: Basic deposit
    assert(manager.depositFunds(123, 50, 200) == true);
    assert(manager.getBalance(123, 200) == 50);
    
    // Test 3: Basic payment
    assert(manager.payAmount(123, 20, 250) == true);
    assert(manager.getBalance(123, 250) == 30);
    
    // Test 4: Insufficient funds
    assert(manager.payAmount(123, 100, 300) == false);
    assert(manager.getBalance(123, 300) == 30); // Balance unchanged
    
    // Test 5: Operations on non-existent account
    assert(manager.depositFunds(999, 100, 350) == false);
    assert(manager.payAmount(999, 50, 400) == false);
}

void testHistoricalBalanceQueries(AccountManager& manager) {
    // Create account and perform operations at different times
    assert(manager.createAccount(456, 100) == true);
    assert(manager.depositFunds(456, 100, 200) == true); // Balance = 100
    assert(manager.payAmount(456, 30, 300) == true);     // Balance = 70
    assert(manager.depositFunds(456, 50, 400) == true);  // Balance = 120
    
    // Test historical queries
    assert(manager.getBalance(456, 150) == 0);   // Before any operations
    assert(manager.getBalance(456, 200) == 100); // After first deposit
    assert(manager.getBalance(456, 250) == 100); // Before payment
    assert(manager.getBalance(456, 300) == 70);  // After payment
    assert(manager.getBalance(456, 350) == 70);  // Before second deposit
    assert(manager.getBalance(456, 400) == 120); // After second deposit
    assert(manager.getBalance(456, 500) == 120); // Latest balance
}

void testTwoPhaseTransfers(AccountManager& manager) {
    // Setup accounts
    assert(manager.createAccount(789, 100) == true);
    assert(manager.createAccount(790, 100) == true);
    assert(manager.depositFunds(789, 100, 200) == true);
    
    // Test 1: Valid transfer
    int64_t transferId = manager.initiateTwoPhaseTransfer(789, 790, 60, 300);
    assert(transferId != -1);
    assert(manager.getBalance(789, 300) == 40); // Source debited immediately
    assert(manager.getBalance(790, 300) == 0);  // Destination not credited yet
    
    // Test 2: Accept transfer
    assert(manager.acceptTwoPhaseTransfer(transferId, 400) == true);
    assert(manager.getBalance(790, 400) == 60); // Destination credited
    
    // Test 3: Invalid transfer ID
    assert(manager.acceptTwoPhaseTransfer(99999, 450) == false);
    
    // Test 4: Insufficient funds for transfer
    int64_t failedTransfer = manager.initiateTwoPhaseTransfer(789, 790, 100, 500);
    assert(failedTransfer == -1); // Should fail, only 40 remaining
}

void testMergeOperations(AccountManager& manager) {
    // Setup accounts
    assert(manager.createAccount(555, 100) == true);
    assert(manager.createAccount(556, 100) == true);
    assert(manager.depositFunds(555, 80, 200) == true);
    assert(manager.depositFunds(556, 20, 200) == true);
    
    // Test merge
    assert(manager.mergeAccounts(555, 556, 300) == true);
    assert(manager.getBalance(556, 300) == 100); // 20 + 80 merged
    
    // Test post-merge behavior - account 555 should be inaccessible for new operations
    assert(manager.depositFunds(555, 10, 400) == false);
    
    // Test re-creation after merge
    assert(manager.createAccount(555, 500) == true);
    assert(manager.getBalance(555, 500) == 0); // New account starts with 0 balance
}

void testTopActivityQueries(AccountManager& manager) {
    // Create accounts for activity testing
    assert(manager.createAccount(111, 100) == true);
    assert(manager.createAccount(222, 100) == true);
    assert(manager.createAccount(333, 100) == true);
    
    // Operations at timestamp 500
    assert(manager.depositFunds(111, 100, 500) == true); // Activity = 100
    assert(manager.depositFunds(222, 200, 500) == true); // Activity = 200
    assert(manager.payAmount(111, 50, 500) == true);     // Account 111 activity = 100 + 50 = 150
    
    // Test top activity at timestamp 500
    auto topAccounts = manager.getAccountsWithTopKActivity(2, 500);
    assert(topAccounts.size() == 2);
    assert(topAccounts.count(222) == 1); // Highest activity (200)
    assert(topAccounts.count(111) == 1); // Second highest activity (150)
    
    // Operations at different timestamp 600
    assert(manager.depositFunds(333, 300, 600) == true); // Activity = 300
    
    auto topAccounts600 = manager.getAccountsWithTopKActivity(1, 600);
    assert(topAccounts600.size() == 1);
    assert(topAccounts600.count(333) == 1); // Only account with activity at 600
}

void testEdgeCases(AccountManager& manager) {
    // Test 1: Multiple operations at same timestamp
    assert(manager.createAccount(777, 100) == true);
    assert(manager.depositFunds(777, 100, 700) == true);
    assert(manager.payAmount(777, 30, 700) == true);
    assert(manager.depositFunds(777, 20, 700) == true);
    assert(manager.getBalance(777, 700) == 90); // 100 - 30 + 20
    
    // Test 2: Balance queries for non-existent accounts
    assert(manager.getBalance(9999, 800) == -1);
    
    // Test 3: Top activity with no activity at timestamp
    auto emptyResult = manager.getAccountsWithTopKActivity(5, 999);
    assert(emptyResult.size() == 0);
    
    // Test 4: Operations with zero amounts (edge case)
    assert(manager.depositFunds(777, 0, 800) == true);
    assert(manager.getBalance(777, 800) == 90); // Balance unchanged
}

int main() {
    AccountManager manager;
    
    // Run all test suites
    testBasicAccountOperations(manager);
    testHistoricalBalanceQueries(manager);
    testTwoPhaseTransfers(manager);
    testMergeOperations(manager);
    testTopActivityQueries(manager);
    testEdgeCases(manager);
    
    cout << "All tests passed successfully!" << endl;
    return 0;
}