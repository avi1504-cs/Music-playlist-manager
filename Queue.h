#ifndef QUEUE_H
#define QUEUE_H

#include "Song.h"
#include <iostream>

struct QueueNode {
    Song song;
    QueueNode* next;

    QueueNode(const Song& s) : song(s), next(nullptr) {}
};

class Queue {
private:
    QueueNode* frontNode;
    QueueNode* rearNode;
    int count;
    int maxCapacity;

public:
    Queue(int capacity = 10);
    ~Queue();

    Queue(const Queue& other) = delete;
    Queue& operator=(const Queue& other) = delete;

    void enqueue(const Song& song);
    bool dequeue(Song& outSong);
    bool isEmpty() const;
    int size() const;
    void clear();

    void display() const;
};

#endif // QUEUE_H
