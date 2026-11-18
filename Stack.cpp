#include "Stack.h"

Stack::Stack() : topNode(nullptr), count(0) {}

Stack::~Stack() {
    clear();
}

void Stack::clear() {
    while (topNode != nullptr) {
        StackNode* temp = topNode;
        topNode = topNode->next;
        delete temp;
    }
    count = 0;
}

void Stack::push(const UndoAction& action) {
    StackNode* newNode = new StackNode(action);
    newNode->next = topNode;
    topNode = newNode;
    count++;
}

bool Stack::pop(UndoAction& outAction) {
    if (isEmpty()) return false;

    StackNode* temp = topNode;
    outAction = temp->action;
    topNode = topNode->next;
    delete temp;
    count--;
    return true;
}

bool Stack::peek(UndoAction& outAction) const {
    if (isEmpty()) return false;
    outAction = topNode->action;
    return true;
}

bool Stack::isEmpty() const {
    return topNode == nullptr;
}

int Stack::size() const {
    return count;
}
