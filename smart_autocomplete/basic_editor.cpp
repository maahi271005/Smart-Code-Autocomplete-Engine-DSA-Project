// Basic working editor with autocomplete - NO COLORS, JUST WORKS
// Compile: g++ -std=c++17 basic_editor.cpp src/tst.cpp src/phrase_store.cpp src/freq_store.cpp src/ranker.cpp src/graph.cpp src/minheap.cpp src/lru.cpp src/stack.cpp src/kmp.cpp -lncurses -Iinclude -o basic_editor

#include <ncurses.h>
#include <string>
#include <vector>
#include <fstream>

#include "tst.h"
#include "phrase_store.h"
#include "freq_store.h"
#include "ranker.h"
#include "graph.h"

class BasicEditor {
private:
    std::vector<std::string> lines;
    std::vector<std::string> suggestions;
    std::vector<bool> isPhraseFlag;
    int cursorY, cursorX;
    int scrollY;  // Add scroll offset
    bool showingSuggestions;
    int selectedSuggestion;

    TST tst;
    PhraseStore phraseStore;
    FreqStore freqStore;
    CooccurrenceGraph graph;
    Ranker ranker;
    std::string lastAcceptedWord;

public:
    BasicEditor()
        : cursorY(0), cursorX(0), scrollY(0),
          showingSuggestions(false), selectedSuggestion(0),
          phraseStore("data/phrases.txt"),
          freqStore("data/frequency.txt"),
          ranker(&freqStore, &graph) {

        lines.push_back("");
        loadDictionary();
    }

    void loadDictionary() {
        std::ifstream file("data/words.txt");
        if (!file.is_open()) return;

        std::string word;
        while (file >> word) {
            if (!word.empty()) {
                tst.insert(word);
            }
        }
        file.close();
    }

    void run() {
        initscr();
        raw();
        keypad(stdscr, TRUE);
        noecho();
        curs_set(1);

        bool running = true;
        while (running) {
            draw();
            int ch = getch();
            running = handleInput(ch);
        }

        phraseStore.save();
        freqStore.save();
        endwin();
    }

private:
    void draw() {
        clear();

        // Calculate how many lines we can show
        int maxLines = LINES - 3;

        // Draw lines with scrolling
        for (int i = 0; i < maxLines && (scrollY + i) < lines.size(); i++) {
            int lineNum = scrollY + i + 1;
            mvprintw(i, 0, "%3d | %s", lineNum, lines[scrollY + i].c_str());
        }

        // Draw suggestions popup
        if (showingSuggestions && !suggestions.empty()) {
            int displayY = cursorY - scrollY;
            int popupY = (displayY < maxLines - 1) ? displayY + 1 : displayY;
            int popupX = cursorX + 6;

            for (size_t i = 0; i < suggestions.size() && i < 5; i++) {
                int y = popupY + i;
                if (y < maxLines && y >= 0) {
                    if (i == selectedSuggestion) {
                        mvprintw(y, popupX, " > %s", suggestions[i].c_str());
                    } else {
                        mvprintw(y, popupX, "   %s", suggestions[i].c_str());
                    }
                }
            }
        }

        // Status bar
        mvprintw(LINES - 2, 0, "Line %d, Col %d | %d phrases | Tab: Accept | Ctrl+P: Save Phrase | Ctrl+Q: Quit",
                 cursorY + 1, cursorX + 1, phraseStore.getTotalPhrases());

        // Position cursor - calculate display position relative to scroll
        int displayY = cursorY - scrollY;
        int displayX = cursorX + 6;

        // Make sure cursor position is valid
        if (displayY >= 0 && displayY < maxLines && displayX >= 0 && displayX < COLS) {
            move(displayY, displayX);
        }

        refresh();
    }

    void updateScroll() {
        int maxLines = LINES - 3;

        // If cursor is above visible area, scroll up
        if (cursorY < scrollY) {
            scrollY = cursorY;
        }
        // If cursor is below visible area, scroll down
        else if (cursorY >= scrollY + maxLines) {
            scrollY = cursorY - maxLines + 1;
        }
    }

    bool handleInput(int ch) {
        switch (ch) {
            case 17:  // Ctrl+Q
                return false;

            case 16:  // Ctrl+P - Save phrase
                saveCurrentLineAsPhrase();
                break;

            case KEY_UP:
                if (showingSuggestions && selectedSuggestion > 0) {
                    selectedSuggestion--;
                } else if (cursorY > 0) {
                    cursorY--;
                    if (cursorX > lines[cursorY].size()) {
                        cursorX = lines[cursorY].size();
                    }
                    updateScroll();
                }
                break;

            case KEY_DOWN:
                if (showingSuggestions && selectedSuggestion < suggestions.size() - 1) {
                    selectedSuggestion++;
                } else if (cursorY < lines.size() - 1) {
                    cursorY++;
                    if (cursorX > lines[cursorY].size()) {
                        cursorX = lines[cursorY].size();
                    }
                    updateScroll();
                }
                break;

            case KEY_LEFT:
                if (cursorX > 0) {
                    cursorX--;
                } else if (cursorY > 0) {
                    cursorY--;
                    cursorX = lines[cursorY].size();
                    updateScroll();
                }
                showingSuggestions = false;
                break;

            case KEY_RIGHT:
                if (cursorX < lines[cursorY].size()) {
                    cursorX++;
                } else if (cursorY < lines.size() - 1) {
                    cursorY++;
                    cursorX = 0;
                    updateScroll();
                }
                showingSuggestions = false;
                break;

            case '\t':  // Tab - Accept suggestion
                if (showingSuggestions && !suggestions.empty()) {
                    acceptSuggestion();
                } else {
                    triggerAutocomplete();
                }
                break;

            case '\n':  // Enter with smart indentation
                showingSuggestions = false;
                {
                    // Allow unlimited lines with scrolling
                        // Calculate base indentation
                        int baseIndent = 0;
                        for (char c : lines[cursorY]) {
                            if (c == ' ') baseIndent++;
                            else break;
                        }

                        // Check if we need to add extra indent
                        bool needExtraIndent = false;
                        bool addClosingBrace = false;

                        // Look at character before cursor
                        if (cursorX > 0) {
                            char prevChar = lines[cursorY][cursorX - 1];
                            if (prevChar == '{') {
                                needExtraIndent = true;
                                // Check if next character is closing brace
                                if (cursorX < lines[cursorY].size() && lines[cursorY][cursorX] == '}') {
                                    addClosingBrace = true;
                                }
                            } else if (prevChar == '(') {
                                needExtraIndent = true;
                            }
                        }

                    std::string remainder = lines[cursorY].substr(cursorX);
                    lines[cursorY] = lines[cursorY].substr(0, cursorX);

                    int newIndent = baseIndent + (needExtraIndent ? 4 : 0);

                    if (addClosingBrace) {
                        // We have {|}, so remainder starts with }
                        // Remove the } from remainder and add it on a separate line
                        if (!remainder.empty() && remainder[0] == '}') {
                            remainder = remainder.substr(1); // Remove the }
                        }

                        // Insert indented empty line for code
                        lines.insert(lines.begin() + cursorY + 1, std::string(newIndent, ' ') + remainder);
                        cursorY++;
                        cursorX = newIndent;

                        // Add closing brace on next line at original indent
                        lines.insert(lines.begin() + cursorY + 1, std::string(baseIndent, ' ') + "}");
                    } else {
                        // Normal case - just indent
                        lines.insert(lines.begin() + cursorY + 1, std::string(newIndent, ' ') + remainder);
                        cursorY++;
                        cursorX = newIndent;
                    }

                    updateScroll();
                }
                break;

            case 127:  // Backspace
            case KEY_BACKSPACE:
                showingSuggestions = false;
                if (cursorX > 0) {
                    lines[cursorY].erase(cursorX - 1, 1);
                    cursorX--;
                } else if (cursorY > 0) {
                    cursorX = lines[cursorY - 1].size();
                    lines[cursorY - 1] += lines[cursorY];
                    lines.erase(lines.begin() + cursorY);
                    cursorY--;
                    updateScroll();
                }
                break;

            case 27:  // Escape
                showingSuggestions = false;
                break;

            default:
                if (ch >= 32 && ch <= 126) {
                    lines[cursorY].insert(cursorX, 1, ch);
                    cursorX++;

                    // Auto-close brackets
                    if (ch == '(') {
                        lines[cursorY].insert(cursorX, ")");
                    } else if (ch == '{') {
                        lines[cursorY].insert(cursorX, "}");
                    } else if (ch == '[') {
                        lines[cursorY].insert(cursorX, "]");
                    } else if (ch == '"') {
                        lines[cursorY].insert(cursorX, "\"");
                    } else if (ch == '\'') {
                        lines[cursorY].insert(cursorX, "'");
                    }

                    if (isalnum(ch)) {
                        triggerAutocomplete();
                    } else {
                        showingSuggestions = false;
                    }
                }
        }

        return true;
    }

    void triggerAutocomplete() {
        std::string currentWord = getCurrentWord();
        if (currentWord.empty()) {
            showingSuggestions = false;
            return;
        }

        suggestions.clear();
        isPhraseFlag.clear();

        // Get phrase suggestions
        auto phrases = phraseStore.getTopPhrases(currentWord, 3);
        for (const auto& phrase : phrases) {
            suggestions.push_back("[PHRASE] " + phrase.snippet);
            isPhraseFlag.push_back(true);
        }

        // Get token suggestions
        auto tokens = tst.prefixSearch(currentWord, 5);
        for (const auto& token : tokens) {
            double score = freqStore.get(token);
            suggestions.push_back(token + " (" + std::to_string((int)score) + ")");
            isPhraseFlag.push_back(false);
        }

        showingSuggestions = !suggestions.empty();
        selectedSuggestion = 0;
    }

    void acceptSuggestion() {
        if (!showingSuggestions || suggestions.empty()) return;

        std::string currentWord = getCurrentWord();
        std::string suggestion = suggestions[selectedSuggestion];

        // Extract actual text
        std::string textToInsert;
        if (suggestion.find("[PHRASE]") == 0) {
            textToInsert = suggestion.substr(9);
        } else {
            size_t pos = suggestion.find(" (");
            if (pos != std::string::npos) {
                textToInsert = suggestion.substr(0, pos);
            } else {
                textToInsert = suggestion;
            }
        }

        // Replace current word
        int wordStart = cursorX - currentWord.length();
        lines[cursorY].erase(wordStart, currentWord.length());
        lines[cursorY].insert(wordStart, textToInsert);
        cursorX = wordStart + textToInsert.length();

        // Update frequency
        freqStore.bump(textToInsert, 1);
        if (!lastAcceptedWord.empty()) {
            graph.addEdge(lastAcceptedWord, textToInsert);
        }
        lastAcceptedWord = textToInsert;

        showingSuggestions = false;
    }

    std::string getCurrentWord() {
        std::string& line = lines[cursorY];
        int end = cursorX;
        int start = end;

        while (start > 0 && (isalnum(line[start - 1]) || line[start - 1] == '_')) {
            start--;
        }

        if (start == end) return "";
        return line.substr(start, end - start);
    }

    void saveCurrentLineAsPhrase() {
        std::string line = lines[cursorY];
        if (line.empty()) return;

        size_t spacePos = line.find(' ');
        std::string trigger = (spacePos != std::string::npos) ?
                             line.substr(0, spacePos) : line;

        phraseStore.addPhrase(trigger, line);
        phraseStore.save();

        mvprintw(LINES - 1, 0, "[OK] Phrase saved: '%s' -> '%s' (Press any key)",
                trigger.c_str(), line.c_str());
        refresh();
        getch();
    }
};

int main() {
    BasicEditor editor;
    editor.run();
    return 0;
}
