#include <iostream>

using namespace std;

/**
 * 1 pointer on abbr and another pointer on word --> String manipulation
 * https://www.hellointerview.com/community/questions/valid-word-abbreviation/cm5eguhab02bq838oi66lqj0r?level=SENIOR
 */
bool validWordAbbreviation(string word, string abbr) {
    if (word.size() < abbr.size()) {
        return false;
    }

    if (abbr == word) {
        return true;
    }
    
    int abbreviationPtr = 0;
    int wordPointer = -1;
    string currentNumber;
    for (abbreviationPtr = 0; abbreviationPtr < abbr.size(); abbreviationPtr++) {
        char abbrChar = abbr.at(abbreviationPtr);
        if (isdigit(abbrChar)) {
            if (currentNumber.empty() && abbrChar == '0') {
                return false;
            }
            currentNumber += abbrChar;
            continue;
        }

        wordPointer = currentNumber.empty() ? ++wordPointer : wordPointer + std::stoi(currentNumber) + 1;
        // validation of incrementing the word pointer
        if (wordPointer >= (int)word.size()) {
            return false;
        }
        char wordChar = word.at(wordPointer);
        if (abbrChar != wordChar) {
            return false;
        }
        currentNumber = "";
    }

    if (!currentNumber.empty()) {
        wordPointer += std::stoi(currentNumber);
    }

    // Happy case where every char of abbr matched the word char and now we are checking the size
    return wordPointer + 1 == (int)word.size();
}

int main() {
    // Test 1: Valid abbreviation — "s10n" matches "substitution"
    bool result1 = validWordAbbreviation("substitution", "s10n");
    cout << "[" << (result1 == true ? "PASS" : "FAIL") << "] "
         << "Valid abbreviation | Expected: true | Got: " << (result1 ? "true" : "false") << "\n";

    // Test 2: Invalid abbreviation — leading zero in "s010n"
    bool result2 = validWordAbbreviation("substitution", "s010n");
    cout << "[" << (result2 == false ? "PASS" : "FAIL") << "] "
         << "Leading zero invalid | Expected: false | Got: " << (result2 ? "true" : "false") << "\n";

    return 0;
}
