#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H

#include "Song.h"
#include <iostream>

struct Node {
    Song data;
    Node* prev;
    Node* next;

    Node(const Song& song) : data(song), prev(nullptr), next(nullptr) {}
};

class DoublyLinkedList {
private:
    Node* head;
    Node* tail;
    int sizeCount;

public:
    DoublyLinkedList();
    ~DoublyLinkedList();

    // Prevent direct copying to avoid double-free memory bugs
    DoublyLinkedList(const DoublyLinkedList& other) = delete;
    DoublyLinkedList& operator=(const DoublyLinkedList& other) = delete;

    // Operations
    void append(const Song& song);
    void prepend(const Song& song);
    bool insertAt(int position, const Song& song); // 1-based indexing
    bool removeById(int id, Song& outDeletedSong);
    bool removeAt(int position, Song& outDeletedSong);
    Node* findNodeById(int id) const;
    
    bool movePosition(int oldPosition, int newPosition);
    void reverse();
    void shuffle();
    void clear();

    // Accessors
    Node* getHead() const;
    Node* getTail() const;
    int getSize() const;
    bool isEmpty() const;

    // Display helpers
    void displayForward() const;
    void displayReverse() const;
};

#endif // DOUBLY_LINKED_LIST_H
