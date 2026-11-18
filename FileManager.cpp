#include "FileManager.h"
#include <fstream>
#include <iostream>

bool FileManager::saveToFile(const std::string& filename, const DoublyLinkedList& list) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "[!] Error: Could not open file '" << filename << "' for writing.\n";
        return false;
    }

    Node* current = list.getHead();
    while (current != nullptr) {
        outFile << current->data.serialize() << "\n";
        current = current->next;
    }

    outFile.close();
    return true;
}

bool FileManager::loadFromFile(const std::string& filename, DoublyLinkedList& list, int& maxIdFound) {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        std::cout << "[i] Note: File '" << filename << "' not found. Initializing empty playlist.\n";
        return false;
    }

    list.clear();
    maxIdFound = 0;

    std::string line;
    while (std::getline(inFile, line)) {
        if (line.empty()) continue;
        Song song = Song::deserialize(line);
        if (song.getId() > 0) {
            list.append(song);
            if (song.getId() > maxIdFound) {
                maxIdFound = song.getId();
            }
        }
    }

    inFile.close();
    return true;
}
