#include "../include/ranker.h"
#include "../include/minheap.h"
#include <algorithm>

Ranker::Ranker(FreqStore* fs, CooccurrenceGraph* g) 
    : freqStore(fs), graph(g), lastToken("") {}

void Ranker::setLastToken(const std::string& token) {
    lastToken = token;
}

double Ranker::computeScore(const std::string& token){
    double freqScore = freqStore->get(token);
    double graphBoost = 0.0;
    
    if (!lastToken.empty()){
        graphBoost = graph->getBoost(lastToken, token);
    }
    
    return freqScore + graphBoost;
}

std::vector<std::pair<std::string, double>> Ranker::rankResults(
    const std::vector<std::string>& candidates, int k){
    
    if (candidates.empty()){
        return std::vector<std::pair<std::string, double>>();
    }
    
    MinHeap heap(k);
    
    for (const auto& token : candidates){
        double score = computeScore(token);
        heap.insert(score, token);
    }
    
    auto sortedResults = heap.getAll();
    
    std::vector<std::pair<std::string, double>> result;
    for (const auto& [score, token] : sortedResults){
        result.push_back({token, score});
    }
    
    return result;
}