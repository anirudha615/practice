#include <iostream>
#include <map>
#include <stack>

using namespace std;

std::string decodeString(string encodedString) {
    
    std::string decodedString;
    std::stack<std::string> stack;
    std::stack<int> numberStack;
    bool isNested = false;
    string currentNumber;

    for (char c : encodedString) {
        // Step 1: Find a closing bracket
        if (c == ']') {
            // Step 2: Keep on popping till you find the opening bracket and prepare character by appening the top at the left
            string character;
            while (stack.top() != "[") {
                character = stack.top() + character;
                stack.pop();
            }

            // STEP 3: Pop out the opening bracket and K value from number stack
            stack.pop(); 
            int k = numberStack.top(); // Extract the K value
            numberStack.pop();

            // STEP 4: Decode the K[encodedString]
            std::string tempDecodedString;
            for (int i = 0; i < k; i++) {
                tempDecodedString += character;
            }

            // Push the decoded string only if the stack is not empty (it is nested)
            if (!stack.empty()) {
                stack.push(tempDecodedString);
            } else {
                // If the stack is empty, append to the decodedString
                decodedString += tempDecodedString;
            }
        } else {
            // The digit in string literal could be 100.
            // Hence, this will collect those string. Once we encounter '[', we push the number to number stack.
            if (isdigit(c)) {
                currentNumber += c;
                continue;
            } else if (c == '[') {
                numberStack.push(std::stoi(currentNumber));
                stack.push({c});
                currentNumber.clear();
            } else if (stack.empty()) {
                // If the stack is empty, just append to the decodedString
                decodedString += c;
            } else {
                stack.push({c});
            }
        }
    }

    return decodedString;
}

int main() {
    string encodedString = "3[z]2[2[y]pq4[2[jk]e1[f]]]ef";
    std::cout << decodeString(encodedString) << std::endl;
}