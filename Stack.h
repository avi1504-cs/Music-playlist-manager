#ifndef STACK_H
#define STACK_H

#include "Song.h"
#include <iostream>

enum class UndoType {
    DELETE_ACTION,
    UPDATE_ACTION
};

struct UndoAction {
    UndoType type;
    Song songData;
    int originalPosition; // 1-based position in playlist

    UndoAction() : type(UndoType::DELETE_ACTION), songData(), originalPosition(1) {}
    UndoAction(UndoType t, const Song& s, int pos) : type(t), songData(s), originalPosition(pos) {}
};

struct StackNode {
    UndoAction action;
    StackNode* next;

    StackNode(const UndoAction& act) : action(act), next(nullptr) {}
};

class Stack {
private:
    StackNode* topNode;
    int count;

public:
    Stack();
    ~Stack();

    Stack(const Stack& other) = delete;
    Stack& operator=(const Stack& other) = delete;

    void push(const UndoAction& action);
    bool pop(UndoAction& outAction);
    bool peek(UndoAction& outAction) const;
    bool isEmpty() const;
    int size() const;
    void clear();
};

#endif // STACK_H
