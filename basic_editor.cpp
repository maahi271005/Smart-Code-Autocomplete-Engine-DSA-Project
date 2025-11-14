// Basic working editor with autocomplete - NO COLORS, JUST WORKS
// Compile: g++ -std=c++17 basic_editor.cpp src/tst.cpp src/phrase_store.cpp src/freq_store.cpp src/ranker.cpp src/graph.cpp src/minheap.cpp src/lru.cpp src/stack.cpp src/kmp.cpp -lncurses -Iinclude -o basic_editor

#include <ncurses.h>
#include <string>
#include <vector>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <filesystem>
#include <unordered_set>

#include "tst.h"
#include "phrase_store.h"
#include "freq_store.h"
#include "ranker.h"
#include "graph.h"
#include "kmp.h"
#include "minheap.h"
#include "lru.h"
#include "stack.h"

class BasicEditor {
private:
    std::vector<std::string> lines;
    std::vector<std::string> suggestions;
    std::vector<bool> isPhraseFlag;
    int cursorY, cursorX;
    int scrollY;  
    bool showingSuggestions;
    int selectedSuggestion;

    TST tst;
    PhraseStore phraseStore;
    FreqStore freqStore;
    CooccurrenceGraph graph;
    Ranker ranker;
    std::vector<std::string> dictionaryWords;
    std::string lastAcceptedWord;

    // A) MinHeap for Top-K ranking
    MinHeap suggestionHeap{10};

    // B) LRU Cache for query caching
    lru_cache suggestionCache{100};

    // C) Stack for undo/redo
    UndoRedoStack undoRedoStack;

    std::string currentFileName;
    bool fileModified;

    std::string searchQuery;
    bool searchMode;

public:
    BasicEditor()
        : cursorY(0), cursorX(0), scrollY(0),
        showingSuggestions(false), selectedSuggestion(0),
        phraseStore("data/phrases.txt"),
        freqStore("data/frequency.txt"),
        ranker(&freqStore, &graph),
        currentFileName(""),
        fileModified(false),
        searchMode(false) {

        lines.push_back("");
                loadDictionary();

                std::error_code ec;
                std::filesystem::create_directories("scratch", ec);
                
    }

    void loadDictionary() {
        std::ifstream file("data/words.txt");
        if (!file.is_open()) return;

        std::string word;
        while (file >> word) {
            if (!word.empty()) {
                tst.insert(word);
                dictionaryWords.push_back(word);
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

        //Colors for syntax highlighting
        start_color();
        init_pair(1, COLOR_BLUE, COLOR_BLACK);    // Keywords
        init_pair(2, COLOR_GREEN, COLOR_BLACK);   // Strings
        init_pair(3, COLOR_CYAN, COLOR_BLACK);    // Comments
        init_pair(4, COLOR_YELLOW, COLOR_BLACK);  // Numbers
        init_pair(5, COLOR_MAGENTA, COLOR_BLACK); // Preprocessor
        init_pair(6, COLOR_RED, COLOR_BLACK);     // Operators
        init_pair(7, COLOR_WHITE, COLOR_BLUE);    // Selection/Highlight

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
    bool isKeyword(const std::string& word) {
        static const std::vector<std::string> keywords = {
            "auto", "break", "case", "char", "const", "continue", "default", "do",
            "double", "else", "enum", "extern", "float", "for", "goto", "if",
            "int", "long", "register", "return", "short", "signed", "sizeof", "static",
            "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while",
            "class", "namespace", "template", "public", "private", "protected", "virtual",
            "bool", "true", "false", "nullptr", "new", "delete", "try", "catch", "throw",
            "using", "std", "string", "vector", "map", "set", "include", "define", "ifdef"
        };
        return std::find(keywords.begin(), keywords.end(), word) != keywords.end();
    }

    void drawLineWithSyntax(int y, int lineNum, const std::string& line) {
        mvprintw(y, 0, "%3d | ", lineNum);
        int x = 6;

        for (size_t i = 0; i < line.length(); i++) {
            // Handle strings
            if (line[i] == '"' || line[i] == '\'') {
                char quote = line[i];
                attron(COLOR_PAIR(2));
                mvaddch(y, x++, line[i]);
                i++;
                while (i < line.length() && line[i] != quote) {
                    mvaddch(y, x++, line[i]);
                    if (line[i] == '\\' && i + 1 < line.length()) {
                        i++;
                        mvaddch(y, x++, line[i]);
                    }
                    i++;
                }
                if (i < line.length()) mvaddch(y, x++, line[i]);
                attroff(COLOR_PAIR(2));
                continue;
            }

            // Handle comments
            if (i + 1 < line.length() && line[i] == '/' && line[i+1] == '/') {
                attron(COLOR_PAIR(3));
                while (i < line.length()) {
                    mvaddch(y, x++, line[i++]);
                }
                attroff(COLOR_PAIR(3));
                break;
            }

            // Handle preprocessor
            if (line[i] == '#') {
                attron(COLOR_PAIR(5));
                std::string word;
                while (i < line.length() && (isalnum(line[i]) || line[i] == '#' || line[i] == '_')) {
                    word += line[i];
                    mvaddch(y, x++, line[i++]);
                }
                attroff(COLOR_PAIR(5));
                i--;
                continue;
            }

            // Handle numbers
            if (isdigit(line[i])) {
                attron(COLOR_PAIR(4));
                while (i < line.length() && (isdigit(line[i]) || line[i] == '.')) {
                    mvaddch(y, x++, line[i++]);
                }
                attroff(COLOR_PAIR(4));
                i--;
                continue;
            }

            // Handle operators
            if (strchr("+-*/%=<>!&|^~?:;,(){}[]", line[i])) {
                attron(COLOR_PAIR(6));
                mvaddch(y, x++, line[i]);
                attroff(COLOR_PAIR(6));
                continue;
            }

            // Handle keywords and identifiers
            if (isalnum(line[i]) || line[i] == '_') {
                std::string word;
                size_t start = i;
                while (i < line.length() && (isalnum(line[i]) || line[i] == '_')) {
                    word += line[i++];
                }
                i--;

                if (isKeyword(word)) {
                    attron(COLOR_PAIR(1) | A_BOLD);
                    mvprintw(y, x, "%s", word.c_str());
                    attroff(COLOR_PAIR(1) | A_BOLD);
                } else {
                    mvprintw(y, x, "%s", word.c_str());
                }
                x += word.length();
                continue;
            }

            // Default: print character as-is
            mvaddch(y, x++, line[i]);
        }
    }

    void draw() {
        clear();

        // Calculate how many lines we can show
        int maxLines = LINES - 3;

        // Draw lines with syntax highlighting
        for (int i = 0; i < maxLines && (scrollY + i) < lines.size(); i++) {
            int lineNum = scrollY + i + 1;
            drawLineWithSyntax(i, lineNum, lines[scrollY + i]);
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

        // Status bar with file info
        std::string fileName = currentFileName.empty() ? "[No Name]" : currentFileName;
        std::string modifiedMark = fileModified ? " [+]" : "";
        attron(A_REVERSE);
        mvprintw(LINES - 2, 0, " %s%s | Line %d/%zu Col %d | %d phrases | Ctrl+O: Open | Ctrl+W: Save | Ctrl+R: Search | Ctrl+N: Next | Ctrl+H: Help | Ctrl+Q: Quit ",
                fileName.c_str(), modifiedMark.c_str(), cursorY + 1, lines.size(), cursorX + 1, phraseStore.getTotalPhrases());
        attroff(A_REVERSE);

        // Clear rest of status line
        for (int i = fileName.length() + 130; i < COLS; i++) {
            mvaddch(LINES - 2, i, ' ');
        }

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
                if (fileModified) {
                    // Show confirmation dialog
                    mvprintw(LINES - 1, 0, "File modified. Save? (y/n/c to cancel): ");
                    refresh();
                    int confirm = getch();
                    if (confirm == 'y' || confirm == 'Y') {
                        saveFile();
                    } else if (confirm == 'c' || confirm == 'C') {
                        return true;  // Don't quit
                    }
                }
                return false;

            case 15:  // Ctrl+O - Open file
                openFile();
                break;

            case 23:  // Ctrl+W - Write/Save file
                saveFile();
                break;

            case 18:  // Ctrl+R - Find/Search (R for seaRch)
                searchFile();
                break;

            case 14:  // Ctrl+N - Find Next
                findNext();
                break;

            case 16:  // Ctrl+P - Save phrase
            case 19:  // Ctrl+S - Save phrase (alternative)
                saveCurrentLineAsPhrase();
                break;

            case 8:   // Ctrl+H - Show help
                showHelp();
                break;

            case 26:  // Ctrl+Z - Undo
                undoLastChange();
                break;

            case 25:  // Ctrl+Y - Redo
                redoLastChange();
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
                // C) Save to undo stack before modification
                saveToUndoStack();

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
                // C) Save to undo stack before modification
                saveToUndoStack();

                showingSuggestions = false;
                fileModified = true;
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
                    // C) Save to undo stack before modification
                    saveToUndoStack();

                    lines[cursorY].insert(cursorX, 1, ch);
                    cursorX++;
                    fileModified = true;

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

                    if (isalnum(ch) || ch == '#') {
                        triggerAutocomplete();
                    } else {
                        showingSuggestions = false;
                    }
                }
        }

        return true;
    }

    // C) Undo/Redo using Stack
    void saveToUndoStack() {
        // Store current line state (simple: just track line content changes)
        if (cursorY < (int)lines.size()) {
            undoRedoStack.pushInsert(cursorY, lines[cursorY]);
        }
    }

    void undoLastChange() {
        if (!undoRedoStack.canUndo()) {
            mvprintw(LINES - 1, 0, "Nothing to undo                                                                                                                                                              ");
            refresh();
            return;
        }
        auto [lineIdx, content] = undoRedoStack.undo();
        if (lineIdx >= 0 && lineIdx < (int)lines.size()) {
            lines[lineIdx] = content;
            mvprintw(LINES - 1, 0, "Undo performed                                                                                                                                                                    ");
            refresh();
            getch();
        }
    }

    void redoLastChange() {
        if (!undoRedoStack.canRedo()) {
            mvprintw(LINES - 1, 0, "Nothing to redo                                                                                                                                                              ");
            refresh();
            return;
        }
        auto [lineIdx, content] = undoRedoStack.redo();
        if (lineIdx >= 0 && lineIdx < (int)lines.size()) {
            lines[lineIdx] = content;
            mvprintw(LINES - 1, 0, "Redo performed                                                                                                                                                                    ");
            refresh();
            getch();
        }
    }

    void restoreState(const std::string& state) {
        // Not needed anymore with UndoRedoStack
    }

    void triggerAutocomplete() {
        std::string currentWord = getCurrentWord();
        if (currentWord.empty()) {
            showingSuggestions = false;
            return;
        }

        suggestions.clear();
        isPhraseFlag.clear();

        // B) Check LRU cache first
        if (suggestionCache.exists(currentWord)) {
            auto cached = suggestionCache.get(currentWord);
            // Use cached suggestions if available
            if (!cached.empty()) {
                for (const auto& suggestion : cached) {
                    suggestions.push_back(suggestion);
                    isPhraseFlag.push_back(suggestion.find("[PHRASE]") == 0);
                }
                showingSuggestions = !suggestions.empty();
                selectedSuggestion = 0;
                return;
            }
        }

        // A) Use MinHeap for ranking
        suggestionHeap.clear(); // reset heap for new query

        const int maxSuggestions = 10;
        std::unordered_set<std::string> seen;

        // 1) Phrase suggestions
        auto phrases = phraseStore.getTopPhrases(currentWord, 3);
        for (const auto& phrase : phrases) {
            std::string displayStr = "[PHRASE] " + phrase.snippet;
            suggestionHeap.insert(5.0, displayStr); // phrase priority: 5.0
            seen.insert(phrase.snippet);
        }

        // 2) Prefix token suggestions from TST
        int need = maxSuggestions - suggestionHeap.size();
        if (need > 0) {
            auto tokens = tst.prefixSearch(currentWord, need);
            for (const auto& token : tokens) {
                if (seen.find(token) != seen.end()) continue;
                double score = freqStore.get(token);
                suggestionHeap.insert(score, token);
                seen.insert(token);
            }
        }

        // 3) Substring matches using KMP
        need = maxSuggestions - suggestionHeap.size();
        if (need > 0) {
            for (const auto& word : dictionaryWords) {
                if ((int)suggestionHeap.size() >= maxSuggestions) break;
                if (seen.find(word) != seen.end()) continue;
                if (word.rfind(currentWord, 0) == 0) continue; // skip prefix matches
                if (KMP::contains(word, currentWord)) {
                    double score = freqStore.get(word);
                    suggestionHeap.insert(score, word);
                    seen.insert(word);
                }
            }
        }

        // Extract all from heap and build suggestions list
        auto heapResults = suggestionHeap.getAll();
        // Sort by score (descending)
        std::sort(heapResults.begin(), heapResults.end(), 
                 [](const auto& a, const auto& b) { return a.first > b.first; });

        for (const auto& [score, suggestion] : heapResults) {
            suggestions.push_back(suggestion);
            isPhraseFlag.push_back(suggestion.find("[PHRASE]") == 0);
        }

        // B) Store in LRU cache for next time
        if (!suggestions.empty()) {
            suggestionCache.put(currentWord, suggestions);
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

        // Include alphanumeric, underscore, and # for preprocessor directives
        while (start > 0 && (isalnum(line[start - 1]) || line[start - 1] == '_' || line[start - 1] == '#')) {
            start--;
        }

        if (start == end) return "";
        return line.substr(start, end - start);
    }

    void openFile() {
        echo();
        mvprintw(LINES - 1, 0, "Open file: ");
        clrtoeol();
        refresh();

        char filename[256];
        getnstr(filename, 255);
        noecho();

        if (strlen(filename) == 0) return;

        std::ifstream file(filename);
        if (!file.is_open()) {
            mvprintw(LINES - 1, 0, "Error: Could not open file '%s'", filename);
            refresh();
            getch();
            return;
        }

        lines.clear();
        std::string line;
        while (std::getline(file, line)) {
            lines.push_back(line);
        }
        file.close();

        if (lines.empty()) {
            lines.push_back("");
        }

        currentFileName = filename;
        fileModified = false;
        cursorY = 0;
        cursorX = 0;
        scrollY = 0;

        mvprintw(LINES - 1, 0, "Loaded '%s' (%zu lines)", filename, lines.size());
        refresh();
        getch();
    }

    void saveFile() {
        std::string filename = currentFileName;

        if (filename.empty()) {
            echo();
            mvprintw(LINES - 1, 0, "Save as: ");
            clrtoeol();
            refresh();

            char fname[256];
            getnstr(fname, 255);
            noecho();

            if (strlen(fname) == 0) return;
            filename = fname;
        }

        // Ensure we save into `scratch/` for local editor-created files when a simple filename is provided
        std::string finalPath = filename;
        if (filename.empty()) {
            // handled above
        } else {
            // If user provided just a filename (no directories) and it's not an absolute path, place it under scratch/
            if (filename.find('/') == std::string::npos && filename[0] != '/') {
                std::error_code ec;
                std::filesystem::create_directories("scratch", ec);
                finalPath = std::string("scratch/") + filename;
            }
        }

        // Ensure parent directories for the final path exist (covers absolute or nested paths)
        {
            std::filesystem::path p(finalPath);
            if (p.has_parent_path()) {
                std::error_code ec;
                std::filesystem::create_directories(p.parent_path(), ec);
            }
        }

        std::ofstream file(finalPath);
        if (!file.is_open()) {
            mvprintw(LINES - 1, 0, "Error: Could not save file '%s'", finalPath.c_str());
            refresh();
            getch();
            return;
        }

        for (size_t i = 0; i < lines.size(); i++) {
            file << lines[i];
            if (i < lines.size() - 1) {
                file << "\n";
            }
        }
        file.close();

        currentFileName = finalPath;
        fileModified = false;

        mvprintw(LINES - 1, 0, "Saved '%s' (%zu lines)", currentFileName.c_str(), lines.size());
        refresh();
        getch();
    }

    void searchFile() {
        echo();
        mvprintw(LINES - 1, 0, "Search: ");
        clrtoeol();
        refresh();

        char query[256];
        getnstr(query, 255);
        noecho();

        if (strlen(query) == 0) return;

        searchQuery = query;

        // Search from current position
        for (size_t i = cursorY; i < lines.size(); i++) {
            size_t pos = lines[i].find(searchQuery, (i == cursorY) ? cursorX + 1 : 0);
            if (pos != std::string::npos) {
                cursorY = i;
                cursorX = pos;
                updateScroll();
                mvprintw(LINES - 1, 0, "Found at line %zu, column %zu (Ctrl+N for next)", i + 1, pos + 1);
                refresh();
                return;
            }
        }

        // Wrap around
        for (size_t i = 0; i < cursorY; i++) {
            size_t pos = lines[i].find(searchQuery);
            if (pos != std::string::npos) {
                cursorY = i;
                cursorX = pos;
                updateScroll();
                mvprintw(LINES - 1, 0, "Found at line %zu, column %zu (wrapped, Ctrl+N for next)", i + 1, pos + 1);
                refresh();
                return;
            }
        }

        mvprintw(LINES - 1, 0, "Not found: '%s'", searchQuery.c_str());
        refresh();
        getch();
    }

    void findNext() {
        if (searchQuery.empty()) {
            mvprintw(LINES - 1, 0, "No search query. Press Ctrl+R to search first.");
            refresh();
            getch();
            return;
        }

        // Search from next position after current cursor
        for (size_t i = cursorY; i < lines.size(); i++) {
            size_t startPos = (i == cursorY) ? cursorX + 1 : 0;
            size_t pos = lines[i].find(searchQuery, startPos);
            if (pos != std::string::npos) {
                cursorY = i;
                cursorX = pos;
                updateScroll();
                mvprintw(LINES - 1, 0, "Found at line %zu, column %zu (Ctrl+N for next)", i + 1, pos + 1);
                refresh();
                return;
            }
        }

        // Wrap around to beginning
        for (size_t i = 0; i <= cursorY; i++) {
            size_t pos = lines[i].find(searchQuery);
            if (pos != std::string::npos) {
                // Skip the current match
                if (i == cursorY && pos == cursorX) {
                    continue;
                }
                cursorY = i;
                cursorX = pos;
                updateScroll();
                mvprintw(LINES - 1, 0, "Found at line %zu, column %zu (wrapped, Ctrl+N for next)", i + 1, pos + 1);
                refresh();
                return;
            }
        }

        mvprintw(LINES - 1, 0, "No more matches for '%s'", searchQuery.c_str());
        refresh();
        getch();
    }

    void showHelp() {
        clear();

        // Title
        attron(A_BOLD | COLOR_PAIR(1));
        mvprintw(1, 2, "=== SMART CODE AUTOCOMPLETE EDITOR - HELP ===");
        attroff(A_BOLD | COLOR_PAIR(1));

        int line = 3;

        // File Operations - NEW!
        attron(A_UNDERLINE | COLOR_PAIR(5));
        mvprintw(line++, 2, "FILE OPERATIONS (NEW!):");
        attroff(A_UNDERLINE | COLOR_PAIR(5));
        attron(COLOR_PAIR(2));
        mvprintw(line++, 4, "Ctrl+O           - Open file");
        mvprintw(line++, 4, "Ctrl+W           - Save file (Write)");
        mvprintw(line++, 4, "Ctrl+R           - Find/Search text");
        mvprintw(line++, 4, "Ctrl+N           - Find next match (after Ctrl+R)");
        mvprintw(line++, 4, "Ctrl+Z           - Undo last change");
        mvprintw(line++, 4, "Ctrl+Y           - Redo change");
        mvprintw(line++, 4, "[+] indicator    - Shows unsaved changes in status bar");
        attroff(COLOR_PAIR(2));
        line++;

        // Syntax Highlighting - NEW!
        attron(A_UNDERLINE | COLOR_PAIR(5));
        mvprintw(line++, 2, "SYNTAX HIGHLIGHTING (NEW!):");
        attroff(A_UNDERLINE | COLOR_PAIR(5));
        attron(COLOR_PAIR(1));
        mvprintw(line++, 4, "Keywords         - Blue bold (for, if, while, class, int...)");
        attroff(COLOR_PAIR(1));
        attron(COLOR_PAIR(2));
        mvprintw(line++, 4, "Strings          - Green (\"hello\", 'c')");
        attroff(COLOR_PAIR(2));
        attron(COLOR_PAIR(3));
        mvprintw(line++, 4, "Comments         - Cyan (// comment)");
        attroff(COLOR_PAIR(3));
        attron(COLOR_PAIR(4));
        mvprintw(line++, 4, "Numbers          - Yellow (123, 45.67)");
        attroff(COLOR_PAIR(4));
        attron(COLOR_PAIR(5));
        mvprintw(line++, 4, "Preprocessor     - Magenta (#include, #define)");
        attroff(COLOR_PAIR(5));
        attron(COLOR_PAIR(6));
        mvprintw(line++, 4, "Operators        - Red (+, -, *, /, =, <, >)");
        attroff(COLOR_PAIR(6));
        line++;

        // Navigation
        attron(A_UNDERLINE);
        mvprintw(line++, 2, "NAVIGATION:");
        attroff(A_UNDERLINE);
        mvprintw(line++, 4, "Arrow Keys       - Move cursor up/down/left/right");
        mvprintw(line++, 4, "Enter            - New line (with smart indentation)");
        mvprintw(line++, 4, "Backspace        - Delete character");
        line++;

        // Autocomplete
        attron(A_UNDERLINE);
        mvprintw(line++, 2, "AUTOCOMPLETE:");
        attroff(A_UNDERLINE);
        mvprintw(line++, 4, "Type             - Auto-trigger suggestions as you type");
        mvprintw(line++, 4, "Tab              - Accept selected suggestion");
        mvprintw(line++, 4, "Up/Down          - Navigate suggestions (when popup open)");
        mvprintw(line++, 4, "Esc              - Close suggestions popup");
        line++;

        // Phrase Learning
        attron(A_UNDERLINE);
        mvprintw(line++, 2, "PHRASE LEARNING:");
        attroff(A_UNDERLINE);
        mvprintw(line++, 4, "Ctrl+S           - Save current line as reusable phrase");
        mvprintw(line++, 4, "[PHRASE]         - Indicates learned phrase in suggestions");
        mvprintw(line++, 4, "Phrases persist  - Saved in data/phrases.txt");
        line++;

        // Smart Features
        attron(A_UNDERLINE);
        mvprintw(line++, 2, "SMART FEATURES:");
        attroff(A_UNDERLINE);
        mvprintw(line++, 4, "Auto-close       - ( ) { } [ ] \" \" ' ' auto-complete");
        mvprintw(line++, 4, "Smart indent     - Auto-indent after { or (");
        mvprintw(line++, 4, "Bracket match    - Press Enter between {} for auto-format");
        mvprintw(line++, 4, "Scrolling        - Unlimited lines with auto-scroll");
        line++;

        // Data Structures
        attron(A_UNDERLINE);
        mvprintw(line++, 2, "POWERED BY 8 DATA STRUCTURES:");
        attroff(A_UNDERLINE);
        mvprintw(line++, 4, "1. TST (Ternary Search Tree)   - Prefix search O(m log n)");
        mvprintw(line++, 4, "2. MinHeap                      - Top-K ranking O(n log k)");
        mvprintw(line++, 4, "3. HashMap (PhraseStore)        - Phrase storage O(1)");
        mvprintw(line++, 4, "4. HashMap (FreqStore)          - Frequency tracking O(1)");
        mvprintw(line++, 4, "5. Graph (Co-occurrence)        - Context awareness O(1)");
        mvprintw(line++, 4, "6. Stack                        - Undo/redo (internal) O(1)");
        mvprintw(line++, 4, "7. LRU Cache                    - Query optimization O(1)");
        mvprintw(line++, 4, "8. KMP Algorithm                - Pattern matching O(n+m)");
        line++;

        // Status
        mvprintw(line++, 2, "Current Status:");
        mvprintw(line++, 4, "Dictionary words: 10,000+");
        mvprintw(line++, 4, "Learned phrases: %d", phraseStore.getTotalPhrases());
        line++;

        // Footer
        attron(A_BOLD);
        mvprintw(LINES - 2, 2, "Press any key to return to editor...");
        attroff(A_BOLD);

        refresh();
        getch();
    }

    void saveCurrentLineAsPhrase() {
        std::string line = lines[cursorY];

        // Remove leading/trailing spaces
        size_t start = line.find_first_not_of(" \t");
        if (start == std::string::npos || line.empty()) {
            mvprintw(LINES - 1, 0, "[ERROR] Empty line, cannot save phrase (Press any key)");
            refresh();
            getch();
            return;
        }

        line = line.substr(start);
        size_t end = line.find_last_not_of(" \t");
        if (end != std::string::npos) {
            line = line.substr(0, end + 1);
        }

        // Extract trigger (first word or until opening bracket/paren/angle)
        std::string trigger;
        for (char c : line) {
            if (c == ' ' || c == '(' || c == '{' || c == '[' || c == '<' || c == '>') {
                break;
            }
            trigger += c;
        }

        if (trigger.empty()) {
            mvprintw(LINES - 1, 0, "[ERROR] Cannot extract trigger (Press any key)");
            refresh();
            getch();
            return;
        }

        phraseStore.addPhrase(trigger, line);
        phraseStore.save();

        mvprintw(LINES - 1, 0, "[OK] Phrase saved: '%s' -> '%s' (Press any key)          ",
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
