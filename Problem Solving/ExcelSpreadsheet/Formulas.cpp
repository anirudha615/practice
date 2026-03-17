#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <algorithm>

using namespace std;

/**
 * Logic would be reversing the direction of Forumlas For ex -> C = A+B
 * The graph would look like A->C and B->C 
 * Adj[A]={C}; Inbound[C] = 0 Inbound[A] = 1
 * 
 * 
 * 2 worfklows - Both O(V+E)
 *    1. Updating or Inserting new seed value - (1)Update cache and mark InboundEdges as 0 and 
 *      (2) run DFS starting from that Node to update all its dependents
 *    
 *    2. Updating or Inserting a formula - (1) create local copy of state and update those states, (2) parse the formula
 *      (3) run topological sort to detect cycles (4) perisist the state if no cycle is detected
 *      (5) run DFS on all the dependents (connected + disconnected)
 * 
 * https://www.hellointerview.com/community/questions/spreadsheet-formulas-graph/cmbsl1b4r004x07ad388lxwci
 * 
 */
class ExcelFormula {
private:
    const std::set<char> operators = {'+','-','*','/'};
    std::unordered_map<char, std::vector<char>> m_adjacencyListWithOutboundEdges; // adjacencyList
    std::unordered_map<char, int> m_inBoundEdges;
    std::unordered_map<char, int> m_cache; // pre-computed cache
    unordered_map<char, std::pair<char, std::vector<char>>> m_formulas; // Formula map
    
public:

    int getValue(char key) {
        if (!m_cache.count(key)) {
            return -1;
        }
        return m_cache[key];
    }
    
    /** Complexity is O(V+E) */
    bool setValue(char key, string value){
        try {
            // If the value is seed value, update the cache and initiate adjacency list 
            int integerValue = std::stoi(value);
            prepareSeedValue(key, integerValue); // O(1)
            // Run DFS on the Key to update dependents
            updateDependent(key); //O(V+E)
            return true;
        } catch (const std::invalid_argument& e) {
            // If the value is formula, parse it.
            if (parseFormulas(key, value)) { // O(K + V + E)
                // USE DFS to update all the nodes
                updateDependents(); // O(V+E)
                return true;
            }
            return false;
        }
    }

    void prepareSeedValue(char key, int value) {
        m_cache[key] = value;
        m_inBoundEdges[key] = 0; // inbound Edges for Seed Node is always 0 as we have reversed the direction
    }

    void updateDependent(char startingNode) {
        std::set<char> visitedNodes;
        runDFS(visitedNodes, startingNode);
    }

    bool parseFormulas(char key, string value) {
        auto localCopyInboundEdges = m_inBoundEdges;
        auto localCopyAdjacencyList = m_adjacencyListWithOutboundEdges;
        auto localCopyFormulas = m_formulas;
        auto localCopyCache = m_cache;

        // Parse would be O(K) K = length of value string
        int currentPointer = 1; // First char would be '='
        std::vector<char> operands;
        char operatorInFormula;
        while(currentPointer < value.length()) {
            if (operators.count(value.at(currentPointer))) {
                operatorInFormula = value.at(currentPointer);
            } else {
                char operand = value.at(currentPointer);
                operands.push_back(value.at(currentPointer));
                localCopyAdjacencyList[operand].push_back(key);
            }
            currentPointer++;
        }
        localCopyAdjacencyList[key] = {}; // setup empty list for the formula key
        // there should be some value of inbound edges for formula Keys as we have reversed the edge direction
        localCopyInboundEdges[key] = operands.size(); 
        localCopyFormulas[key] = {operatorInFormula, operands}; // persist the formula

        // Use Topological sort to detect cycles O(V+E)
        if (!detectCycle(localCopyAdjacencyList, localCopyInboundEdges)) {
            m_adjacencyListWithOutboundEdges = localCopyAdjacencyList;
            m_inBoundEdges = localCopyInboundEdges;
            m_formulas = localCopyFormulas;
            return true;
        }
        return false;
    }

    // detecting cycle using topological sort
    bool detectCycle(
        std::unordered_map<char, std::vector<char>> localCopyAdjacencyList, 
        std::unordered_map<char, int> localCopyInboundEdge) {
        std::queue<char> bfs_queue;
        std::vector<char> visited_nodes;

        for (auto edge: localCopyInboundEdge) {
            if (!edge.second) {
                bfs_queue.push(edge.first);
            }
        }

        while (!bfs_queue.empty()) {
            char node = bfs_queue.front();
            visited_nodes.push_back(node);
            bfs_queue.pop();

            for (auto neighbor : localCopyAdjacencyList[node]) {
                localCopyInboundEdge[neighbor]--;
                if (!localCopyInboundEdge[neighbor]) {
                    bfs_queue.push(neighbor);
                }
            }
        }
        return !(visited_nodes.size() == localCopyInboundEdge.size());
    }

    void updateDependents() {
        std::set<char> visitedNodes;
        for (auto node : m_adjacencyListWithOutboundEdges) {
            if (!visitedNodes.count(node.first)) {
                runDFS(visitedNodes, node.first);
            }
        }
    }

    void runDFS(std::set<char>& visitedNodes, char node) {
        // Mark as visited
        visitedNodes.insert(node);

        // Execute the formula of the node
        if (m_formulas.count(node)) {
            auto formula = m_formulas[node];
            switch (formula.first) {
                case '+': 
                {
                    m_cache[node] = m_cache[formula.second[0]] + m_cache[formula.second[1]];
                    break;
                }
                case '-': 
                {
                    m_cache[node] = m_cache[formula.second[0]] - m_cache[formula.second[1]];
                    break;
                }
                case '*': 
                {
                    m_cache[node] = m_cache[formula.second[0]] * m_cache[formula.second[1]];
                    break;
                }
                case '/': 
                {
                    m_cache[node] = m_cache[formula.second[0]] / m_cache[formula.second[1]];
                    break;
                }
            }
        }

        for (auto neighbor : m_adjacencyListWithOutboundEdges[node]) {
            if (!visitedNodes.count(node)) {
                runDFS(visitedNodes, neighbor);
            }
        }
    }
};


int main() {
    ExcelFormula spreadsheet;
    
    cout << "=== Excel Formula Addition Test ===" << endl;
    cout << "Testing with addition operations and dependency propagation" << endl;
    
    // Test 1: Basic seed values
    cout << "\n1. Basic seed values:" << endl;
    spreadsheet.setValue('A', "10");
    spreadsheet.setValue('B', "20");
    cout << "   Set A=10, B=20" << endl;
    
    // Test 2: Simple addition formula
    cout << "\n2. Addition formula:" << endl;
    spreadsheet.setValue('C', "=A+B");
    cout << "   Set C=A+B (should compute C=30)" << endl;
    cout << "   Actual C=" << spreadsheet.getValue('C') << endl;
    
    // Test 3: Chained additions
    cout << "\n3. Addition chain:" << endl;
    spreadsheet.setValue('D', "=C+A");
    spreadsheet.setValue('E', "=D+B");
    cout << "   Set D=C+A, E=D+B (should compute D=40, E=60)" << endl;
    cout << "   Actual D=" << spreadsheet.getValue('D') << ", E=" << spreadsheet.getValue('E') << endl;
    
    // Test 4: Update propagation
    cout << "\n4. Update propagation:" << endl;
    spreadsheet.setValue('A', "5");
    cout << "   Update A=5 (should cascade: C=25, D=30, E=50)" << endl;
    cout << "   After update - A=" << spreadsheet.getValue('A') << ", C=" << spreadsheet.getValue('C') 
         << ", D=" << spreadsheet.getValue('D') << ", E=" << spreadsheet.getValue('E') << endl;
    
    // Test 5: Cycle detection
    cout << "\n5. Cycle detection:" << endl;
    spreadsheet.setValue('X', "=Y+10");
    cout << "   Set X=Y+10" << endl;
    auto result = spreadsheet.setValue('Y', "=X+5");
    cout << "   Attempt Y=X+5 (should detect cycle and reject) Cycle Detected : " << result << endl;
    
    cout << "\n=== Addition Test Completed ===" << endl;
    
    return 0;
}
