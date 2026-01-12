#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
using namespace std;

/**
 * 1. Create a Route Adjacency List with bus number too.
 * 2. Use BFS to find the shortest distance between 2 nodes.
 * 
 * Noting the bus number is helpful so that when we move to your neighbors (routes) 
 * and you need to change the bus, you know about it and increment the busChanged Value
 */
int numBusesToDestinationv2(vector<vector<int>>& BusRouteVector, int sourceRoute, int targetRoute) {
    std::unordered_map<int, std::vector<std::pair<int, int>>> routeAdjacencyList;
    std::unordered_map<int, int> visitedRoute;
    std::queue<std::tuple<int, int, int>> bfs_queue;
    
    /**
     * Prepare the route adjacency list - 
     *    a. First Stop will have an edge to the Next Stop only
     *    b. Middle Stop will have edges to previous and next stop 
     *    c. Last Stop will have edge to previous stop only.
     * 
     * No edge if there is single bus route.
     */
    for (int bus = 0; bus < BusRouteVector.size(); bus++) {
        for (int route = 0; route < BusRouteVector[bus].size(); route++) {
            int routeNumber = BusRouteVector[bus][route];
            if (BusRouteVector[bus].size() == 1) {
                routeAdjacencyList[routeNumber] = {};
            } else if (route == 0) {
                routeAdjacencyList[routeNumber].push_back({BusRouteVector[bus][route + 1], bus});
            } else if (route == BusRouteVector[bus].size() - 1) {
                routeAdjacencyList[routeNumber].push_back({BusRouteVector[bus][route - 1], bus});
            } else {
                routeAdjacencyList[routeNumber].push_back({BusRouteVector[bus][route + 1], bus});
                routeAdjacencyList[routeNumber].push_back({BusRouteVector[bus][route - 1], bus});
            }

            if (routeNumber == sourceRoute) {
                bfs_queue.push({routeNumber, bus, 1});
            }
        }
    }

    while (!bfs_queue.empty()) {
        int levelSize = bfs_queue.size();
        for (int level = 0; level < levelSize; level++) {
            
            // Extract the route.
            std::tuple<int, int, int> route = bfs_queue.front();
            bfs_queue.pop();
            int routeNumber = std::get<0>(route);
            int currentBusNumber = std::get<1>(route);
            int totalBusesTaken = std::get<2>(route);

            // Mark the route as visited
            visitedRoute[routeNumber] = 1;

            if (routeNumber == targetRoute) {
                return totalBusesTaken;
            }

            // Add its neighbors
            std::vector<std::pair<int, int>> edges = routeAdjacencyList.at(routeNumber);
            for (auto edge : edges) {
                if (!visitedRoute.count(edge.first)) {
                    // If the bus number changed between neighbor and current node, increment totalBusesTakenYet else do not.
                    int totalBusesTakenYet = totalBusesTaken;
                    if (edge.second != currentBusNumber) {
                        totalBusesTakenYet++;  
                    }
                    bfs_queue.push({edge.first, edge.second, totalBusesTakenYet});
                }
            }
        }
    }

    return -1;
}


int main() {
    vector<vector<int>> BusRouteVector = {{24}, {3,6,11,14,22}, {1,23,24}, {0,6,14}, {1,3,8,11,20}};
    std::cout << numBusesToDestinationv2(BusRouteVector, 20, 8) << std::endl;
}