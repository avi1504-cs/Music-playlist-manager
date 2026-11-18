#include "Song.h"
#include <sstream>
#include <iomanip>
#include <cmath>

Song::Song() 
    : id(0), title("Unknown"), artist("Unknown"), album("Unknown"), 
      genre("Unknown"), durationSeconds(0), year(2000), rating(0.0), favorite(false), audioPath("default.mp3") {}

Song::Song(int id, const std::string& title, const std::string& artist, 
           const std::string& album, const std::string& genre, 
           int durationSeconds, int year, double rating, bool favorite, const std::string& audioPath)
    : id(id), title(title), artist(artist), album(album), genre(genre),
      durationSeconds(durationSeconds), year(year), rating(rating), favorite(favorite), audioPath(audioPath) {}

int Song::getId() const { return id; }
std::string Song::getTitle() const { return title; }
std::string Song::getArtist() const { return artist; }
std::string Song::getAlbum() const { return album; }
std::string Song::getGenre() const { return genre; }
int Song::getDurationSeconds() const { return durationSeconds; }

std::string Song::getFormattedDuration() const {
    int minutes = durationSeconds / 60;
    int seconds = durationSeconds % 60;
    std::stringstream ss;
    ss << std::setw(2) << std::setfill('0') << minutes << ":"
       << std::setw(2) << std::setfill('0') << seconds;
    return ss.str();
}

int Song::getYear() const { return year; }
double Song::getRating() const { return rating; }
bool Song::isFavorite() const { return favorite; }
std::string Song::getAudioPath() const { return audioPath; }

void Song::setId(int id) { this->id = id; }
void Song::setTitle(const std::string& title) { this->title = title; }
void Song::setArtist(const std::string& artist) { this->artist = artist; }
void Song::setAlbum(const std::string& album) { this->album = album; }
void Song::setGenre(const std::string& genre) { this->genre = genre; }
void Song::setDurationSeconds(int durationSeconds) { this->durationSeconds = durationSeconds; }
void Song::setYear(int year) { this->year = year; }
void Song::setRating(double rating) { this->rating = rating; }
void Song::setFavorite(bool fav) { this->favorite = fav; }
void Song::setAudioPath(const std::string& path) { this->audioPath = path; }

void Song::displayFormattedRow() const {
    std::cout << std::left 
              << std::setw(6)  << id
              << std::setw(22) << (title.length() > 20 ? title.substr(0, 17) + "..." : title)
              << std::setw(18) << (artist.length() > 16 ? artist.substr(0, 13) + "..." : artist)
              << std::setw(18) << (album.length() > 16 ? album.substr(0, 13) + "..." : album)
              << std::setw(12) << genre
              << std::setw(10) << getFormattedDuration()
              << std::setw(8)  << year
              << std::setw(8)  << std::fixed << std::setprecision(1) << rating
              << std::setw(10) << (favorite ? "[Fav]" : "  - ")
              << std::endl;
}

void Song::displayCard() const {
    std::cout << "====================================================\n";
    std::cout << " ID            : " << id << "\n";
    std::cout << " Title         : " << title << "\n";
    std::cout << " Artist        : " << artist << "\n";
    std::cout << " Album         : " << album << "\n";
    std::cout << " Genre         : " << genre << "\n";
    std::cout << " Duration      : " << getFormattedDuration() << " (" << durationSeconds << "s)\n";
    std::cout << " Year          : " << year << "\n";
    std::cout << " Rating        : " << std::fixed << std::setprecision(1) << rating << " / 5.0\n";
    std::cout << " Status        : " << (favorite ? "Favorite ★" : "Normal") << "\n";
    std::cout << " MP3 Audio File: " << (audioPath.empty() ? "default.mp3" : audioPath) << "\n";
    std::cout << "====================================================\n";
}

std::string Song::serialize() const {
    // Delimited format: ID|Title|Artist|Album|Genre|Duration|Year|Rating|Favorite|AudioPath
    std::stringstream ss;
    ss << id << "|" << title << "|" << artist << "|" << album << "|"
       << genre << "|" << durationSeconds << "|" << year << "|" 
       << rating << "|" << (favorite ? 1 : 0) << "|" << (audioPath.empty() ? "default.mp3" : audioPath);
    return ss.str();
}

Song Song::deserialize(const std::string& line) {
    if (line.empty()) return Song();

    std::stringstream ss(line);
    std::string token;
    std::string tokens[10];
    int idx = 0;

    while (std::getline(ss, token, '|') && idx < 10) {
        tokens[idx++] = token;
    }

    if (idx < 9) return Song();

    try {
        int sId = std::stoi(tokens[0]);
        std::string sTitle = tokens[1];
        std::string sArtist = tokens[2];
        std::string sAlbum = tokens[3];
        std::string sGenre = tokens[4];
        int sDur = std::stoi(tokens[5]);
        int sYr = std::stoi(tokens[6]);
        double sRat = std::stod(tokens[7]);
        bool sFav = (std::stoi(tokens[8]) == 1);
        std::string sAudio = (idx >= 10 && !tokens[9].empty()) ? tokens[9] : "default.mp3";

        return Song(sId, sTitle, sArtist, sAlbum, sGenre, sDur, sYr, sRat, sFav, sAudio);
    } catch (...) {
        return Song();
    }
}
