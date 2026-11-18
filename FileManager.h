#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "DoublyLinkedList.h"
#include <string>

class FileManager {
public:
    static bool saveToFile(const std::string& filename, const DoublyLinkedList& list);
    static bool loadFromFile(const std::string& filename, DoublyLinkedList& list, int& maxIdFound);
};

#endif // FILE_MANAGER_H
