#include <iostream>
#include <vector>
#include <string>
#include <optional>

struct Product {
    int product_id;
    std::string name;
    int quantity;
    std::string location;
    int warehouse_id;

    bool operator<(const Product& other) const 
    {
        return quantity > other.quantity;  // for min heap
    }
};

// Binary search inside a single warehouse inventory
std::optional<Product> binarySearchProduct(const std::vector<Product>& inventory, int product_id) {
    int left = 0, right = inventory.size() - 1;

    while (left <= right) {
        int mid = (left + right) / 2;
        if (inventory[mid].product_id == product_id) {
            return inventory[mid];
        } else if (product_id < inventory[mid].product_id) {
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }
    return std::nullopt; // Not found
}

std::optional<Product> findProductInWarehouses(
    const std::vector<std::vector<Product>>& warehouseGrid,
    int product_id,
    int required_quantity
) {
    std::priority_queue<Product> productList;
    for (int warehouseIndex = 0; warehouseIndex < warehouseGrid.size(); warehouseIndex++) {
        auto inventory = warehouseGrid.at(warehouseIndex);
        // Optimization 1
        if (inventory.front().product_id <= product_id && inventory.back().product_id >= product_id)
        {
            // Candidate with loop through the inventory - hint towards a better approach fo doing binary search 
            auto result = binarySearchProduct(inventory, product_id);
            if (result)
            {
                auto product = *result;
                if (product.quantity >= required_quantity)
                {
                    product.warehouse_id = warehouseIndex+1;
                    productList.push(product);
                }
            }
        }
    }
    return productList.empty() ? std::nullopt : std::make_optional(productList.top());
}

/**
 * Amazon  manages their inventory through a structured matrix system. 
 * In this system, each row represents a warehouse while the columns represent different products sorted by their product IDs.alignas
 * 
 * Based on the input of product_id and required quanity, return the product from the warehouse whose inventory count is just greater than the
 * required quantity
 */
int main() {
    std::vector<std::vector<Product>> grid = {
        // Warehouse 0
        {
            {101, "Laptop", 50, "A12-S03"}, 
            {103, "Mouse", 150, "A12-S03"},
            {104, "Keyboard", 100, "A12-S04"},
            {106, "Webcam", 70, "A12-S04"},
            {110, "Printer", 25, "A13-S01"}
            // many more ... 
        },
        
        // Warehouse 1
        {
            {102, "Headphones", 150, "B05-S02"},
            {103, "Mouse", 80, "B05-S02"},
            {105, "Monitor", 80, "B06-S01"},
            {108, "Speakers", 120, "B06-S03"},
            {109, "Tablet", 45, "B06-S03"}
            // many more ... 
        },
        
        // Warehouse 2
        {
            {101, "Laptop", 25, "C03-S01"},
            {102, "Headphones", 175, "C03-S02"},
            {106, "Webcam", 85, "C03-S03"},
            {107, "External HDD", 60, "C03-S03"}
            // many more ... 
        },
        
        // Warehouse 3
        {
            {104, "Keyboard", 200, "D08-S01"},
            {105, "Monitor", 95, "D08-S02"},
            {107, "External HDD", 150, "D08-S02"},
            {109, "Tablet", 80, "D09-S01"},
            {110, "Printer", 40, "D09-S01"}
            // many more ... 
        },
        
        // Warehouse 4
        {
            {101, "Laptop", 75, "E15-S01"},
            {108, "Speakers", 90, "E15-S02"},
            {109, "Tablet", 120, "E15-S02"},
            {110, "Printer", 55, "E15-S03"}
            // many more ... 
        }
    };
    
    /** Input data */
    int product_id = 109;
    int required_quantity = 5;

    std::optional<Product> result = findProductInWarehouses(grid, product_id, required_quantity);

    if (result.has_value()) 
    {
        Product product = result.value();
        std::cout << "Product found in Warehouse : " << product.warehouse_id << "\n";
        std::cout << "Product ID: " << product.product_id << ":\n";
        std::cout << "Name: " <<  product.name << "\n";
        std::cout << "Quantity Available: " << product.quantity << "\n";
        std::cout << "Product can be found in : " << product.location << "\n";
    } else {
        std::cout << "Product not available in the required quantity.\n";
    }

    return 0;
}
