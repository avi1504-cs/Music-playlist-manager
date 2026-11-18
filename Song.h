#ifndef SONG_H
#define SONG_H

#include <string>
#include <iostream>
#include <iomanip>

class Song {
private:
    int id;
    std::string title;
    std::string artist;
    std::string album;
    std::string genre;
    int durationSeconds; // Duration in total seconds
    int year;
    double rating;       // Rating from 1.0 to 5.0
    bool favorite;
    std::string audioPath; // Path to MP3 audio file

public:
    // Constructors
    Song();
    Song(int id, const std::string& title, const std::string& artist, 
         const std::string& album, const std::string& genre, 
         int durationSeconds, int year, double rating, bool favorite = false,
         const std::string& audioPath = "default.mp3");

    // Getters
    int getId() const;
    std::string getTitle() const;
    std::string getArtist() const;
    std::string getAlbum() const;
    std::string getGenre() const;
    int getDurationSeconds() const;
    std::string getFormattedDuration() const; // Formats to MM:SS
    int getYear() const;
    double getRating() const;
    bool isFavorite() const;
    std::string getAudioPath() const;

    // Setters
    void setId(int id);
    void setTitle(const std::string& title);
    void setArtist(const std::string& artist);
    void setAlbum(const std::string& album);
    void setGenre(const std::string& genre);
    void setDurationSeconds(int durationSeconds);
    void setYear(int year);
    void setRating(double rating);
    void setFavorite(bool fav);
    void setAudioPath(const std::string& path);

    // Helper functions
    void displayFormattedRow() const;
    void displayCard() const;
    std::string serialize() const; // For file storage (delimiter separated)
    static Song deserialize(const std::string& line);
};

#endif // SONG_H
