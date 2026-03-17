#include <iostream>
#include <unordered_set>
using namespace std;

/**
 * Find the triplets of the sides of the triange such that (side1 + side2 > Side3) where Side3 is the largest of them.
 * 
 * 1. Sort the sides of the triangle.
 * 2. Create a fixed pointer at the last index of the array (As that would be largest side)
 * 3. Place left pointer at the starting index (Side 1 which is the minimum) and right pointer at the index just less than the fixed pointer
 * 4. Use the logic (side1 + side2 > Side3). If satisfied, push the triplet to the result. 
 * 5. Remember, if left pointer + right pointer > fixed pointer, all the elements between left and right pointer will also satisfy the condition.
 * 6. Since the value of side1 + side2 is greater than side3, we will decrement the right pointer(Side2) as per the two-sum technique.
 * 7. If the logic is not satisfied and side1 + side2 < side3, we need a bigger number, hence increment the left pointer (Side1) as per the two-sum technique.
 */
std::vector<std::vector<int>> getTripletsOfSides(std::vector<int>& sides)
{
    std::sort(sides.begin(), sides.end());
    std::vector<std::vector<int>> results;
    
    for (int fixedPointer = sides.size() - 1; fixedPointer >= 0; fixedPointer--)
    {
        // At this point, we know that fixed pointer will always be maximum
        int firstPointer = 0; // this will always be minimum
        int secondPointer = fixedPointer - 1;
        while (secondPointer > firstPointer)
        {
            if ((sides.at(firstPointer) +  sides.at(secondPointer)) > sides.at(fixedPointer))
            {
                // Since the above condition is satisfied, all the elements between first pointer and second pointer
                // will also satify the conditon.
                for (int i = firstPointer; i < secondPointer; i++) {
                    results.push_back({sides.at(i), sides.at(secondPointer), sides.at(fixedPointer)});
                }
                // As per two point sum technique, if the value is greater, we will look for a smaller number and 
                // hence decrement the second pointer
                --secondPointer;
            } 
            else 
            {
                // As per two point sum technique, if the value is lesser, we will look for a bigger number and 
                // hence increment the first pointer
                ++firstPointer;
            }   
        }
    }
   
    return results;
}

int main()
{
    std::vector<int> sides {4,6,9,11,15,18};
    for (const auto& list : getTripletsOfSides(sides)) {
        std::cout << "{ " << list[0] << ", " << list[1] << ", " << list[2] << " }\n";
    }
}