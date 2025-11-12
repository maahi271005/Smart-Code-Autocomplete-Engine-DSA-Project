#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include "../include/tst.h"
#include "../include/minheap.h"
#include "../include/lru.h"
#include "../include/kmp.h"
#include "../include/stack.h"
#include "../include/graph.h"
#include "../include/freq_store.h"
#include "../include/ranker.h"

class AutocompleteEngine {
private:
    TST tst;
    LRUCache cache;
    FreqStore freqStore;
    CooccurrenceGraph graph;
    Ranker ranker;
    UndoRedoStack undoRedo;
    std::string lastAccepted;
    bool useSubstringSearch;

    void loadSeeds(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()){
            std::cerr << "Warning: Could not open " << filename << std::endl;
            return;
        }

        std::string word;
        int count = 0;
        while (file >> word) {
            if (!word.empty()) {
                tst.insert(word);
                count++;
            }
        }
        file.close();
        std::cout << "Loaded " << count << " tokens from seed file." << std::endl;
    }

    std::vector<std::string> substringSearch(const std::string& prefix){
        std::vector<std::string> allWords;
        tst.getAllWords(allWords);

        std::vector<std::string> results;
        for (const auto& word : allWords) {
            if (KMP::contains(word, prefix)) {
                results.push_back(word);
            }
        }

        return results;
    }


public:
    AutocompleteEngine() 
        : cache(50), 
        freqStore("data/frequencies.txt"), 
        ranker(&freqStore, &graph),
        useSubstringSearch(false){
        
        loadSeeds("data/seeds.txt");
    }

    std::vector<std::pair<std::string, double>> getSuggestions(const std::string& prefix, int k = 5){
        if (prefix.empty()) {
            return std::vector<std::pair<std::string, double>>();
        }

        if (cache.exists(prefix)) {
            auto cached = cache.get(prefix);
            std::vector<std::pair<std::string, double>> result;
            for (const auto& token : cached) {
                result.push_back({token, freqStore.get(token)});
            }
            return result;
        }

        std::vector<std::string> candidates = tst.prefixSearch(prefix, k * 2);

        if (candidates.size()<3 && useSubstringSearch) {
            auto substringResults = substringSearch(prefix);
            candidates.insert(candidates.end(), substringResults.begin(), substringResults.end());
            
            std::sort(candidates.begin(), candidates.end());
            candidates.erase(std::unique(candidates.begin(), candidates.end()), candidates.end());
        }

        ranker.setLastToken(lastAccepted);
        auto ranked = ranker.rankResults(candidates, k);

        std::vector<std::string> toCache;
        for (const auto& [token, score] : ranked){
            toCache.push_back(token);
        }
        cache.put(prefix, toCache);

        return ranked;
    }

    void acceptSuggestion(const std::string& token){
        freqStore.bump(token, 1);
        
        if (!lastAccepted.empty()){
            graph.addEdge(lastAccepted, token);
        }
        
        undoRedo.pushInsert(0, token);
        lastAccepted = token;
        
        std::cout << "Accepted: "<< token << std::endl;
    }

    void bumpToken(const std::string& token) {
        freqStore.bump(token, 5);
        std::cout << "Bumped frequency of '" << token << "' by 5" << std::endl;
    }

    void performUndo() {
        if (undoRedo.canUndo()) {
            auto [pos, token] = undoRedo.undo();
            std::cout << "Undo: Removed '" << token << "'" << std::endl;
        } else {
            std::cout << "Nothing to undo" << std::endl;
        }
    }

    void performRedo() {
        if (undoRedo.canRedo()) {
            auto [pos, token] = undoRedo.redo();
            std::cout << "Redo: Restored '" << token << "'" << std::endl;
        } else {
            std::cout << "Nothing to redo" << std::endl;
        }
    }

    void toggleSubstringSearch() {
        useSubstringSearch = !useSubstringSearch;
        std::cout << "Substring search: " << (useSubstringSearch ? "ON" : "OFF") << std::endl;
    }

    void displayGraph() {
        graph.display();
    }

    void showHelp() {
        std::cout << "\n=== Smart Autocomplete Engine ===" << std::endl;
        std::cout << "\nCommands:" << std::endl;
        std::cout << ":help - Show this help message" << std::endl;
        std::cout << ":exit or :q - Exit the program" << std::endl;
        std::cout << " :bump <token> - Increase frequency of a token" << std::endl;
        std::cout << ":undo - Undo last accepted token" << std::endl;
        std::cout << ":redo - Redo last undone token" << std::endl;
        std::cout << ":toggle_contains- Toggle substring search" << std::endl;
        std::cout << ":graph  - Display co-occurrence graph" << std::endl;
        std::cout << "\nUsage:" << std::endl;
        std::cout << " - Type a prefix to get suggestions" << std::endl;
        std::cout << " - Select by number or type the full token" << std::endl;
        std::cout << std::endl;
    }
};

int main() {
    AutocompleteEngine engine;
    
    std::cout << "\n=== Smart Autocomplete Engine ===" << std::endl;
    std::cout << "Type ':help' for commands" << std::endl;
    std::cout << "Type ':exit' or ':q' to quit\n" << std::endl;

    std::string input;
    
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);

        if (input.empty()) {
            continue;
        }

        if (input == ":exit" || input == ":q") {
            std::cout << "Goodbye!" << std::endl;
            break;
        }

        
        if (input == ":help") {
            engine.showHelp();
            continue;
        }

        if (input == ":undo") {
            engine.performUndo();
            continue;
        }

        if (input == ":redo") {
            engine.performRedo();
            continue;
        }

        if (input == ":toggle_contains") {
            engine.toggleSubstringSearch();
            continue;
        }

        if (input == ":graph") {
            engine.displayGraph();
            continue;
        }

        if (input.substr(0, 6) == ":bump ") {
            std::string token = input.substr(6);
            engine.bumpToken(token);
            continue;
        }

        auto suggestions = engine.getSuggestions(input, 5);

        if (suggestions.empty()) {
            std::cout << "No suggestions found for '" << input << "'" << std::endl;
            continue;
        }

        std::cout << "\nSuggestions:" << std::endl;
        for (size_t i = 0; i < suggestions.size(); i++) {
            std::cout << "  " << (i + 1) << ". " << suggestions[i].first 
                    << " (score=" << suggestions[i].second << ")" << std::endl;
        }

        std::cout << "\nAccept by number or token (or press Enter to skip): ";
        std::string choice;
        std::getline(std::cin, choice);

        if (choice.empty()) {
            continue;
        }

        if (isdigit(choice[0]) && choice.length() == 1) {
            int num = choice[0] - '0';
            if (num >= 1 && num <= suggestions.size()) {
                engine.acceptSuggestion(suggestions[num - 1].first);
            }
            else{
                std::cout << "Invalid selection" << std::endl;
            }
        }
        else{
            bool found = false;
            for (const auto& [token, score] : suggestions) {
                if (token == choice) {
                    engine.acceptSuggestion(token);
                    found = true;
                    break;
                }
            }
            if (!found) {
                std::cout << "Token not in suggestions" << std::endl;
            }
        }

        std::cout << std::endl;
    }

    return 0;
}