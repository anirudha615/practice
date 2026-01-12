#include <iostream>
#include <chrono>
#include <unordered_map>
#include <set>

using namespace std;

struct Package {
public:
    int16_t package_id;
    int32_t package_size;
    bool isRetrieved = false;

    Package(int16_t id, int32_t size) 
        : package_id(id), package_size(size) {}
};

class Locker {
public:
    int16_t locker_id;
    int32_t remaining_locker_size;
    
    Locker(int16_t id, int32_t size) 
        : locker_id(id), remaining_locker_size(size) {}

    bool operator<(const Locker& other) const {
        return remaining_locker_size < other.remaining_locker_size;
    }
};

/**
 * Every RBT must follow these five rules:
 *    1. Every node is red or black.
 *    2. The root is black.
 *    3. All leaves (nulls) are black.
 *    4. Red nodes cannot have red children (no two reds in a row).
 *    5. Every path from a node to its leaves must have the same number of black nodes (black-height).
 * 
 * Types of Rotations:
 *   1. Left-Left (LL)	↙ ↙	Right Rotation at grandparent
 *   2. Right-Right (RR)	↘ ↘	Left Rotation at grandparent
 *   3. Left-Right (LR)	↙ ↘	Left at parent, then Right at grandparent
 *   4. Right-Left (RL)	↘ ↙	Right at parent, then Left at grandparent
 * 
 * When does Recoloring occures:
 *  1. Red parent, red uncle	-->Recolor parent, uncle → black; grandparent → red
 *  2. Red parent, black or null uncle	--> Requires rotation + recolor
 * 
 * If the RBT rules are broken, then use recoloring and rotation to fix it.
 * 
 * 
 * The difference between AVL Tree and RBT is the former will rotate if the height difference 
 * of left and right sub-tree is just 1.
 * 
 * AVL --> Since it has to re-balance quite often, look-ups are faster but inserts are slower 
 *         Useful when it is read-heavy and not frequently written for best fit problem.
 * RBT --> Since it has relax re-balance, look-ups are slower than AVL but inserts are faster
 *         Useful when it is write-heavy for best fit problem.
 */


class LockerSystem 
{
private:
  
    std::set<Locker> lockers; // Use Red-Black Tree (Balanced BST) to find best fit locker whose size is just >= package size.
    std::vector<Package> packages; // packages database
    std::unordered_map<int16_t, int16_t> package_locker_tracking; // can be persisted in database 

public:
    LockerSystem(const std::set<Locker>& lockerList) {
        lockers = lockerList;
    }

    void updateLockerBST(Locker lockerToBeUpdated, int32_t package_size, bool add)
    {
        // Erasing may require Red Black Tree to rotate or make itself balanced
        // Worst case is O(LogN) 
        // Best case no rotation required hence O(1)
        lockers.erase(lockerToBeUpdated);

        if (add)
        {
            lockerToBeUpdated.remaining_locker_size += package_size;
        } else 
        {
            lockerToBeUpdated.remaining_locker_size -= package_size;
        }
        

        // Inserting may require Red Black Tree to rotate or make itself balanced
        // Worst case is O(LogN) as re-coloring and rotation required
        // Best case no rotation required, only recoloring or no recoloring - hence O(1)
        lockers.insert(lockerToBeUpdated);
    }

    void print_lockers() 
    {
        for (const auto& locker : lockers) 
        {
            std::cout << "  Locker ID: " << locker.locker_id << ", Remaining Space: " << locker.remaining_locker_size << '\n';
        }
    }

    Locker getAvailableLocker(Package package)
    {
        Locker dummy(-1, package.package_size); // create dummy locker to use lower_bound
        
        // Searches the tree in O(Log N) just like Binary Tree.
        auto it = lockers.lower_bound(dummy);

        if (it == lockers.end()) {
            return Locker(-1, -1); // Return invalid locker
        }

        // Update Locker
        Locker locker_found = *it;
        updateLockerBST(locker_found, package.package_size, false);

        // Update package database and include it in the tracking
        packages.push_back(package);
        package_locker_tracking[package.package_id] = locker_found.locker_id;

        return locker_found;
    }

    bool retrievePackageFromLocker(int16_t package_id)
    {
        // Find the package
        auto package_it = std::find_if(packages.begin(), packages.end(), [package_id](const Package& p) {
            return p.package_id == package_id;
        });

        if (package_it == packages.end()) {
            return false; // Package not found
        }

        // Retrieve the locker from tracking map
        int16_t locker_id = package_locker_tracking[package_id];

        // Find the locker
        auto locker_it = std::find_if(lockers.begin(), lockers.end(),
                       [locker_id](const Locker& l) { return l.locker_id == locker_id; });

        if (locker_it == lockers.end()) {
            return false; // Locker not found
        }

        // Update package object and remove it from tracking
        package_it->isRetrieved = true;
        package_locker_tracking.erase(package_id);

        // Update the locker
        updateLockerBST(*locker_it, package_it->package_size, true);

        return true;
    }
};

int main() {
    std::set<Locker> lockers = {Locker(1, 10), Locker(2, 20), Locker(3, 30), Locker(4, 40)};
    LockerSystem lockerSystem(lockers);

    Package package1(1, 9);

    Locker locker = lockerSystem.getAvailableLocker(package1);
    if (locker.locker_id != -1) {
        std::cout << "Package 1 assigned to Locker: " << locker.locker_id << std::endl;
    } else {
        std::cout << "No available locker for Package 1\n";
    }

    lockerSystem.print_lockers();

    Package package2(2, 1);
    locker = lockerSystem.getAvailableLocker(package2);
    if (locker.locker_id != -1) {
        std::cout << "Package 2 assigned to Locker: " << locker.locker_id << std::endl;
    } else {
        std::cout << "No available locker for Package 2\n";
    }

    lockerSystem.print_lockers();

    Package package3(3, 35);
    locker = lockerSystem.getAvailableLocker(package3);
    if (locker.locker_id != -1) {
        std::cout << "Package 3 assigned to Locker: " << locker.locker_id << std::endl;
    } else {
        std::cout << "No available locker for Package 3\n";
    }

    lockerSystem.print_lockers();

    Package package4(4, 45);
    locker = lockerSystem.getAvailableLocker(package4);
    if (locker.locker_id != -1) {
        std::cout << "Package 4 assigned to Locker: " << locker.locker_id << std::endl;
    } else {
        std::cout << "No available locker for Package 4\n";
    }

    lockerSystem.print_lockers();

    bool retrieved = lockerSystem.retrievePackageFromLocker(package1.package_id);
    std::cout << "Package 1 retrieved: " << (retrieved ? "Yes" : "No") << std::endl;

    lockerSystem.print_lockers();

    retrieved = lockerSystem.retrievePackageFromLocker(package2.package_id);
    std::cout << "Package 2 retrieved: " << (retrieved ? "Yes" : "No") << std::endl;

    lockerSystem.print_lockers();

    return 0;
}


/**
 * Complexity 0 : 1 package per locker (Shoud be good to use a queue)
 * 
 * // Give Directions to Heap (Check if we is able to bring out pain points for Heap vs BST)
 * // Heap would require to pop out every element till finding the best fit and then we have to reinsert everything 
 * Complexity 1: Multiple packages per locker (Balanced BST)
 *                
 */
