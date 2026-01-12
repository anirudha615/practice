#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <unordered_set>

/**
 * 1. Create an interface which will display the details of Clothing, Electronics and Books Category (only price, category)
 * 2. Filter out products which matches the category name.
 *      a. Possible Implementation will be looping through the products and matching the category name.
 * 3. Filter out products which is within the price range
 *      a. Possible Implementation will be adding AND operation to the IF statement.
 *      b. Every time a new criteria comes, we do have to modify our business logic. 
 *         Can we make it generic so that any new criteria, we won't be requiring to modify our business logic
 * 4. Add criteria such as prime eligibility and shipping speed
 */


// Forward declaration of visitor
class SearchCriteria;

// Base Interface for all products
class Product {
public:
    std::string category;
    double price;
    bool primeEligible;

    Product(std::string c, double p, bool prime) : category(c), price(p), primeEligible(prime) {}

    virtual void showDetails() = 0;
    virtual bool matches(SearchCriteria& criteria) = 0;
    virtual ~Product() = default;
};

// Visitor Interface for search criteria
class SearchCriteria {
public:
    virtual bool validateMatch(Product& product) = 0;
    virtual ~SearchCriteria() = default;
};

// Concrete class for Clothing
class Clothing : public Product {
public:
    std::string name;

    Clothing(std::string _name, double price, bool prime) : Product("Clothing", price, prime), name(_name) {}

    void showDetails() override {
        std::cout << "Cloth name: " << name << " - $" << price << (primeEligible ? " (Prime Eligible)" : "") << "\n";
    }

    bool matches(SearchCriteria& criteria) override {
        return criteria.validateMatch(*this);
    }
};

// Concrete class for Electronics
class Electronics : public Product {
public:
    std::string name;

    Electronics(std::string _name, double price, bool prime) : Product("Electronics", price, prime), name(_name) {}

    void showDetails() override {
        std::cout << "Electronics Name: " << name << " - $" << price << (primeEligible ? " (Prime Eligible)" : "") << "\n";
    }

    bool matches(SearchCriteria& criteria) override {
        return criteria.validateMatch(*this);
    }
};

// Concrete class for Books
class Books : public Product {
public:
    std::string title;

    Books(std::string _title, double price, bool prime) : Product("Books", price, prime), title(_title) {}

    void showDetails() override {
        std::cout << "Book title: " << title << " - $" << price << (primeEligible ? " (Prime Eligible)" : "") << "\n";
    }

    bool matches(SearchCriteria& criteria) override {
        return criteria.validateMatch(*this);
    }
};

// Concrete Search Criteria: Max Price Filter
class MaxPriceCriteria : public SearchCriteria {
private:
    double maxPrice;
public:
    MaxPriceCriteria(double p) : maxPrice(p) {}

    bool validateMatch(Product& product) override {
        return product.price >= 0 && product.price <= maxPrice;
    }
};

// Concrete Search Criteria: Category Filter
class CategoryCriteria : public SearchCriteria {
private:
    std::string targetCategory;
public:
    CategoryCriteria(std::string category) : targetCategory(category) {}

    bool validateMatch(Product& product) override {
        return product.category == targetCategory;
    }
};

// Concrete Search Criteria: Prime Eligibility Filter
class PrimeEligibilityCriteria : public SearchCriteria {
public:
    bool validateMatch(Product& product) override {
        return product.primeEligible;
    }
};

// Search Function
std::unordered_set<std::shared_ptr<Product>> searchCatalog(
    const std::vector<std::shared_ptr<Product>>& catalog,
    std::vector<SearchCriteria*> criteriaList) 
{    
    std::unordered_set<std::shared_ptr<Product>> results(catalog.begin(), catalog.end());
    for (auto& criteria : criteriaList)
    {
        for (const auto& product : catalog) {
            // Should not expand this IF statement ?
            if (!product->matches(*criteria)) {
                results.erase(product);
            }
        }
    }
    return results;
}

// Example Usage
int main() {
    std::vector<std::shared_ptr<Product>> catalog = {
        std::make_shared<Clothing>("Red Dress", 50.0, true),
        std::make_shared<Electronics>("Smartphone", 699.99, false),
        std::make_shared<Books>("C++ Programming", 39.99, true),
        std::make_shared<Books>("AI Revolution", 29.99, false)
    };

    std::cout << "Full Catalog:\n";
    for (const auto& product : catalog) {
        product->showDetails();
    }

    std::cout << "\nSearching for Products Under $50:\n";
    MaxPriceCriteria priceCriteria(50.0);
    auto filteredByPrice = searchCatalog(catalog, {&priceCriteria});
    for (const auto& product : filteredByPrice) {
        product->showDetails();
    }

    std::cout << "\nSearching for Prime Eligible Products:\n";
    PrimeEligibilityCriteria primeCriteria;
    auto filteredByPrime = searchCatalog(catalog, {&primeCriteria});
    for (const auto& product : filteredByPrime) {
        product->showDetails();
    }

    std::cout << "\nSearching for Books:\n";
    CategoryCriteria bookCriteria("Books");
    auto filteredByCategory = searchCatalog(catalog, {&bookCriteria});
    for (const auto& product : filteredByCategory) {
        product->showDetails();
    }

    std::cout << "\nSearching for Books Under $50 and prime eligible:\n";
    auto filteredByAllCriteria = searchCatalog(catalog, {&bookCriteria, &priceCriteria, &primeCriteria});
    for (const auto& product : filteredByAllCriteria) {
        product->showDetails();
    }

    return 0;
}
