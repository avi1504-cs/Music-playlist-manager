#include "BST.h"
#include <algorithm>
#include <cctype>

BST::BST(SortCriteria crit) : root(nullptr), criteria(crit) {}

BST::~BST() {
    clear();
}

void BST::clear() {
    clearHelper(root);
    root = nullptr;
}

void BST::clearHelper(BSTNode* node) {
    if (node == nullptr) return;
    clearHelper(node->left);
    clearHelper(node->right);
    delete node;
}

void BST::setCriteria(SortCriteria crit) {
    this->criteria = crit;
}

static std::string toLowerStr(const std::string& str) {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return lower;
}

bool BST::isLessThan(const Song& a, const Song& b) const {
    switch (criteria) {
        case SortCriteria::TITLE:
            return toLowerStr(a.getTitle()) < toLowerStr(b.getTitle());
        case SortCriteria::ARTIST:
            return toLowerStr(a.getArtist()) < toLowerStr(b.getArtist());
        case SortCriteria::RATING:
            return a.getRating() > b.getRating(); // Descending order for rating
        case SortCriteria::DURATION:
            return a.getDurationSeconds() < b.getDurationSeconds();
        case SortCriteria::YEAR:
            return a.getYear() < b.getYear();
        default:
            return a.getId() < b.getId();
    }
}

void BST::insert(const Song& song) {
    root = insertHelper(root, song);
}

BSTNode* BST::insertHelper(BSTNode* node, const Song& song) {
    if (node == nullptr) {
        return new BSTNode(song);
    }

    if (isLessThan(song, node->song)) {
        node->left = insertHelper(node->left, song);
    } else {
        node->right = insertHelper(node->right, song);
    }
    return node;
}

void BST::displaySorted() const {
    if (root == nullptr) {
        std::cout << "\n[!] BST is empty.\n";
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
    inorderHelper(root);
    std::cout << "----------------------------------------------------------------------------------------------------\n\n";
}

void BST::inorderHelper(BSTNode* node) const {
    if (node == nullptr) return;
    inorderHelper(node->left);
    node->song.displayFormattedRow();
    inorderHelper(node->right);
}

std::vector<Song> BST::searchByArtist(const std::string& artist) const {
    std::vector<Song> results;
    searchArtistHelper(root, toLowerStr(artist), results);
    return results;
}

void BST::searchArtistHelper(BSTNode* node, const std::string& targetArtist, std::vector<Song>& results) const {
    if (node == nullptr) return;
    searchArtistHelper(node->left, targetArtist, results);
    if (toLowerStr(node->song.getArtist()).find(targetArtist) != std::string::npos) {
        results.push_back(node->song);
    }
    searchArtistHelper(node->right, targetArtist, results);
}

std::vector<Song> BST::searchByTitle(const std::string& title) const {
    std::vector<Song> results;
    searchTitleHelper(root, toLowerStr(title), results);
    return results;
}

void BST::searchTitleHelper(BSTNode* node, const std::string& targetTitle, std::vector<Song>& results) const {
    if (node == nullptr) return;
    searchTitleHelper(node->left, targetTitle, results);
    if (toLowerStr(node->song.getTitle()).find(targetTitle) != std::string::npos) {
        results.push_back(node->song);
    }
    searchTitleHelper(node->right, targetTitle, results);
}
