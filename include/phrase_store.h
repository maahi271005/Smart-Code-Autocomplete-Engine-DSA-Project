#ifndef PHRASE_STORE_H
#define PHRASE_STORE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <fstream>

using namespace std;

// Structure to store a learned phrase/snippet
struct Phrase {
    string trigger;     // The prefix that triggers this (e.g., "for")
    string snippet;     // The full text (e.g., "for(i=0;i<n;i++)")
    int useCount;       // Frequency of usage

    Phrase(const string& t, const string& s)
        : trigger(t), snippet(s), useCount(1) {}
};

/**
 * PhraseStore - Stores learned code snippets/phrases
 * Data Structure: Hash Map (unordered_map) + Vector
 *
 * Purpose: Learn complete code patterns user types and suggest them
 * Example: User types "for(i=0;i<n;i++)" after accepting "for"
 *          Next time they type "fo", suggest the complete loop
 *
 * Time Complexity:
 * - addPhrase: O(n) where n = phrases for that trigger
 * - getPhrases: O(n log n) for sorting by frequency
 * - save/load: O(total phrases)
 */
class PhraseStore {
private:
    // Map: trigger word -> list of learned snippets
    unordered_map<string, vector<Phrase>> phrases;
    string filePath;

public:
    PhraseStore(const string& path);

    // Add a new phrase or increment existing one
    void addPhrase(const string& trigger, const string& fullText);

    // Get all phrases for a trigger, sorted by frequency (most used first)
    vector<Phrase> getPhrases(const string& trigger);

    // Get top N phrases for a trigger
    vector<Phrase> getTopPhrases(const string& trigger, int n = 5);

    // Check if exact phrase exists
    bool hasPhrase(const string& trigger, const string& fullText);

    // Save learned phrases to file for persistence
    void save();

    // Load phrases from file
    void load();

    // Get total number of learned phrases
    int getTotalPhrases() const;
};

#endif
