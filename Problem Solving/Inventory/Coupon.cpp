#include <iostream>
using namespace std;

struct Item {
    int id;
    string name;
    double price;
    Item(int _id, string _name, double _price) : id(_id), name(_name), price(_price) {}
};

struct Coupon {
    std::vector<string> category;
    int minimumTotalItemRequirement;
    double minimumPriceRequirement;
    double percentDiscount;
    double flatDiscount;
    Coupon(std::vector<string> c, int minimumTotalItem, double minimumPrice, double p, double f) :
    category(c), minimumTotalItemRequirement(minimumTotalItem), minimumPriceRequirement(minimumPrice), 
    percentDiscount(p), flatDiscount(f) {}
};

/**
 * https://prachub.com/interview-questions/design-a-coupon-pricing-engine
 */
double applyCoupon(vector<Item> cart, Coupon coupon) {
    std::unordered_map<string, std::pair<int, double>> groupCart;
    double totalPrice = 0;
    for (Item item : cart) {
        if (groupCart.count(item.name)) {
            std::pair<int, double>& totalItemsAndTotalPrice = groupCart[item.name];
            totalItemsAndTotalPrice.first++;
            totalItemsAndTotalPrice.second += item.price;
        } else {
            groupCart[item.name] = {1, item.price};
        }
        totalPrice += item.price;
    }

    std::vector<string> discountedCategories = coupon.category;
    double totalPriceOfCombinedItems = 0;
    int totalCombinedItems = 0;
    for (string category : discountedCategories) {
        if (groupCart.count(category)) {
            std::pair<int, double>& totalItemsAndTotalPrice = groupCart[category];
            totalPriceOfCombinedItems += totalItemsAndTotalPrice.second;
            totalCombinedItems += totalItemsAndTotalPrice.first;
        }
    }

    if (totalCombinedItems >= coupon.minimumTotalItemRequirement && totalPriceOfCombinedItems >= coupon.minimumPriceRequirement) {
        // It fits the rule
        double discount = std::min(totalPriceOfCombinedItems, (totalPriceOfCombinedItems * coupon.percentDiscount)/100 + coupon.flatDiscount);
        totalPrice -= discount;
    }

    return totalPrice;
}


int main() {

    // Test 1: Simple — basic valid coupon, single category
    // Clothing($50+$30=$80), 10% off → $72, $5 flat → $67
    // Final = Electronics($500) + Food($20) + $67 = $587
    {
        vector<Item> cart = {
            {1, "ELECTRONICS", 500.0},
            {2, "CLOTHING",     50.0},
            {3, "CLOTHING",     30.0},
            {4, "FOOD",         20.0}
        };
        Coupon coupon = {{"CLOTHING"}, 2, 50.0, 10.0, 5.0};
        std::cout << applyCoupon(cart, coupon) << std::endl; // expected: 587.0
    }

    // Test 2: Complicated — multi-category coupon
    // Eligible = Clothing($50+$30) + Food($20) = $100, count=3 → meets minimums
    // 20% off → $80, $10 flat → $70
    // Final = Electronics($500) + $70 = $570
    {
        vector<Item> cart = {
            {1, "ELECTRONICS", 500.0},
            {2, "CLOTHING",     50.0},
            {3, "CLOTHING",     30.0},
            {4, "FOOD",         20.0}
        };
        Coupon coupon = {{"CLOTHING", "FOOD"}, 3, 80.0, 20.0, 10.0};
        std::cout << applyCoupon(cart, coupon) << std::endl; // expected: 570.0
    }

    // Test 3: Edge case — valueDiscount exceeds eligible subtotal, floor at $0
    // Clothing($10), 0% off → $10, $50 flat off → floor at $0
    // Final = Electronics($500) + $0 = $500
    {
        vector<Item> cart = {
            {1, "ELECTRONICS", 500.0},
            {2, "CLOTHING",     10.0}
        };
        Coupon coupon = {{"CLOTHING"}, 1, 0.0, 0.0, 50.0};
        std::cout << applyCoupon(cart, coupon) << std::endl; // expected: 500.0
    }

    return 0;
}
