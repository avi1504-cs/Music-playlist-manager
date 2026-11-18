#include "Queue.h"

Queue::Queue(int capacity) : frontNode(nullptr), rearNode(nullptr), count(0), maxCapacity(capacity) {}

Queue::~Queue() {
    clear();
}

void Queue::clear() {
    while (frontNode != nullptr) {
        QueueNode* temp = frontNode;
        frontNode = frontNode->next;
        delete temp;
    }
    rearNode = nullptr;
    count = 0;
}

void Queue::enqueue(const Song& song) {
    // If queue reaches max capacity, automatically dequeue the oldest entry
    if (count >= maxCapacity) {
        Song dummy;
        dequeue(dummy);
    }

    QueueNode* newNode = new QueueNode(song);
    if (isEmpty()) {
        frontNode = rearNode = newNode;
    } else {
        rearNode->next = newNode;
        rearNode = newNode;
    }
    count++;
}

bool Queue::dequeue(Song& outSong) {
    if (isEmpty()) return false;

    QueueNode* temp = frontNode;
    outSong = temp->song;
    frontNode = frontNode->next;
    
    if (frontNode == nullptr) {
        rearNode = nullptr;
    }

    delete temp;
    count--;
    return true;
}

bool Queue::isEmpty() const {
    return frontNode == nullptr;
}

int Queue::size() const {
    return count;
}

void Queue::display() const {
    if (isEmpty()) {
        std::cout << "\n[!] No recently played songs.\n";
        return;
    }

    std::cout << "\n====================================================================================================\n";
    std::cout << "                               RECENTLY PLAYED SONGS (Queue - FIFO)                                 \n";
    std::cout << "====================================================================================================\n";
    std::cout << std::left 
              << std::setw(6)  << "ID"
              << std::setw(22) << "Title"
              << std::setw(18) << "Artist"
              << std::setw(18) << "Album"
              << std::setw(12) << "Genre"
              << std::setw(10) << "Duration"
              << std::setw(8)  << "Year"
              << std::setw(8)  << "Rating"
              << std::setw(10) << "Favorite"
              << std::endl;
    std::cout << "----------------------------------------------------------------------------------------------------\n";

    QueueNode* curr = frontNode;
    while (curr != nullptr) {
        curr->song.displayFormattedRow();
        curr = curr->next;
    }
    std::cout << "----------------------------------------------------------------------------------------------------\n";
    std::cout << " Total Items in History Queue: " << count << " / " << maxCapacity << "\n\n";
}
