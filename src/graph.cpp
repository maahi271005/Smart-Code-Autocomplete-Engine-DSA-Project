#include "../include/graph.h"
#include <iostream>
#include <cmath>

void CooccurrenceGraph::addEdge(const std::string& from, const std::string& to) {
    adjacencyList[from][to]++;
}

double CooccurrenceGraph::getBoost(const std::string& from, const std::string& to) {
    if (adjacencyList.find(from) == adjacencyList.end()) {
        return 0.0;
    }
    
    auto& neighbors = adjacencyList[from];
    
    if (neighbors.find(to) == neighbors.end()) {
        return 0.0;
    }
    
    int weight = neighbors[to];
    return std::log(1 + weight) * 0.5;
}

int CooccurrenceGraph::getEdgeWeight(const std::string& from, const std::string& to) {
    if (adjacencyList.find(from) == adjacencyList.end()) {
        return 0;
    }
    
    auto& neighbors = adjacencyList[from];
    
    if (neighbors.find(to) == neighbors.end()) {
        return 0;
    }
    
    return neighbors[to];
}

void CooccurrenceGraph::display() {
    std::cout << "\n=== Co-occurrence Graph ===" << std::endl;
    
    for (const auto& [from, neighbors] : adjacencyList) {
        std::cout << from << " -> ";
        for (const auto& [to, weight] : neighbors) {
            std::cout << to << "(" << weight << ") ";
        }
        std::cout << std::endl;
    }
}