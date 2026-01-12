#include <iostream>
#include <string>
#include <vector>

/**
 * 1. Design a pattern where customer clicks on 'Place Order' and the following activities happen synchronously
 *     a. Inventory gets updated
 *     b. Payment gets charged
 *     c. USPS tag is created
 *     d. Warehouse is notified
 * 
 * If any one of the workflow fails, the order is marked as cancelled.
 * 
 * You can also change this to Chain of Responsibility Pattern.
 */

struct Cart {
    std::string itemName;
    int itemCount;
    double itemPrice;

    Cart(const std::string& name, int count, double price) : itemName(name), itemCount(count), itemPrice(price) {}
};

struct CustomerInfo {
    std::string customerName;
    std::string customerAddress;

    CustomerInfo(const std::string& name, const std::string& addr) : customerName(name), customerAddress(addr) {}
};


class Inventory {
public:
    Cart& cart;

    Inventory(Cart& c) : cart(c) {}

    void updateInventory() {
        std::cout << "Inventory updated for " << cart.itemCount << " item(s) of '" << cart.itemName << "'.\n";
    }
};

class Payment {
public:
    Cart& cart;

    Payment(Cart& c) : cart(c) {}

    bool makePayment() {
        std::cout << "Payment of $" << (cart.itemCount * cart.itemPrice) << " is charged for " << cart.itemName << " with a count of '" << cart.itemCount << "' is completed.\n";
        return true;  // Payment is always successful in this example
    }
};

class USPS {
public:
    Cart& cart;
    CustomerInfo& customerInfo;

    USPS(Cart& c, CustomerInfo& ci) : cart(c), customerInfo(ci) {}

    void createTag() {
        std::cout << "USPS tag created for customer: " << customerInfo.customerName << " with address: " << customerInfo.customerAddress 
        << " for " << cart.itemCount << " item(s) of '" << cart.itemName << "\n";
    }
};

class Warehouse {
public:
    USPS& usps;

    Warehouse(USPS& _usps) : usps(_usps) {}

    void placeOrder() {
        std::cout << "Order placed for " << usps.cart.itemCount << " item(s) of '" << usps.cart.itemName << 
        "' to be delivered to customer: " << usps.customerInfo.customerName << " with address: " << usps.customerInfo.customerAddress << "\n";
    }
};



// Command Interface
class Command {
public:
    virtual bool execute() = 0;
    virtual ~Command() = default;
};

class UpdateInventoryCommand : public Command {
private:
    Inventory& inventory;

public:
    UpdateInventoryCommand(Inventory& inv) : inventory(inv) {}

    bool execute() override {
        inventory.updateInventory();
        return true;
    }
};

class MakePaymentCommand : public Command {
private:
    Payment& payment;

public:
    MakePaymentCommand(Payment& p) : payment(p) {}

    bool execute() override {
        return payment.makePayment();
    }
};

class CreateUSPSCommand : public Command {
private:
    USPS& usps;

public:
    CreateUSPSCommand(USPS& u) : usps(u) {}

    bool execute() override {
        usps.createTag();
        return true;
    }
};

class WarehousePlaceOrderCommand : public Command {
    private:
        Warehouse& warehouse;
    
    public:
        WarehousePlaceOrderCommand(Warehouse& w) : warehouse(w) {}
    
        bool execute() override {
            warehouse.placeOrder();
            return true;
        }
    };

// Invoker class
class OrderProcessor {
private:
    std::vector<Command*> commandList;

public:
    void addCommand(Command* command) {
        commandList.push_back(command);
    }

    void process() {
        for (auto& cmd : commandList) {
            if (!cmd->execute()) {
                std::cout << "An error occurred in one of the workflow. Marking the order as cancelled.\n";
                break;  // Stop if any workflow fails
            }
        }
    }

    ~OrderProcessor() {
        for (auto& cmd : commandList) {
            delete cmd;
        }
    }
};

int main() {
    
    // Prepare a cart
    CustomerInfo CustomerInfo("Anirudha Bhaskar", "206 Bell Street, Seattle, WA - 98121");
    Cart cart("Laptop", 2, 1000);

    // Step 1: Prepare objects for the commands
    Inventory inventory(cart);
    Payment payment(cart);
    USPS usps(cart, CustomerInfo);
    Warehouse warehouse(usps);
    
    // Step 2: Create invoker (OrderProcessor) and add the commands
    OrderProcessor orderProcessor;
    orderProcessor.addCommand(new UpdateInventoryCommand(inventory));
    orderProcessor.addCommand(new MakePaymentCommand(payment));
    orderProcessor.addCommand(new CreateUSPSCommand(usps));
    orderProcessor.addCommand(new WarehousePlaceOrderCommand(warehouse));

    // Step 3: Process the order flow sequentially
    orderProcessor.process();

    return 0;
}
