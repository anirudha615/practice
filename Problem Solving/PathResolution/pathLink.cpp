#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <algorithm>
#include <stack>

using namespace std;

/**
 * Use stack to store path levels. At every command, check for cycle. If found a symlink, break command and start a new one.
 * Link: https://www.hellointerview.com/community/questions/unix-cd-resolution/cmbskxz3y004n07adimbogre1
 */
class UnixPathResolution {
private:
    std::stack<string> m_stack;
    unordered_map<string, string> symbolicLinks = {
        {"/home/user/shortcut", "/var/log/system"},
        {"/var/log/system", "/home/user/documents"},
        {"/home/user/documents", "/var/link2"},
        {"/var/link2", "/var/log/system"},
        {"/home/user/pikachu", "/home/user/anirudha"}
    };


    std::vector<string> parsePath(const string& currentPath) {
        std::vector<string> pathList;
        int pathLength = 0;
        int currentPointer = 0;
        int previousPointer = 0;
        while (currentPointer < currentPath.length()) {
            if (currentPath.at(currentPointer) == '/') {
                if (pathLength) {
                    string path = currentPath.substr(previousPointer, pathLength);
                    pathList.push_back(path);
                }
                previousPointer = currentPointer + 1;
                pathLength = 0;
            } else {
                pathLength++;
            }
            currentPointer++;
        }
        pathList.push_back(currentPath.substr(previousPointer, pathLength));
        return pathList;
    }

    string getAbsolutePath() {
        string path;
        auto localStackCopy = m_stack;
        while (!localStackCopy.empty()) {
            path = "/" + localStackCopy.top() + path;
            localStackCopy.pop();
        }
        return path;
    }

    // detect cycle using unidirectional graph pattern where you don't have to pass parent
    // For bidirectional, you need to pass parent also
    bool detectCycle(std::set<string> visitedNodes, string absolutePath) {
        if (visitedNodes.count(absolutePath)) {
            return true; //cycle detected
        }
        
        visitedNodes.insert(absolutePath);
        if (symbolicLinks.count(absolutePath)) {
            return detectCycle(visitedNodes, symbolicLinks[absolutePath]);
        }

        return false;
    }
public:

    void addCurrentPath(const string& currentPath) {
        std::vector<string> pathList = parsePath(currentPath);
        for (string path : pathList) {
            if (!path.empty()) {
                m_stack.push(path);
            }
        }
    }

    void executeCommand(const string& commandPath) {
        if (commandPath[0] == '/') {
            m_stack = std::stack<string>();  // clear stack
        }

        bool abortCommandAndExecuteNew = false;
        std::vector<string> pathList = parsePath(commandPath);
        string absolutePath;
        for (string path : pathList) {
            if (path == "." || path.empty()) {
                continue;
            } else if (path == "..") {
                if (!m_stack.empty()) {
                    m_stack.pop();
                }
            } else {
                m_stack.push(path);
            }

            absolutePath = getAbsolutePath();
            if (symbolicLinks.count(absolutePath)) {
                std::set<std::string> visitedNodes;
                if (detectCycle(visitedNodes, absolutePath)) {
                    std::cout << "command : " << commandPath << " is leading to infinite loops. Hence, terminating..." << std::endl;
                    break;
                }
                // No cycle detected but we need to follow symlink
                abortCommandAndExecuteNew = true;
                break;
            }
        }

        if (abortCommandAndExecuteNew) {
            m_stack = std::stack<string>();  // clear stack
            executeCommand(symbolicLinks[absolutePath]);
        } else {
            std::cout << "Absolute Path : " << absolutePath << std::endl;
        }
    }
};

int main() {
    UnixPathResolution resolution;
    
    cout << "=== Unix Path Resolution Test ===" << endl;
    
    // Test 1: Basic relative navigation
    cout << "\n1. Basic relative navigation:" << endl;
    resolution.addCurrentPath("/home/user");
    cout << "   Starting at: /home/user" << endl;
    resolution.executeCommand("photos");
    resolution.executeCommand("../documents");
    resolution.executeCommand("./subfolder");
    
    // Test 2: Absolute path navigation
    cout << "\n2. Absolute path navigation:" << endl;
    resolution.executeCommand("/usr/bin");
    resolution.executeCommand("/var/log");
    
    // Test 3: Symbolic link Cycle detection (valid)
    cout << "\n3. Testing symbolic link following:" << endl;
    resolution.executeCommand("/home/user/shortcut");
    cout << "   The symbolic link chain eventually creates a cycle and should be detected!" << endl;
    
    // Test 4: Navigation from different starting points
    cout << "\n4. Navigation from root:" << endl;
    UnixPathResolution rootTest;
    rootTest.addCurrentPath("/");
    cout << "   Starting at root: /" << endl;
    rootTest.executeCommand("home/user/photos");
    rootTest.executeCommand("../../..");
    
    // Test 5: Edge cases
    cout << "\n5. Edge cases:" << endl;
    UnixPathResolution edgeTest;
    edgeTest.addCurrentPath("/usr");
    edgeTest.executeCommand("../../../../../../../");  // Many levels up
    edgeTest.executeCommand("home/user/documents");
    
    // Test 6: Mixed absolute and relative
    cout << "\n6. Mixed absolute and relative:" << endl;
    UnixPathResolution mixedTest;
    mixedTest.addCurrentPath("/home/user/photos");
    cout << "   Starting at /home/user/photos" << endl;
    mixedTest.executeCommand("../documents");
    mixedTest.executeCommand("/tmp");
    mixedTest.executeCommand("./cache");
    
    cout << "\n=== Path Resolution Test Completed ===" << endl;
    
    return 0;
}
