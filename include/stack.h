#ifndef STACK_H
#define STACK_H

#include <stack>
#include <string>
#include <utility>

class UndoRedoStack {
private:
    std::stack<std::pair<int, std::string>> undoStack;
    std::stack<std::pair<int, std::string>> redoStack;

public:
    void pushInsert(int position, const std::string& text);
    std::pair<int, std::string> undo();
    std::pair<int, std::string> redo();
    bool canUndo() const { return !undoStack.empty(); }
    bool canRedo() const { return !redoStack.empty(); }
    void clearRedo();
};

#endif