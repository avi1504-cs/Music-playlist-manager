#include "DoublyLinkedList.h"
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>

DoublyLinkedList::DoublyLinkedList() : head(nullptr), tail(nullptr), sizeCount(0) {}

DoublyLinkedList::~DoublyLinkedList() {
    clear();
}

void DoublyLinkedList::clear() {
    Node* current = head;
    while (current != nullptr) {
        Node* nextNode = current->next;
        delete current;
        current = nextNode;
    }
    head = nullptr;
    tail = nullptr;
    sizeCount = 0;
}

void DoublyLinkedList::append(const Song& song) {
    Node* newNode = new Node(song);
    if (isEmpty()) {
        head = tail = newNode;
    } else {
        tail->next = newNode;
        newNode->prev = tail;
        tail = newNode;
    }
    sizeCount++;
}

void DoublyLinkedList::prepend(const Song& song) {
    Node* newNode = new Node(song);
    if (isEmpty()) {
        head = tail = newNode;
    } else {
        newNode->next = head;
        head->prev = newNode;
        head = newNode;
    }
    sizeCount++;
}

bool DoublyLinkedList::insertAt(int position, const Song& song) {
    if (position < 1 || position > sizeCount + 1) return false;
    if (position == 1) {
        prepend(song);
        return true;
    }
    if (position == sizeCount + 1) {
        append(song);
        return true;
    }

    Node* current = head;
    for (int i = 1; i < position - 1; ++i) {
        current = current->next;
    }

    Node* newNode = new Node(song);
    newNode->next = current->next;
    newNode->prev = current;
    if (current->next != nullptr) {
        current->next->prev = newNode;
    }
    current->next = newNode;
    sizeCount++;
    return true;
}

bool DoublyLinkedList::removeById(int id, Song& outDeletedSong) {
    Node* target = findNodeById(id);
    if (target == nullptr) return false;

    outDeletedSong = target->data;

    if (target == head && target == tail) {
        head = tail = nullptr;
    } else if (target == head) {
        head = head->next;
        head->prev = nullptr;
    } else if (target == tail) {
        tail = tail->prev;
        tail->next = nullptr;
    } else {
        target->prev->next = target->next;
        target->next->prev = target->prev;
    }

    delete target;
    sizeCount--;
    return true;
}

bool DoublyLinkedList::removeAt(int position, Song& outDeletedSong) {
    if (position < 1 || position > sizeCount || isEmpty()) return false;

    Node* current = head;
    for (int i = 1; i < position; ++i) {
        current = current->next;
    }

    outDeletedSong = current->data;

    if (current == head && current == tail) {
        head = tail = nullptr;
    } else if (current == head) {
        head = head->next;
        head->prev = nullptr;
    } else if (current == tail) {
        tail = tail->prev;
        tail->next = nullptr;
    } else {
        current->prev->next = current->next;
        current->next->prev = current->prev;
    }

    delete current;
    sizeCount--;
    return true;
}

Node* DoublyLinkedList::findNodeById(int id) const {
    Node* current = head;
    while (current != nullptr) {
        if (current->data.getId() == id) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

bool DoublyLinkedList::movePosition(int oldPosition, int newPosition) {
    if (oldPosition < 1 || oldPosition > sizeCount ||
        newPosition < 1 || newPosition > sizeCount ||
        oldPosition == newPosition) {
        return false;
    }

    // Step 1: Extract node from old position without deleting it
    Node* nodeToMove = head;
    for (int i = 1; i < oldPosition; ++i) {
        nodeToMove = nodeToMove->next;
    }

    if (nodeToMove == head) {
        head = head->next;
        if (head) head->prev = nullptr;
    } else if (nodeToMove == tail) {
        tail = tail->prev;
        if (tail) tail->next = nullptr;
    } else {
        nodeToMove->prev->next = nodeToMove->next;
        nodeToMove->next->prev = nodeToMove->prev;
    }

    nodeToMove->prev = nullptr;
    nodeToMove->next = nullptr;

    // Step 2: Insert node at newPosition
    if (newPosition == 1) {
        nodeToMove->next = head;
        if (head) head->prev = nodeToMove;
        head = nodeToMove;
        if (tail == nullptr) tail = head;
    } else if (newPosition == sizeCount) {
        nodeToMove->prev = tail;
        if (tail) tail->next = nodeToMove;
        tail = nodeToMove;
        if (head == nullptr) head = tail;
    } else {
        Node* target = head;
        // Adjust for index offset if moving backwards vs forwards
        int targetIdx = (newPosition > oldPosition) ? newPosition - 1 : newPosition - 1;
        for (int i = 1; i < targetIdx; ++i) {
            target = target->next;
        }

        nodeToMove->next = target->next;
        nodeToMove->prev = target;
        if (target->next != nullptr) {
            target->next->prev = nodeToMove;
        }
        target->next = nodeToMove;
    }

    return true;
}

void DoublyLinkedList::reverse() {
    if (isEmpty() || head == tail) return;

    Node* current = head;
    Node* temp = nullptr;

    // Swap next and prev pointers for all nodes
    while (current != nullptr) {
        temp = current->prev;
        current->prev = current->next;
        current->next = temp;
        current = current->prev; // move to next node in original sequence
    }

    // Swap head and tail
    if (temp != nullptr) {
        tail = head;
        head = temp->prev;
    }
}

void DoublyLinkedList::shuffle() {
    if (sizeCount <= 1) return;

    // Collect node pointers
    std::vector<Node*> nodes;
    Node* curr = head;
    while (curr) {
        nodes.push_back(curr);
        curr = curr->next;
    }

    // Shuffle using Fisher-Yates algorithm
    unsigned seed = (unsigned)std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(nodes.begin(), nodes.end(), std::default_random_engine(seed));

    // Re-link nodes in shuffled order
    head = nodes[0];
    head->prev = nullptr;
    curr = head;

    for (size_t i = 1; i < nodes.size(); ++i) {
        curr->next = nodes[i];
        nodes[i]->prev = curr;
        curr = nodes[i];
    }
    curr->next = nullptr;
    tail = curr;
}

Node* DoublyLinkedList::getHead() const { return head; }
Node* DoublyLinkedList::getTail() const { return tail; }
int DoublyLinkedList::getSize() const { return sizeCount; }
bool DoublyLinkedList::isEmpty() const { return sizeCount == 0; }

void DoublyLinkedList::displayForward() const {
    if (isEmpty()) {
        std::cout << "\n[!] Playlist is empty.\n";
        return;
    }

    std::cout << "\n----------------------------------------------------------------------------------------------------\n";
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

    Node* current = head;
    while (current != nullptr) {
        current->data.displayFormattedRow();
        current = current->next;
    }
    std::cout << "----------------------------------------------------------------------------------------------------\n";
    std::cout << " Total Songs: " << sizeCount << "\n\n";
}

void DoublyLinkedList::displayReverse() const {
    if (isEmpty()) {
        std::cout << "\n[!] Playlist is empty.\n";
        return;
    }

    std::cout << "\n----------------------------------------------------------------------------------------------------\n";
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

    Node* current = tail;
    while (current != nullptr) {
        current->data.displayFormattedRow();
        current = current->prev;
    }
    std::cout << "----------------------------------------------------------------------------------------------------\n";
    std::cout << " Total Songs (Reversed View): " << sizeCount << "\n\n";
}
