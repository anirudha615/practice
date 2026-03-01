#include <iostream>

using namespace std;

struct Delivery {
    int m_id;
    string m_status;
    long timestamp;
};

/**
 * https://www.hellointerview.com/community/questions/dasher-payout-classes/cmdckvv0i008qad08os5l6gv2?level=SENIOR
 * 
 * Payment is locked when the driver 'accept' multiple concurrent orders
 * Workflow - 
 *    1. At every accepted delivery, we will calculate the potential payment and save it in the cache.
 *    2. If the delivery is completed, we will use the persisted value from the cache else not use it.
 */
double payoutCalculation(std::vector<Delivery> deliveryList) {
    std::unordered_map<int, std::pair<string, double>> m_cache;
    double base_rate = 10;
    int active_count = 0; // calculate payment based on concurrent active orders
    double totalPayout = 0;

    std::sort(deliveryList.begin(), deliveryList.end(), [](const Delivery& deliveryA, const Delivery& deliveryB){
        return deliveryA.timestamp < deliveryB.timestamp; // Ascending
    });

    for (const Delivery& delivery : deliveryList) {
        if (delivery.m_status == "ACCEPTED") {
            active_count++;
            // If active count is 1, just include the base rate
            // IF active count is > 1, we need to include bonus which is 0.25 * concurrent deliveries (active count - 1) + base rate
            double potentialPayment = active_count == 1 ? base_rate : base_rate + (0.25 * (active_count - 1));
            m_cache[delivery.m_id] = {delivery.m_status, potentialPayment};
        } else if (delivery.m_status == "DELIVERED") {
            // If the delivery status is delivered, add the potential amount calculated when the order was accepted
            totalPayout += m_cache[delivery.m_id].second;
            // decrement the active count
            --active_count;
        } else {
            // If the delivery status is cancelled, don't add potential amount and decrement the active count
            --active_count;
        }
    }

    return totalPayout;
}

void runTest(const std::string& testName, std::vector<Delivery> events, double expected) {
    double result = payoutCalculation(events);
    std::string status = (result == expected) ? "PASS" : "FAIL";
    std::cout << "[" << status << "] " << testName
              << " | Expected: $" << expected
              << " | Got: $" << result << "\n";
}

int main() {
    // Test 1: Single delivery, no concurrency → base rate only
    runTest("Single delivery", {
        {1, "ACCEPTED",  1000},
        {1, "DELIVERED", 1030}
    }, 10.0);

    // Test 2: Two sequential deliveries (no overlap) → both get base rate
    runTest("Two sequential deliveries", {
        {1, "ACCEPTED",  1000},
        {1, "DELIVERED", 1030},
        {2, "ACCEPTED",  1100},  // D1 already done
        {2, "DELIVERED", 1130}
    }, 20.0);

    // Test 3: Two overlapping deliveries → D2 gets bonus
    // D2 accepted while D1 active → concurrent = 1 → D2 = 10 + 0.25*1 = 10.25
    runTest("Two overlapping deliveries", {
        {1, "ACCEPTED",  1000},
        {2, "ACCEPTED",  1015},  // D1 still active
        {1, "DELIVERED", 1030},
        {2, "DELIVERED", 1045}
    }, 20.25);  // D1=$10 + D2=$10.25

    // Test 4: Three overlapping deliveries
    // D1: concurrent=0 → $10
    // D2: concurrent=1 → $10.25
    // D3: concurrent=2 → $10.50
    runTest("Three overlapping deliveries", {
        {1, "ACCEPTED",  1000},
        {2, "ACCEPTED",  1010},
        {3, "ACCEPTED",  1020},
        {1, "DELIVERED", 1040},
        {2, "DELIVERED", 1050},
        {3, "DELIVERED", 1060}
    }, 30.75);  // 10 + 10.25 + 10.50

    // Test 5: Cancelled delivery → no payout for D2, D1 still paid
    runTest("Cancelled delivery no payout", {
        {1, "ACCEPTED",   1000},
        {2, "ACCEPTED",   1015},
        {2, "CANCELLED",  1020},  // D2 cancelled
        {1, "DELIVERED",  1030}
    }, 10.0);  // only D1 paid

    // Test 6: Cancelled delivery still affects D3's concurrent count
    // D1: concurrent=0 → $10
    // D2: concurrent=1 → $10.25 (cancelled, not paid)
    // D3: concurrent=2 → $10.50 (D1 + D2 both active at acceptance)
    runTest("Cancelled delivery inflates concurrent count", {
        {1, "ACCEPTED",   1000},
        {2, "ACCEPTED",   1005},
        {3, "ACCEPTED",   1010},  // sees D1 + D2 active → concurrent=2
        {2, "CANCELLED",  1015},
        {1, "DELIVERED",  1030},
        {3, "DELIVERED",  1045}
    }, 20.50);  // D1=$10 + D3=$10.50

    // Test 7: Out-of-order events (validates timestamp sort)
    // Same as Test 3 but events arrive out of order
    runTest("Out-of-order events", {
        {2, "ACCEPTED",  1015},  // arrives first in array but timestamp is later
        {1, "ACCEPTED",  1000},
        {2, "DELIVERED", 1045},
        {1, "DELIVERED", 1030}
    }, 20.25);  // should match Test 3 result

    return 0;
}