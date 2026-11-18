#include "Playlist.h"
#include "FileManager.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>

static HANDLE g_hFallbackPlaybackProcess = NULL;

static void stopAudioConsole() {
    mciSendStringA("stop console_audio", NULL, 0, NULL);
    mciSendStringA("close console_audio", NULL, 0, NULL);
    if (g_hFallbackPlaybackProcess != NULL) {
        TerminateProcess(g_hFallbackPlaybackProcess, 0);
        CloseHandle(g_hFallbackPlaybackProcess);
        g_hFallbackPlaybackProcess = NULL;
    }
    std::cout << "[⏹ AUDIO ENGINE] Console audio playback stopped.\n";
}

static void pauseAudioConsole() {
    mciSendStringA("pause console_audio", NULL, 0, NULL);
    std::cout << "[⏸ AUDIO ENGINE] Console audio playback paused.\n";
}

static void resumeAudioConsole() {
    mciSendStringA("resume console_audio", NULL, 0, NULL);
    std::cout << "[▶ AUDIO ENGINE] Console audio playback resumed.\n";
}

static void playAudioConsole(const std::string& path) {
    stopAudioConsole();

    std::string cleanedPath = path;
    // Trim whitespace
    size_t first = cleanedPath.find_first_not_of(" \t\r\n");
    if (first != std::string::npos) {
        size_t last = cleanedPath.find_last_not_of(" \t\r\n");
        cleanedPath = cleanedPath.substr(first, (last - first + 1));
    } else {
        cleanedPath.clear();
    }
    // Strip surrounding quotes (both double and single)
    while (cleanedPath.size() >= 2 &&
           ((cleanedPath.front() == '"' && cleanedPath.back() == '"') ||
            (cleanedPath.front() == '\'' && cleanedPath.back() == '\''))) {
        cleanedPath = cleanedPath.substr(1, cleanedPath.size() - 2);
    }

    if (cleanedPath.empty() || cleanedPath == "default.mp3") {
        std::cout << "[i] No audio file path set. Playing synth tones.\n";
        Beep(523, 200); Beep(659, 200); Beep(784, 300);
        return;
    }

    std::error_code fileError;
    std::filesystem::path audioPath(cleanedPath);

    // Resolve relative path against working directory
    if (!std::filesystem::exists(audioPath, fileError)) {
        std::filesystem::path localCandidate = std::filesystem::current_path() / audioPath;
        if (std::filesystem::exists(localCandidate, fileError)) {
            audioPath = localCandidate;
        }
    }

    if (!std::filesystem::exists(audioPath, fileError) ||
        !std::filesystem::is_regular_file(audioPath, fileError)) {
        std::cerr << "[!] Audio file not found: " << cleanedPath << "\n";
        std::cout << "[🎵 AUDIO ENGINE] Playing synth melody instead...\n";
        Beep(523, 200); Beep(659, 200); Beep(784, 300);
        return;
    }

    audioPath = std::filesystem::absolute(audioPath, fileError).lexically_normal();
    std::wstring absPathW = audioPath.wstring();

    // Obtain 8.3 short path name to avoid spaces/unicode issues with MCI
    wchar_t shortPath[MAX_PATH] = {0};
    DWORD spLen = GetShortPathNameW(absPathW.c_str(), shortPath, MAX_PATH);
    std::wstring targetPath = (spLen > 0 && spLen < MAX_PATH) ? shortPath : absPathW;

    std::string ext = audioPath.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    bool success = false;

    if (ext == ".wav") {
        // WAV: use waveaudio MCI device (always supported)
        std::wstring command = L"open \"" + targetPath + L"\" type waveaudio alias console_audio";
        MCIERROR err = mciSendStringW(command.c_str(), NULL, 0, NULL);
        if (err == 0) {
            err = mciSendStringW(L"play console_audio", NULL, 0, NULL);
            success = (err == 0);
        }
        if (!success) {
            // Fallback: PlaySound API for WAV
            success = (PlaySoundW(absPathW.c_str(), NULL, SND_ASYNC | SND_FILENAME) == TRUE);
        }
    } else {
        // MP3/M4A/OGG etc: try MCI auto-detect first (let Windows choose device)
        // Note: "mpegvideo" is invalid on most systems. Use auto-detect (no type) instead.
        std::wstring command = L"open \"" + targetPath + L"\" alias console_audio";
        MCIERROR err = mciSendStringW(command.c_str(), NULL, 0, NULL);
        if (err == 0) {
            err = mciSendStringW(L"play console_audio", NULL, 0, NULL);
            if (err == 0) {
                success = true;
            } else {
                wchar_t errBuf[256] = {0};
                mciGetErrorStringW(err, errBuf, 256);
                std::cerr << "[MCI] Play failed: " << err << " - ";
                std::wcerr << errBuf << L"\n";
                mciSendStringW(L"close console_audio", NULL, 0, NULL);
            }
        } else {
            wchar_t errBuf[256] = {0};
            mciGetErrorStringW(err, errBuf, 256);
            std::cerr << "[MCI] Open failed: " << err << " - ";
            std::wcerr << errBuf << L"\n";
        }
    }

    if (success) {
        std::cout << "[AUDIO] Now playing: " << audioPath.filename().string() << "\n";
        std::cout << "    [Source: " << audioPath.string() << "]\n";
        return;
    }

    // Final fallback: use Windows PowerShell + Windows Media Player COM
    // This plays virtually any audio/video format that WMP supports.
    std::wstring escapedPath = absPathW;
    // Escape single quotes in path for PowerShell string
    for (size_t i = 0; i < escapedPath.size(); ++i) {
        if (escapedPath[i] == L'\'') {
            escapedPath.insert(i, L"'");
            i++;
        }
    }
    std::wstring psCommand = 
        L"powershell.exe -NoProfile -NonInteractive -WindowStyle Hidden -Command "
        L"\"Add-Type -AssemblyName presentationCore; "
        L"$p = [System.Windows.Media.MediaPlayer]::new(); "
        L"$p.Open([System.Uri]::new('" + escapedPath + L"')); "
        L"$p.Play(); "
        L"Start-Sleep -Seconds 1; "
        L"while ($p.IsBuffering -or $p.Position.TotalSeconds -lt $p.NaturalDuration.TimeSpan.TotalSeconds - 0.5) { Start-Sleep -Milliseconds 500 }"  
        L"\"";

    STARTUPINFOW si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    ZeroMemory(&pi, sizeof(pi));

    if (CreateProcessW(NULL, const_cast<LPWSTR>(psCommand.c_str()),
                       NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        g_hFallbackPlaybackProcess = pi.hProcess;
        CloseHandle(pi.hThread);
        std::cout << "[AUDIO] Playing via Windows Audio Subsystem: " << audioPath.filename().string() << "\n";
        return;
    }

    std::cout << "[!] All playback methods failed for: " << cleanedPath << "\n";
    std::cout << "[AUDIO] Playing synth melody notes instead...\n";
    Beep(523, 200);
    Beep(659, 200);
    Beep(784, 300);
}
#else
static void playAudioConsole(const std::string& path) {
    std::cout << "[AUDIO] Playing audio track: " << path << "\n";
}
static void pauseAudioConsole() {}
static void resumeAudioConsole() {}
static void stopAudioConsole() {}
#endif

Playlist::Playlist() 
    : list(), idMap(), undoStack(), recentlyPlayedQueue(10),
      currentlyPlaying(nullptr), playbackStatus(PlaybackStatus::STOPPED), nextAutoId(101) {}

Playlist::~Playlist() {}

void Playlist::rebuildIdMap() {
    idMap.clear();
    Node* curr = list.getHead();
    while (curr != nullptr) {
        idMap[curr->data.getId()] = curr;
        curr = curr->next;
    }
}

int Playlist::generateNextId() {
    while (idMap.find(nextAutoId) != idMap.end()) {
        nextAutoId++;
    }
    return nextAutoId;
}

int Playlist::getSongCount() const {
    return list.getSize();
}

int Playlist::getTotalDurationSeconds() const {
    int total = 0;
    Node* curr = list.getHead();
    while (curr != nullptr) {
        total += curr->data.getDurationSeconds();
        curr = curr->next;
    }
    return total;
}

bool Playlist::addSong(const Song& song) {
    if (idMap.find(song.getId()) != idMap.end()) {
        std::cout << "[!] Error: Song ID " << song.getId() << " already exists!\n";
        return false;
    }

    list.append(song);
    Node* newNode = list.getTail();
    idMap[song.getId()] = newNode;

    if (song.getId() >= nextAutoId) {
        nextAutoId = song.getId() + 1;
    }

    std::cout << "[✓] Successfully added: \"" << song.getTitle() << "\" by " << song.getArtist() << "\n";
    return true;
}

bool Playlist::deleteSongById(int id) {
    auto it = idMap.find(id);
    if (it == idMap.end()) {
        std::cout << "[!] Song ID " << id << " not found in playlist.\n";
        return false;
    }

    // Find 1-based position for Undo restoration
    Node* nodeToDelete = it->second;
    int pos = 1;
    Node* curr = list.getHead();
    while (curr != nullptr && curr != nodeToDelete) {
        curr = curr->next;
        pos++;
    }

    // Handle currently playing node pointer cleanup
    if (currentlyPlaying == nodeToDelete) {
        currentlyPlaying = nodeToDelete->next ? nodeToDelete->next : nodeToDelete->prev;
        if (currentlyPlaying == nullptr) {
            playbackStatus = PlaybackStatus::STOPPED;
            stopAudioConsole();
        }
    }

    // Save state to Undo stack before deletion
    Song songToDelete = nodeToDelete->data;
    undoStack.push(UndoAction(UndoType::DELETE_ACTION, songToDelete, pos));

    // Remove from Doubly LinkedList & Hash Map
    Song tempOut;
    list.removeById(id, tempOut);
    idMap.erase(it);

    std::cout << "[✓] Deleted song: \"" << songToDelete.getTitle() << "\" (ID: " << id << ")\n";
    std::cout << "    [Note: You can undo this action from the menu]\n";
    return true;
}

bool Playlist::updateSongById(int id, const Song& newDetails) {
    auto it = idMap.find(id);
    if (it == idMap.end()) {
        std::cout << "[!] Song ID " << id << " not found.\n";
        return false;
    }

    Node* targetNode = it->second;
    Song oldSong = targetNode->data;

    // Find position for undo
    int pos = 1;
    Node* curr = list.getHead();
    while (curr != nullptr && curr != targetNode) {
        curr = curr->next;
        pos++;
    }

    // Save old state to stack
    undoStack.push(UndoAction(UndoType::UPDATE_ACTION, oldSong, pos));

    // Apply updates
    targetNode->data = newDetails;

    std::cout << "[✓] Updated details for Song ID " << id << " (\"" << newDetails.getTitle() << "\")\n";
    return true;
}

bool Playlist::undoLastAction() {
    if (undoStack.isEmpty()) {
        std::cout << "[!] Nothing to undo.\n";
        return false;
    }

    UndoAction action;
    undoStack.pop(action);

    if (action.type == UndoType::DELETE_ACTION) {
        // Re-insert song at original position
        list.insertAt(action.originalPosition, action.songData);
        rebuildIdMap();
        std::cout << "[✓] UNDO SUCCESS: Restored deleted song \"" << action.songData.getTitle() 
                  << "\" (ID: " << action.songData.getId() << ") at position " << action.originalPosition << ".\n";
        return true;
    } else if (action.type == UndoType::UPDATE_ACTION) {
        // Revert details of the song
        auto it = idMap.find(action.songData.getId());
        if (it != idMap.end()) {
            it->second->data = action.songData;
            std::cout << "[✓] UNDO SUCCESS: Reverted updates for song \"" << action.songData.getTitle() 
                      << "\" (ID: " << action.songData.getId() << ").\n";
            return true;
        } else {
            std::cout << "[!] Undo failed: Target song no longer exists in map.\n";
            return false;
        }
    }
    return false;
}

Song* Playlist::searchById(int id) const {
    auto it = idMap.find(id);
    if (it != idMap.end()) {
        return &(it->second->data);
    }
    return nullptr;
}

static std::string toLowerStr(const std::string& str) {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return lower;
}

std::vector<Song> Playlist::searchByTitle(const std::string& title) const {
    BST bst(SortCriteria::TITLE);
    Node* curr = list.getHead();
    while (curr) {
        bst.insert(curr->data);
        curr = curr->next;
    }
    return bst.searchByTitle(title);
}

std::vector<Song> Playlist::searchByArtist(const std::string& artist) const {
    BST bst(SortCriteria::ARTIST);
    Node* curr = list.getHead();
    while (curr) {
        bst.insert(curr->data);
        curr = curr->next;
    }
    return bst.searchByArtist(artist);
}

std::vector<Song> Playlist::searchByAlbum(const std::string& album) const {
    std::vector<Song> results;
    std::string target = toLowerStr(album);
    Node* curr = list.getHead();
    while (curr) {
        if (toLowerStr(curr->data.getAlbum()).find(target) != std::string::npos) {
            results.push_back(curr->data);
        }
        curr = curr->next;
    }
    return results;
}

std::vector<Song> Playlist::searchByGenre(const std::string& genre) const {
    std::vector<Song> results;
    std::string target = toLowerStr(genre);
    Node* curr = list.getHead();
    while (curr) {
        if (toLowerStr(curr->data.getGenre()).find(target) != std::string::npos) {
            results.push_back(curr->data);
        }
        curr = curr->next;
    }
    return results;
}

bool Playlist::playSongById(int id) {
    auto it = idMap.find(id);
    if (it == idMap.end()) {
        std::cout << "[!] Song ID " << id << " not found in playlist.\n";
        return false;
    }

    currentlyPlaying = it->second;
    playbackStatus = PlaybackStatus::PLAYING;

    // Add to Recently Played Queue
    recentlyPlayedQueue.enqueue(currentlyPlaying->data);

    std::cout << "\n▶ NOW PLAYING:\n";
    currentlyPlaying->data.displayCard();
    playAudioConsole(currentlyPlaying->data.getAudioPath());
    return true;
}

bool Playlist::playCurrent() {
    if (currentlyPlaying == nullptr) {
        currentlyPlaying = list.getHead();
    }

    if (currentlyPlaying == nullptr) {
        std::cout << "[!] Playlist is empty. Cannot play.\n";
        return false;
    }

    playbackStatus = PlaybackStatus::PLAYING;
    recentlyPlayedQueue.enqueue(currentlyPlaying->data);

    std::cout << "\n▶ NOW PLAYING:\n";
    currentlyPlaying->data.displayCard();
    playAudioConsole(currentlyPlaying->data.getAudioPath());
    return true;
}

bool Playlist::playNext() {
    if (currentlyPlaying == nullptr) {
        return playCurrent();
    }

    if (currentlyPlaying->next == nullptr) {
        std::cout << "[i] End of playlist reached. Loop to start? (Playing first song)\n";
        currentlyPlaying = list.getHead();
    } else {
        currentlyPlaying = currentlyPlaying->next;
    }

    if (currentlyPlaying == nullptr) return false;

    playbackStatus = PlaybackStatus::PLAYING;
    recentlyPlayedQueue.enqueue(currentlyPlaying->data);

    std::cout << "\n⏭ NEXT SONG:\n";
    currentlyPlaying->data.displayCard();
    playAudioConsole(currentlyPlaying->data.getAudioPath());
    return true;
}

bool Playlist::playPrevious() {
    if (currentlyPlaying == nullptr) {
        return playCurrent();
    }

    if (currentlyPlaying->prev == nullptr) {
        std::cout << "[i] Start of playlist reached. Playing last song.\n";
        currentlyPlaying = list.getTail();
    } else {
        currentlyPlaying = currentlyPlaying->prev;
    }

    if (currentlyPlaying == nullptr) return false;

    playbackStatus = PlaybackStatus::PLAYING;
    recentlyPlayedQueue.enqueue(currentlyPlaying->data);

    std::cout << "\n[<<] PREVIOUS SONG:\n";
    currentlyPlaying->data.displayCard();
    playAudioConsole(currentlyPlaying->data.getAudioPath());
    return true;
}

bool Playlist::pausePlayback() {
    if (playbackStatus == PlaybackStatus::PLAYING) {
        playbackStatus = PlaybackStatus::PAUSED;
        std::cout << "[PAUSED] Playback Paused: \"" << currentlyPlaying->data.getTitle() << "\"\n";
        pauseAudioConsole();
        return true;
    }
    std::cout << "[!] No active track playing to pause.\n";
    return false;
}

bool Playlist::resumePlayback() {
    if (playbackStatus == PlaybackStatus::PAUSED && currentlyPlaying != nullptr) {
        playbackStatus = PlaybackStatus::PLAYING;
        std::cout << "[PLAYING] Playback Resumed: \"" << currentlyPlaying->data.getTitle() << "\"\n";
        resumeAudioConsole();
        return true;
    }
    std::cout << "[!] Playback is not paused.\n";
    return false;
}

bool Playlist::stopPlayback() {
    playbackStatus = PlaybackStatus::STOPPED;
    std::cout << "[STOPPED] Playback Stopped.\n";
    stopAudioConsole();
    return true;
}

bool Playlist::togglePauseResume() {
    if (playbackStatus == PlaybackStatus::PLAYING) {
        return pausePlayback();
    } else if (playbackStatus == PlaybackStatus::PAUSED) {
        return resumePlayback();
    }
    std::cout << "[!] No active track to pause or resume.\n";
    return false;
}

bool Playlist::isPlaying() const {
    return playbackStatus == PlaybackStatus::PLAYING;
}

bool Playlist::isPaused() const {
    return playbackStatus == PlaybackStatus::PAUSED;
}

void Playlist::displayPlaybackStatus() const {
    std::cout << "====================================================\n";
    std::cout << "               PLAYER STATUS BAR                    \n";
    std::cout << "====================================================\n";
    std::string statusStr = "STOPPED";
    if (playbackStatus == PlaybackStatus::PLAYING) statusStr = "PLAYING";
    else if (playbackStatus == PlaybackStatus::PAUSED) statusStr = "PAUSED";

    std::cout << " State   : " << statusStr << "\n";
    if (currentlyPlaying != nullptr) {
        std::cout << " Track   : #" << currentlyPlaying->data.getId() << " - " 
                  << currentlyPlaying->data.getTitle() << " by " << currentlyPlaying->data.getArtist() << "\n";
        std::cout << " Time    : " << currentlyPlaying->data.getFormattedDuration() << "\n";
    } else {
        std::cout << " Track   : None\n";
    }
    std::cout << "====================================================\n";
}

void Playlist::displayRecentlyPlayed() const {
    recentlyPlayedQueue.display();
}

bool Playlist::moveSongPosition(int oldPos, int newPos) {
    bool success = list.movePosition(oldPos, newPos);
    if (success) {
        rebuildIdMap();
        std::cout << "[OK] Successfully moved song from position " << oldPos << " to position " << newPos << ".\n";
    } else {
        std::cout << "[!] Failed to move song. Check valid position indices (1 to " << list.getSize() << ").\n";
    }
    return success;
}

void Playlist::reversePlaylist() {
    list.reverse();
    rebuildIdMap();
    std::cout << "[OK] Playlist order reversed in-place.\n";
}

void Playlist::shufflePlaylist() {
    list.shuffle();
    rebuildIdMap();
    std::cout << "[OK] Playlist shuffled randomly using Fisher-Yates algorithm.\n";
}

void Playlist::sortPlaylist(SortCriteria criteria) {
    if (list.isEmpty()) {
        std::cout << "[!] Playlist is empty. Nothing to sort.\n";
        return;
    }

    std::string critName = "Title";
    if (criteria == SortCriteria::ARTIST) critName = "Artist";
    else if (criteria == SortCriteria::RATING) critName = "Rating";
    else if (criteria == SortCriteria::DURATION) critName = "Duration";
    else if (criteria == SortCriteria::YEAR) critName = "Year";

    std::cout << "\n====================================================================================================\n";
    std::cout << "                         SORTED PLAYLIST VIEW (BST - " << critName << ")                          \n";
    std::cout << "====================================================================================================\n";

    BST bst(criteria);
    Node* curr = list.getHead();
    while (curr != nullptr) {
        bst.insert(curr->data);
        curr = curr->next;
    }
    bst.displaySorted();
}

bool Playlist::markFavorite(int id, bool fav) {
    Song* song = searchById(id);
    if (song != nullptr) {
        song->setFavorite(fav);
        std::cout << "[✓] Song ID " << id << " (" << song->getTitle() << ") " 
                  << (fav ? "marked as Favorite ★" : "removed from Favorites") << ".\n";
        return true;
    }
    std::cout << "[!] Song ID " << id << " not found.\n";
    return false;
}

void Playlist::displayFavorites() const {
    if (list.isEmpty()) {
        std::cout << "\n[!] Playlist is empty.\n";
        return;
    }

    std::cout << "\n====================================================================================================\n";
    std::cout << "                                  FAVORITE SONGS LIST                                              \n";
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

    int favCount = 0;
    Node* curr = list.getHead();
    while (curr != nullptr) {
        if (curr->data.isFavorite()) {
            curr->data.displayFormattedRow();
            favCount++;
        }
        curr = curr->next;
    }
    std::cout << "----------------------------------------------------------------------------------------------------\n";
    std::cout << " Total Favorites: " << favCount << "\n\n";
}

void Playlist::generateAnalyticsReport() const {
    if (list.isEmpty()) {
        std::cout << "\n[!] Playlist is empty. No report available.\n";
        return;
    }

    Node* longestSong = list.getHead();
    Node* shortestSong = list.getHead();
    Node* highestRated = list.getHead();

    double totalRating = 0.0;
    int totalDur = 0;
    int songCount = 0;

    Node* curr = list.getHead();
    while (curr != nullptr) {
        songCount++;
        totalDur += curr->data.getDurationSeconds();
        totalRating += curr->data.getRating();

        if (curr->data.getDurationSeconds() > longestSong->data.getDurationSeconds()) {
            longestSong = curr;
        }
        if (curr->data.getDurationSeconds() < shortestSong->data.getDurationSeconds()) {
            shortestSong = curr;
        }
        if (curr->data.getRating() > highestRated->data.getRating()) {
            highestRated = curr;
        }

        curr = curr->next;
    }

    double avgRating = (songCount > 0) ? (totalRating / songCount) : 0.0;
    int totalMins = totalDur / 60;
    int totalSecs = totalDur % 60;

    std::cout << "\n====================================================================================================\n";
    std::cout << "                                 PLAYLIST ANALYTICS & REPORTS                                       \n";
    std::cout << "====================================================================================================\n";
    std::cout << " Total Songs Count      : " << songCount << "\n";
    std::cout << " Total Duration         : " << totalMins << " mins " << totalSecs << " secs (" << totalDur << " seconds)\n";
    std::cout << " Average Rating         : " << std::fixed << std::setprecision(2) << avgRating << " / 5.0\n";
    std::cout << "----------------------------------------------------------------------------------------------------\n";
    std::cout << " 🎵 Longest Song        : \"" << longestSong->data.getTitle() << "\" by " << longestSong->data.getArtist()
              << " (" << longestSong->data.getFormattedDuration() << ")\n";
    std::cout << " 🎵 Shortest Song       : \"" << shortestSong->data.getTitle() << "\" by " << shortestSong->data.getArtist()
              << " (" << shortestSong->data.getFormattedDuration() << ")\n";
    std::cout << " ★ Highest Rated Song   : \"" << highestRated->data.getTitle() << "\" by " << highestRated->data.getArtist()
              << " (" << std::fixed << std::setprecision(1) << highestRated->data.getRating() << " / 5.0)\n";
    std::cout << "====================================================================================================\n\n";
}

void Playlist::displayPlaylistForward() const {
    list.displayForward();
}

void Playlist::displayPlaylistReverse() const {
    list.displayReverse();
}

bool Playlist::savePlaylistToFile(const std::string& filename) {
    return FileManager::saveToFile(filename, list);
}

bool Playlist::loadPlaylistFromFile(const std::string& filename) {
    int maxId = 0;
    bool success = FileManager::loadFromFile(filename, list, maxId);
    if (success) {
        rebuildIdMap();
        if (maxId >= nextAutoId) {
            nextAutoId = maxId + 1;
        }
        std::cout << "[✓] Loaded " << list.getSize() << " songs from '" << filename << "'.\n";
    }
    return success;
}

