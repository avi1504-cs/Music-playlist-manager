#ifndef BST_H
#define BST_H

#include "Song.h"
#include <iostream>
#include <vector>

enum class SortCriteria {
    TITLE,
    ARTIST,
    RATING,
    DURATION,
    YEAR
};

struct BSTNode {
    Song song;
    BSTNode* left;
    BSTNode* right;

    BSTNode(const Song& s) : song(s), left(nullptr), right(nullptr) {}
};

class BST {
private:
    BSTNode* root;
    SortCriteria criteria;

    BSTNode* insertHelper(BSTNode* node, const Song& song);
    void inorderHelper(BSTNode* node) const;
    void searchArtistHelper(BSTNode* node, const std::string& artist, std::vector<Song>& results) const;
    void searchTitleHelper(BSTNode* node, const std::string& title, std::vector<Song>& results) const;
    void clearHelper(BSTNode* node);

    // Comparator function based on active criteria
    bool isLessThan(const Song& a, const Song& b) const;

public:
    BST(SortCriteria crit = SortCriteria::TITLE);
    ~BST();

    BST(const BST& other) = delete;
    BST& operator=(const BST& other) = delete;

    void setCriteria(SortCriteria crit);
    void insert(const Song& song);
    void displaySorted() const;

    std::vector<Song> searchByArtist(const std::string& artist) const;
    std::vector<Song> searchByTitle(const std::string& title) const;
    void clear();
};

#endif // BST_H
