#ifndef PLAYLIST_H
#define PLAYLIST_H

#include "Song.h"
#include "DoublyLinkedList.h"
#include "Stack.h"
#include "Queue.h"
#include "BST.h"
#include <unordered_map>
#include <string>
#include <vector>

enum class PlaybackStatus {
    STOPPED,
    PLAYING,
    PAUSED
};

class Playlist {
private:
    DoublyLinkedList list;
    std::unordered_map<int, Node*> idMap; // O(1) direct lookup by Song ID
    Stack undoStack;                     // Stack for Undo Delete/Update
    Queue recentlyPlayedQueue;           // Queue for Recently Played history
    
    Node* currentlyPlaying;
    PlaybackStatus playbackStatus;
    int nextAutoId;

    void rebuildIdMap();

public:
    Playlist();
    ~Playlist();

    Playlist(const Playlist& other) = delete;
    Playlist& operator=(const Playlist& other) = delete;

    // Core Playlist Operations
    bool addSong(const Song& song);
    bool deleteSongById(int id);
    bool updateSongById(int id, const Song& newSongDetails);
    
    // Playback Operations
    bool playSongById(int id);
    bool playCurrent();
    bool playNext();
    bool playPrevious();
    bool pausePlayback();
    bool resumePlayback();
    bool stopPlayback();
    bool togglePauseResume();    // Pause if playing, resume if paused
    bool isPlaying() const;      // True when status == PLAYING
    bool isPaused()  const;      // True when status == PAUSED
    void displayPlaybackStatus() const;
    void displayRecentlyPlayed() const;

    // Navigation & Manipulation
    bool moveSongPosition(int oldPos, int newPos);
    void reversePlaylist();
    void shufflePlaylist();
    void sortPlaylist(SortCriteria criteria);

    // Favorites
    bool markFavorite(int id, bool fav);
    void displayFavorites() const;

    // Search Functions
    Song* searchById(int id) const; // O(1) via Hash Map
    std::vector<Song> searchByTitle(const std::string& title) const;
    std::vector<Song> searchByArtist(const std::string& artist) const;
    std::vector<Song> searchByAlbum(const std::string& album) const;
    std::vector<Song> searchByGenre(const std::string& genre) const;

    // Undo Operations
    bool undoLastAction();

    // Display & Analytics Reports
    void displayPlaylistForward() const;
    void displayPlaylistReverse() const;
    void generateAnalyticsReport() const;

    // File Persistence
    bool savePlaylistToFile(const std::string& filename);
    bool loadPlaylistFromFile(const std::string& filename);

    // Helpers
    int generateNextId();
    int getSongCount() const;
    int getTotalDurationSeconds() const;
};

#endif // PLAYLIST_H
