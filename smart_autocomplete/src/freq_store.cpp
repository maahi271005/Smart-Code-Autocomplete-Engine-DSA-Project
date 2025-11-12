#include "../include/freq_store.h"
#include <fstream>
#include <sstream>

FreqStore::FreqStore(const std::string& path) : filePath(path) {
    load();
}

void FreqStore::load() {
    std::ifstream file(filePath);
    
    if (!file.is_open()) {
        return;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string token;
        int freq;
        
        if (iss >> token >> freq) {
            frequencies[token] = freq;
        }
    }
    
    file.close();
}

void FreqStore::save() {
    std::ofstream file(filePath);
    
    if (!file.is_open()) {
        return;
    }
    
    for (const auto& [token, freq] : frequencies) {
        file << token << " " << freq << "\n";
    }
    
    file.close();
}

int FreqStore::get(const std::string& token) {
    auto it = frequencies.find(token);
    return (it != frequencies.end()) ? it->second : 0;
}

void FreqStore::bump(const std::string& token, int amount) {
    frequencies[token] += amount;
    save();
}

void FreqStore::set(const std::string& token, int freq) {
    frequencies[token] = freq;
    save();
}