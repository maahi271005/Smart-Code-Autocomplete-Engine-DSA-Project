#include "../include/tst.h"
#include <algorithm>

TST::TST() : root(nullptr) {}

std::shared_ptr<TSTNode> TST::insertUtil(std::shared_ptr<TSTNode> node, 
    const std::string& word, int index) {
    if (node == nullptr) {
        node = std::make_shared<TSTNode>(word[index]);
    }
    
    if (word[index] < node->data) {
        node->left = insertUtil(node->left, word, index);
        } else if (word[index] > node->data) {
        node->right = insertUtil(node->right, word, index);
    } else {
        if (index < word.length() - 1) {
            node->eq = insertUtil(node->eq, word, index + 1);
        } else {
            node->isEndOfString = true;
        }
    }
    
    return node;
}

void TST::insert(const std::string& word) {
    if (word.empty()) return;
    root = insertUtil(root, word, 0);
}

std::shared_ptr<TSTNode> TST::searchPrefix(const std::string& prefix) {
    if (prefix.empty()) return root;
    
    auto node = root;
    int i = 0;
    
    while (node != nullptr && i < prefix.length()) {
        if (prefix[i] < node->data) {
            node = node->left;
        } else if (prefix[i] > node->data) {
            node = node->right;
        } else {
            i++;
            if (i < prefix.length()) {
                node = node->eq;
            }
        }
    }
    
    return (i == prefix.length()) ? node : nullptr;
}

void TST::collectWords(std::shared_ptr<TSTNode> node, 
        std::string prefix, 
        std::vector<std::string>& results) {
    if (node == nullptr) return;
    
    collectWords(node->left, prefix, results);
    
    std::string current = prefix + node->data;
    
    if (node->isEndOfString) {
        results.push_back(current);
    }
    
    collectWords(node->eq, current, results);
    collectWords(node->right, prefix, results);
}

std::vector<std::string> TST::prefixSearch(const std::string& prefix, int k) {
    std::vector<std::string> results;
    
    if (prefix.empty()) {
        getAllWords(results);
        if (results.size() > k) {
            results.resize(k);
        }
        return results;
    }
    
    auto node = searchPrefix(prefix);
    
    if (node == nullptr) {
        return results;
    }
    
    std::string base = prefix.substr(0, prefix.length() - 1);
    
    if (node->isEndOfString) {
        results.push_back(prefix);
    }
    
    collectWords(node->eq, prefix, results);
    
    if (results.size() > k) {
        results.resize(k);
    }
    
    return results;
}

bool TST::search(const std::string& word) {
    if (word.empty()) return false;
    
    auto node = root;
    int i = 0;
    
    while (node != nullptr && i < word.length()) {
        if (word[i] < node->data) {
            node = node->left;
        } else if (word[i] > node->data) {
            node = node->right;
        } else {
            i++;
            if (i < word.length()) {
                node = node->eq;
            }
        }
    }
    
    return (node != nullptr && i == word.length() && node->isEndOfString);
}

void TST::getAllWords(std::vector<std::string>& results) {
    collectWords(root, "", results);
}

