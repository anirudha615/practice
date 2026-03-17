#include <iostream>

using namespace std;

int main() {
    std::vector<string> serverList {"A", "B", "C"};
    int serverIndex = 0;
    int totalCalls = 10;
    for (int call = 0; call < totalCalls; call++) {
        std::cout << serverList.at(serverIndex) << std::endl;
        serverIndex = (serverIndex + 1) % serverList.size(); // moving forward in a circular array
    }

}