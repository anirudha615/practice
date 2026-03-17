#include <iostream>
using namespace std;

/**
 * https://www.hellointerview.com/community/questions/can-place-flowers/521cb65b-a0ac-4d26-b1f3-f9e7c8a932c2?level=SENIOR
 * 
 * 1. Check prev and next element that it is = 1. If yes, you cannot place. If no, modify the flowerbed and reduce n
 */
bool canPlaceFlowers(vector<int>& flowerbed, int n) {
    if (!n) {
        return true;
    }
    for (int index = 0; index < flowerbed.size(); index++) {
        int flower = flowerbed.at(index);
        if (flower == 1) {
            continue;
        }

        bool canFlowerBePlaced = true;
        if (index-1>=0) {
            canFlowerBePlaced = !(flowerbed.at(index-1) == 1);
        }
        if (index + 1 < flowerbed.size() && canFlowerBePlaced) {
            canFlowerBePlaced = !(flowerbed.at(index+1) == 1);
        }

        if (canFlowerBePlaced) {
            flowerbed[index] = 1;
            --n; // place a flower
            if (!n) {
                break;
            }
        }
    }

    return !n;
}

int main() {
    std::vector<int> flowerBed {1,0,0,0,1};
    std::cout << canPlaceFlowers(flowerBed, 2) << std::endl;
}