#include <iostream>
#include <string>
#include <limits>
#include <cctype>
#include <algorithm>
#include "Playlist.h"

#ifdef _WIN32
#include <windows.h>
#include <commdlg.h>

// Opens native Windows File Explorer / File Manager dialog to choose audio files directly
std::string chooseAudioFileWithDialog() {
    char szFile[MAX_PATH] = {0};
    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = GetConsoleWindow();
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Audio Files (*.mp3;*.wav;*.m4a;*.ogg;*.flac;*.wma)\0*.mp3;*.wav;*.m4a;*.ogg;*.flac;*.wma\0MP3 Files (*.mp3)\0*.mp3\0WAV Files (*.wav)\0*.wav\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
    ofn.lpstrTitle = "Select Music Audio File";

    std::cout << "[FILE MANAGER] Opening Windows File Explorer dialog... Please pick your audio file.\n";
    if (GetOpenFileNameA(&ofn)) {
        return std::string(szFile);
    }
    return "";
}
#else
std::string chooseAudioFileWithDialog() {
    return "";
}
#endif

// ---------------------------------------------------------------
//  NOW PLAYING CONTROLLER
//  Shows an interactive mini-player while audio runs in the
//  background via MCI / PowerShell. Returns when the user
//  explicitly stops or goes back to the main menu.
// ---------------------------------------------------------------
void runPlaybackController(Playlist& playlist) {
    bool inPlayer = true;
    while (inPlayer) {
        // Render the Now-Playing bar
        std::cout << "\n";
        std::cout << "==============================================\n";
        std::cout << "         [NOW PLAYING CONTROLLER]            \n";
        std::cout << "==============================================\n";
        playlist.displayPlaybackStatus();
        std::cout << "----------------------------------------------\n";
        std::cout << "  1  Pause / Resume                          \n";
        std::cout << "  2  Stop Playback                           \n";
        std::cout << "  3  Next Song                               \n";
        std::cout << "  4  Previous Song                           \n";
        std::cout << "  5  Choose a Different Song (by ID)         \n";
        std::cout << "  6  Return to Main Menu (keeps playing)     \n";
        std::cout << "==============================================\n";

        int ctrl;
        std::cout << "Player > ";
        while (!(std::cin >> ctrl) || ctrl < 1 || ctrl > 6) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "[!] Enter 1-6: ";
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (ctrl) {
            case 1:
                // Toggle: pause if playing, resume if paused
                playlist.togglePauseResume();
                break;

            case 2:
                playlist.stopPlayback();
                std::cout << "[STOPPED] Playback stopped. Returning to main menu.\n";
                inPlayer = false;
                break;

            case 3:
                playlist.playNext();
                break;

            case 4:
                playlist.playPrevious();
                break;

            case 5: {
                int sid;
                std::cout << "Enter Song ID to play: ";
                while (!(std::cin >> sid) || sid < 1) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "[!] Enter a valid Song ID: ";
                }
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                playlist.playSongById(sid);
                break;
            }

            case 6:
                std::cout << "[OK] Returning to main menu. Music continues in background.\n";
                inPlayer = false;
                break;
        }
    }
}

// Helper function to validate song title / artist format
bool isValidTextOnly(const std::string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (!std::isalnum(static_cast<unsigned char>(c)) && c != ' ' && c != '-' && c != '\'' && c != '.' && c != '(' && c != ')' && c != '&' && c != ',') {
            return false;
        }
    }
    return true;
}

// Helper function to read Song Title & Artist Name with optional default value
std::string getTextOnlyString(const std::string& prompt, const std::string& defaultValue = "") {
    std::string text;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, text);
        
        size_t first = text.find_first_not_of(" \t\r\n");
        if (first != std::string::npos) {
            size_t last = text.find_last_not_of(" \t\r\n");
            std::string trimmed = text.substr(first, (last - first + 1));
            
            if (isValidTextOnly(trimmed)) {
                return trimmed;
            } else {
                std::cout << "[!] Invalid text format! Please avoid special symbols like %$#^*.\n";
            }
        } else if (!defaultValue.empty()) {
            return defaultValue;
        } else {
            std::cout << "[!] Input field cannot be empty! Please enter valid text.\n";
        }
    }
}

// Helper function to read MP3/audio file path from console with quote stripping
std::string getMP3FilePath(const std::string& prompt) {
    std::string path;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, path);

        size_t first = path.find_first_not_of(" \t\r\n");
        if (first != std::string::npos) {
            size_t last = path.find_last_not_of(" \t\r\n");
            std::string trimmed = path.substr(first, (last - first + 1));

            // Strip double quotes and single quotes (e.g. from PowerShell drag-and-drop or Copy as Path)
            while (trimmed.size() >= 2 && 
                  ((trimmed.front() == '"' && trimmed.back() == '"') ||
                   (trimmed.front() == '\'' && trimmed.back() == '\''))) {
                trimmed = trimmed.substr(1, trimmed.size() - 2);
            }

            return trimmed;
        } else {
            std::cout << "[i] Defaulting to 'default.mp3'\n";
            return "default.mp3";
        }
    }
}

// Helper function to safely read integer input within specified bounds
int getIntInput(const std::string& prompt, int minVal = std::numeric_limits<int>::min(), int maxVal = std::numeric_limits<int>::max()) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            if (value >= minVal && value <= maxVal) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return value;
            } else {
                std::cout << "[!] Input out of valid bounds! Must be between " << minVal << " and " << maxVal << ".\n";
            }
        } else {
            std::cout << "[!] Invalid input! Please enter a valid number.\n";
            std::cin.clear();
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

// Helper function to safely read double input within specified bounds
double getDoubleInput(const std::string& prompt, double minVal = -1e9, double maxVal = 1e9) {
    double value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            if (value >= minVal && value <= maxVal) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return value;
            } else {
                std::cout << "[!] Rating out of bounds! Must be between " << minVal << " and " << maxVal << ".\n";
            }
        } else {
            std::cout << "[!] Invalid input! Please enter a valid numeric decimal number.\n";
            std::cin.clear();
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

// Helper function to read optional string lines
std::string getStringInput(const std::string& prompt) {
    std::string text;
    std::cout << prompt;
    std::getline(std::cin, text);
    size_t first = text.find_first_not_of(" \t\r\n");
    if (first != std::string::npos) {
        size_t last = text.find_last_not_of(" \t\r\n");
        return text.substr(first, (last - first + 1));
    }
    return "";
}

// Display DSA Complexity & Rationale Table for features
void displayDSAExplanationMenu() {
    std::cout << "\n====================================================================================================\n";
    std::cout << "                                  DSA EXPLANATION & ANALYSIS MATRIX                                 \n";
    std::cout << "====================================================================================================\n";
    std::cout << std::left 
              << std::setw(20) << "Feature / Operation"
              << std::setw(24) << "Data Structure Used"
              << std::setw(15) << "Time Complexity"
              << std::setw(15) << "Space Complexity"
              << "Rationale & Justification" << std::endl;
    std::cout << "----------------------------------------------------------------------------------------------------\n";
    
    std::cout << std::left 
              << std::setw(20) << "Playlist Storage"
              << std::setw(24) << "Doubly Linked List"
              << std::setw(15) << "O(1) Ins/Del"
              << std::setw(15) << "O(N)"
              << "Bidirectional prev/next node traversal for Play Next / Prev.\n";

    std::cout << std::left 
              << std::setw(20) << "Fast Search by ID"
              << std::setw(24) << "std::unordered_map"
              << std::setw(15) << "O(1) Avg"
              << std::setw(15) << "O(N)"
              << "Instant hash lookup mapping Song ID -> DoublyLinkedList Node*.\n";

    std::cout << std::left 
              << std::setw(20) << "Undo Operations"
              << std::setw(24) << "Custom Node Stack"
              << std::setw(15) << "O(1) Push/Pop"
              << std::setw(15) << "O(K)"
              << "LIFO behavior ideal for reverting recent delete/update actions.\n";

    std::cout << std::left 
              << std::setw(20) << "Recently Played"
              << std::setw(24) << "Custom Node Queue"
              << std::setw(15) << "O(1) Enq/Deq"
              << std::setw(15) << "O(Capacity)"
              << "FIFO buffer maintains chronological history of played tracks.\n";

    std::cout << std::left 
              << std::setw(20) << "Sorting & Ranges"
              << std::setw(24) << "Binary Search Tree"
              << std::setw(15) << "O(N log N)"
              << std::setw(15) << "O(N)"
              << "In-order traversal yields dynamically sorted order (Title/Artist/etc).\n";

    std::cout << std::left 
              << std::setw(20) << "Playlist Shuffle"
              << std::setw(24) << "Fisher-Yates Algo"
              << std::setw(15) << "O(N)"
              << std::setw(15) << "O(N)"
              << "Unbiased random permutation by re-linking DLL node pointers.\n";

    std::cout << std::left 
              << std::setw(20) << "Reverse Playlist"
              << std::setw(24) << "In-place DLL Reverse"
              << std::setw(15) << "O(N)"
              << std::setw(15) << "O(1)"
              << "Swaps prev and next pointers for every node without extra allocation.\n";

    std::cout << "----------------------------------------------------------------------------------------------------\n\n";
}

int main() {
    Playlist playlist;

    // Load initial playlist automatically from songs.txt if present
    playlist.loadPlaylistFromFile("songs.txt");

    bool running = true;
    while (running) {
        playlist.displayPlaybackStatus();

        std::cout << "==============================\n";
        std::cout << "    MUSIC PLAYLIST MANAGER    \n";
        std::cout << "==============================\n";
        std::cout << " 1  Add Song (Text Title/Artist & MP3 Path)\n";
        std::cout << " 2  Delete Song\n";
        std::cout << " 3  Update Song\n";
        std::cout << " 4  Display Playlist\n";
        std::cout << " 5  Search Song\n";
        std::cout << " 6  Play Song / Control Playback\n";
        std::cout << " 7  Next Song\n";
        std::cout << " 8  Previous Song\n";
        std::cout << " 9  Shuffle Playlist\n";
        std::cout << " 10 Sort Playlist\n";
        std::cout << " 11 Favorites Management\n";
        std::cout << " 12 Recently Played Queue\n";
        std::cout << " 13 Analytics & Reports\n";
        std::cout << " 14 Save Playlist to File\n";
        std::cout << " 15 Load Playlist from File\n";
        std::cout << " 16 Undo Last Action\n";
        std::cout << " 17 Move Song Position / Reverse\n";
        std::cout << " 18 DSA Complexity & Design Explanation\n";
        std::cout << " 19 Exit\n";
        std::cout << "==============================\n";

        int choice = getIntInput("Enter choice (1-19): ", 1, 19);

        switch (choice) {
            case 1: { // Add Song with File Manager or Manual Path
                std::cout << "\n--- ADD NEW SONG ---\n";
                int idChoice = getIntInput("Enter Song ID (Enter 0 for auto-generated ID): ", 0, 999999);
                int songId = (idChoice == 0) ? playlist.generateNextId() : idChoice;
                
                std::cout << "\nChoose Audio File Source:\n";
                std::cout << " 1  Browse & Choose from File Manager (Windows Explorer)\n";
                std::cout << " 2  Enter File Path Manually\n";
                int fileChoice = getIntInput("Select choice (1 or 2): ", 1, 2);

                std::string audioPath = "";
                std::string detectedTitle = "";

                if (fileChoice == 1) {
                    audioPath = chooseAudioFileWithDialog();
                    if (audioPath.empty()) {
                        std::cout << "[!] No file selected from File Manager. Please enter path manually:\n";
                        audioPath = getMP3FilePath("Enter Audio File Path: ");
                    } else {
                        std::cout << "[✓] Selected File: " << audioPath << "\n";
                        // Extract filename without extension as default title
                        size_t lastSlash = audioPath.find_last_of("\\/");
                        std::string fname = (lastSlash == std::string::npos) ? audioPath : audioPath.substr(lastSlash + 1);
                        size_t lastDot = fname.find_last_of('.');
                        if (lastDot != std::string::npos) {
                            detectedTitle = fname.substr(0, lastDot);
                        } else {
                            detectedTitle = fname;
                        }
                    }
                } else {
                    audioPath = getMP3FilePath("Enter Audio File Path (e.g. song.mp3): ");
                }

                std::string titlePrompt = "Enter Song Title";
                if (!detectedTitle.empty()) {
                    titlePrompt += " [Press Enter for \"" + detectedTitle + "\"]";
                }
                titlePrompt += ": ";

                std::string title = getTextOnlyString(titlePrompt, detectedTitle);
                std::string artist = getTextOnlyString("Enter Artist Name                                : ");
                std::string album = getStringInput("Enter Album Name (Optional)                      : ");
                if (album.empty()) album = "Single";
                std::string genre = getStringInput("Enter Genre (Optional)                           : ");
                if (genre.empty()) genre = "Pop";

                int duration = getIntInput("Enter Duration in seconds (> 0)                  : ", 1, 86400);
                int year = getIntInput("Enter Release Year (1800 - 2030)                 : ", 1800, 2030);
                double rating = getDoubleInput("Enter Rating (1.0 to 5.0)                       : ", 1.0, 5.0);
                int favChoice = getIntInput("Mark as Favorite? (1=Yes, 0=No)                  : ", 0, 1);

                Song newSong(songId, title, artist, album, genre, duration, year, rating, (favChoice == 1), audioPath);
                playlist.addSong(newSong);
                std::cout << "[✓] Song \"" << title << "\" successfully added to playlist!\n";
                break;
            }

            case 2: { // Delete Song
                std::cout << "\n--- DELETE SONG ---\n";
                int id = getIntInput("Enter Song ID to delete: ", 1, 999999);
                playlist.deleteSongById(id);
                break;
            }

            case 3: { // Update Song with Strict Text & MP3 Bounds Checks
                std::cout << "\n--- UPDATE SONG ---\n";
                int id = getIntInput("Enter Song ID to update: ", 1, 999999);
                Song* existing = playlist.searchById(id);
                if (existing == nullptr) {
                    std::cout << "[!] Song ID " << id << " not found.\n";
                    break;
                }

                std::cout << "Current Details:\n";
                existing->displayCard();

                std::string title = getStringInput("Enter New Title (Leave empty to keep current): ");
                if (!title.empty()) {
                    if (isValidTextOnly(title)) {
                        existing->setTitle(title);
                    } else {
                        std::cout << "[!] Invalid text format for title! Keeping previous title.\n";
                    }
                }

                std::string artist = getStringInput("Enter New Artist (Leave empty to keep current): ");
                if (!artist.empty()) {
                    if (isValidTextOnly(artist)) {
                        existing->setArtist(artist);
                    } else {
                        std::cout << "[!] Invalid text format for artist! Keeping previous artist.\n";
                    }
                }

                std::string album = getStringInput("Enter New Album (Leave empty to keep current): ");
                if (!album.empty()) existing->setAlbum(album);

                std::string genre = getStringInput("Enter New Genre (Leave empty to keep current): ");
                if (!genre.empty()) existing->setGenre(genre);

                std::cout << "\nUpdate Audio File Path:\n";
                std::cout << " 1  Browse & Choose from File Manager (Windows Explorer)\n";
                std::cout << " 2  Enter File Path Manually\n";
                std::cout << " 3  Keep current path (" << existing->getAudioPath() << ")\n";
                int updateAudioChoice = getIntInput("Select choice (1-3): ", 1, 3);
                if (updateAudioChoice == 1) {
                    std::string newPath = chooseAudioFileWithDialog();
                    if (!newPath.empty()) {
                        existing->setAudioPath(newPath);
                        std::cout << "[✓] Audio path updated to: " << newPath << "\n";
                    }
                } else if (updateAudioChoice == 2) {
                    std::string newPath = getMP3FilePath("Enter New Audio File Path: ");
                    if (!newPath.empty()) existing->setAudioPath(newPath);
                }

                std::string durStr = getStringInput("Enter New Duration in sec (> 0, Leave empty to keep current): ");
                if (!durStr.empty()) {
                    try {
                        int parsedDur = std::stoi(durStr);
                        if (parsedDur > 0 && parsedDur <= 86400) existing->setDurationSeconds(parsedDur);
                        else std::cout << "[!] Invalid duration. Keeping previous value.\n";
                    } catch (...) {}
                }

                std::string yrStr = getStringInput("Enter New Year (1800-2030, Leave empty to keep current): ");
                if (!yrStr.empty()) {
                    try {
                        int parsedYr = std::stoi(yrStr);
                        if (parsedYr >= 1800 && parsedYr <= 2030) existing->setYear(parsedYr);
                        else std::cout << "[!] Invalid year. Keeping previous value.\n";
                    } catch (...) {}
                }

                std::string ratStr = getStringInput("Enter New Rating (1.0-5.0, Leave empty to keep current): ");
                if (!ratStr.empty()) {
                    try {
                        double parsedRat = std::stod(ratStr);
                        if (parsedRat >= 1.0 && parsedRat <= 5.0) existing->setRating(parsedRat);
                        else std::cout << "[!] Invalid rating. Keeping previous value.\n";
                    } catch (...) {}
                }

                int favChoice = getIntInput("Mark as Favorite? (1=Yes, 0=No, -1=Keep current): ", -1, 1);
                if (favChoice != -1) existing->setFavorite(favChoice == 1);
                break;
            }

            case 4: { // Display Playlist
                std::cout << "\n--- DISPLAY PLAYLIST ---\n";
                std::cout << " 1 Display Forward\n";
                std::cout << " 2 Display Reverse\n";
                int viewChoice = getIntInput("Select view mode (1 or 2): ", 1, 2);
                if (viewChoice == 2) {
                    playlist.displayPlaylistReverse();
                } else {
                    playlist.displayPlaylistForward();
                }
                break;
            }

            case 5: { // Search Song
                std::cout << "\n--- SEARCH SONG ---\n";
                std::cout << " 1 Search by ID (O(1) Hash Map)\n";
                std::cout << " 2 Search by Title (BST / String match)\n";
                std::cout << " 3 Search by Artist (BST / String match)\n";
                std::cout << " 4 Search by Album\n";
                std::cout << " 5 Search by Genre\n";
                int searchMode = getIntInput("Select search type (1-5): ", 1, 5);

                if (searchMode == 1) {
                    int id = getIntInput("Enter Song ID: ", 1, 999999);
                    Song* song = playlist.searchById(id);
                    if (song) {
                        std::cout << "\n[✓] Song Found (O(1) Hash Map Lookup):\n";
                        song->displayCard();
                    } else {
                        std::cout << "[!] Song ID " << id << " not found.\n";
                    }
                } else if (searchMode == 2) {
                    std::string title = getTextOnlyString("Enter Song Title query: ");
                    auto results = playlist.searchByTitle(title);
                    std::cout << "\nFound " << results.size() << " matching song(s):\n";
                    for (const auto& s : results) s.displayFormattedRow();
                } else if (searchMode == 3) {
                    std::string artist = getTextOnlyString("Enter Artist query: ");
                    auto results = playlist.searchByArtist(artist);
                    std::cout << "\nFound " << results.size() << " matching song(s):\n";
                    for (const auto& s : results) s.displayFormattedRow();
                } else if (searchMode == 4) {
                    std::string album = getStringInput("Enter Album query: ");
                    auto results = playlist.searchByAlbum(album);
                    std::cout << "\nFound " << results.size() << " matching song(s):\n";
                    for (const auto& s : results) s.displayFormattedRow();
                } else if (searchMode == 5) {
                    std::string genre = getStringInput("Enter Genre query: ");
                    auto results = playlist.searchByGenre(genre);
                    std::cout << "\nFound " << results.size() << " matching song(s):\n";
                    for (const auto& s : results) s.displayFormattedRow();
                }
                break;
            }

            case 6: { // Play Song / Control Playback
                std::cout << "\n--- START PLAYBACK ---\n";
                std::cout << " 1 Play Song by ID\n";
                std::cout << " 2 Play / Resume Current Track\n";
                int pbChoice = getIntInput("Select option (1-2): ", 1, 2);
                if (pbChoice == 1) {
                    int id = getIntInput("Enter Song ID to play: ", 1, 999999);
                    if (playlist.playSongById(id)) {
                        runPlaybackController(playlist);
                    }
                } else {
                    if (playlist.playCurrent()) {
                        runPlaybackController(playlist);
                    }
                }
                break;
            }

            case 7: { // Next Song
                if (playlist.playNext()) {
                    runPlaybackController(playlist);
                }
                break;
            }

            case 8: { // Previous Song
                if (playlist.playPrevious()) {
                    runPlaybackController(playlist);
                }
                break;
            }

            case 9: { // Shuffle Playlist
                playlist.shufflePlaylist();
                playlist.displayPlaylistForward();
                break;
            }

            case 10: { // Sort Playlist
                std::cout << "\n--- SORT PLAYLIST (BST In-Order Traversal) ---\n";
                std::cout << " 1 Sort by Title\n";
                std::cout << " 2 Sort by Artist\n";
                std::cout << " 3 Sort by Rating (Highest First)\n";
                std::cout << " 4 Sort by Duration\n";
                std::cout << " 5 Sort by Release Year\n";
                int sortCrit = getIntInput("Select sorting criteria (1-5): ", 1, 5);
                
                SortCriteria crit = SortCriteria::TITLE;
                if (sortCrit == 2) crit = SortCriteria::ARTIST;
                else if (sortCrit == 3) crit = SortCriteria::RATING;
                else if (sortCrit == 4) crit = SortCriteria::DURATION;
                else if (sortCrit == 5) crit = SortCriteria::YEAR;

                playlist.sortPlaylist(crit);
                break;
            }

            case 11: { // Favorites
                std::cout << "\n--- FAVORITES MANAGEMENT ---\n";
                std::cout << " 1 Display Favorites\n";
                std::cout << " 2 Mark Song as Favorite\n";
                std::cout << " 3 Remove Song from Favorites\n";
                int favOption = getIntInput("Select option (1-3): ", 1, 3);

                if (favOption == 1) {
                    playlist.displayFavorites();
                } else if (favOption == 2) {
                    int id = getIntInput("Enter Song ID to mark favorite: ", 1, 999999);
                    playlist.markFavorite(id, true);
                } else if (favOption == 3) {
                    int id = getIntInput("Enter Song ID to remove from favorites: ", 1, 999999);
                    playlist.markFavorite(id, false);
                }
                break;
            }

            case 12: { // Recently Played Queue
                playlist.displayRecentlyPlayed();
                break;
            }

            case 13: { // Reports
                playlist.generateAnalyticsReport();
                break;
            }

            case 14: { // Save Playlist
                std::string filename = getStringInput("Enter filename to save (default 'songs.txt'): ");
                if (filename.empty()) filename = "songs.txt";
                if (playlist.savePlaylistToFile(filename)) {
                    std::cout << "[✓] Playlist successfully saved to '" << filename << "'.\n";
                }
                break;
            }

            case 15: { // Load Playlist
                std::string filename = getStringInput("Enter filename to load (default 'songs.txt'): ");
                if (filename.empty()) filename = "songs.txt";
                playlist.loadPlaylistFromFile(filename);
                break;
            }

            case 16: { // Undo
                playlist.undoLastAction();
                break;
            }

            case 17: { // Move / Reverse Position
                std::cout << "\n--- REORDER PLAYLIST ---\n";
                std::cout << " 1 Move Song Position\n";
                std::cout << " 2 Reverse Playlist In-Place\n";
                int reorderChoice = getIntInput("Select choice (1 or 2): ", 1, 2);
                if (reorderChoice == 1) {
                    int oldPos = getIntInput("Enter current 1-based position: ", 1, playlist.getSongCount());
                    int newPos = getIntInput("Enter target 1-based position : ", 1, playlist.getSongCount());
                    playlist.moveSongPosition(oldPos, newPos);
                } else if (reorderChoice == 2) {
                    playlist.reversePlaylist();
                    playlist.displayPlaylistForward();
                }
                break;
            }

            case 18: { // DSA Explanation
                displayDSAExplanationMenu();
                break;
            }

            case 19: { // Exit
                std::cout << "\n[!] Saving playlist state to 'songs.txt' before exiting...\n";
                playlist.savePlaylistToFile("songs.txt");
                std::cout << "Thank you for using Music Playlist Manager! Goodbye!\n";
                running = false;
                break;
            }

            default:
                std::cout << "[!] Invalid option selection. Please enter a choice between 1 and 19.\n";
                break;
        }

        if (running && choice != 18) {
            std::cout << "\nPress Enter to return to main menu...";
            std::cin.get();
        }
    }

    return 0;
}

