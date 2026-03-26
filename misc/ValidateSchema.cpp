#include <iostream>
#include <unordered_map>
#include <any>
#include <string>
#include <typeindex>
using namespace std;

/**
 * https://prachub.com/interview-questions/implement-a-nested-object-validator
 * 
 * Logic - 
 *    1. Understand 'type_info' which is a metadata about the object datatype stored as ANY.
 *    2. We used 'type_info' via .type to understand whether recursion is required or not.
 *    3. To compare datatype, we used 'type_info' on actual object and casted back the actual object stored in schema
 *       and compared them using 'type_index' (hashable property to compare)
 */
bool validate(unordered_map<string, any> object, unordered_map<string, any> required_style) {
    for (std::pair<string, any> KeyValue : required_style) {
        // Missing key check
        if (!object.count(KeyValue.first)) {
            return false; 
        }

        // Type Check
        if (required_style[KeyValue.first].type() == typeid(unordered_map<string, any>)) {
            // Nested
            unordered_map<string, any> objectNest = any_cast<unordered_map<string, any>>(object[KeyValue.first]);
            unordered_map<string, any> schemaNest = any_cast<unordered_map<string, any>>(required_style[KeyValue.first]);
            if (!validate(objectNest, schemaNest)) {
                return false;
            }
        } else {
            type_index expectedDataTypeInSchema = any_cast<type_index>(required_style[KeyValue.first]);
            type_index actualDataTypeOfObjectValue = type_index(object[KeyValue.first].type());
            if (actualDataTypeOfObjectValue != expectedDataTypeInSchema) {
                return false;
            }
        }
    }
    return true;
}

int main() {

    // Test 1: Simple — all fields valid
    // Expected: true
    {
        unordered_map<string, any> object = {
            {"name",   string("Alice")},
            {"age",    25},
            {"school", string("MIT")}
        };

        unordered_map<string, any> required_style = {
            {"name",   type_index(typeid(string))},
            {"age",    type_index(typeid(int))},
            {"school", type_index(typeid(string))}
        };

        std::cout << validate(object, required_style) << std::endl; // → expected: true
    }

    // Test 2: Complicated — nested object validation
    // Expected: true
    {
        unordered_map<string, any> attributes = {
            {"color",  string("brown")},
            {"height", 165}
        };

        unordered_map<string, any> object = {
            {"name",       string("Alice")},
            {"age",        25},
            {"school",     string("MIT")},
            {"attributes", attributes}
        };

        unordered_map<string, any> attributeSchema = {
            {"color",  type_index(typeid(string))},
            {"height", type_index(typeid(int))}
        };

        unordered_map<string, any> required_style = {
            {"name",       type_index(typeid(string))},
            {"age",        type_index(typeid(int))},
            {"school",     type_index(typeid(string))},
            {"attributes", attributeSchema}             // ← nested schema
        };

        std::cout << validate(object, required_style) << std::endl; // → expected: true
    }

    // Test 3: Edge case — type mismatch deep in nested object
    // age is a string "twenty-five" instead of int
    // Expected: false, "Type mismatch at 'attributes.height': expected int, got string"
    {
        unordered_map<string, any> attributes = {
            {"color",  string("brown")},
            {"height", string("tall")}  // ← wrong type!
        };

        unordered_map<string, any> object = {
            {"name",       string("Alice")},
            {"age",        25},
            {"school",     string("MIT")},
            {"attributes", attributes}
        };

        unordered_map<string, any> attributeSchema = {
            {"color",  type_index(typeid(string))},
            {"height", type_index(typeid(int))}
        };

        unordered_map<string, any> required_style = {
            {"name",       type_index(typeid(string))},
            {"age",        type_index(typeid(int))},
            {"school",     type_index(typeid(string))},
            {"attributes", attributeSchema}
        };

        std::cout << validate(object, required_style) << std::endl; // → expected: false
    }

    return 0;
}