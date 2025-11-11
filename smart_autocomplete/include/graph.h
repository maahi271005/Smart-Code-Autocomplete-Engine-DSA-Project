#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <unordered_map>
#include <map>

class CooccurrenceGraph {
private:
    std::unordered_map<std::string, std::map<std::string, int>> adjacencyList;

public:
    void addEdge(const std::string& from, const std::string& to);
    double getBoost(const std::string& from, const std::string& to);
    void display();
    int getEdgeWeight(const std::string& from, const std::string& to);
};

#endif