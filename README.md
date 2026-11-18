# 🎵 Music Playlist Manager (C++ OOP & Data Structures Project)

An interview-level, modular C++ application designed for Placement Enhancement Programs (PEP), college evaluation, and technical interviews. This application manages music playlists using custom-built Data Structures implemented from scratch without relying on STL container abstractions like `std::list` or `std::queue`.

---

## 📌 Project Overview

The **Music Playlist Manager** provides an interactive command-line interface (CLI) to store, navigate, manipulate, search, sort, and track music songs. It brings together fundamental Data Structures and Object-Oriented Programming (OOP) concepts:

1. **Doubly Linked List**: Primary sequence container supporting $O(1)$ node insertion/deletion, bidirectional navigation (`next`/`prev`), in-place reversal, and position reordering.
2. **Custom Stack**: Node-based LIFO data structure managing **Undo** history (reverting deletions and updates).
3. **Custom Queue**: Node-based FIFO circular/buffered queue tracking **Recently Played** songs.
4. **Hash Map (`std::unordered_map`)**: Direct key-value lookup mapping `Song ID -> DoublyLinkedList Node*` for instant $O(1)$ song lookups.
5. **Binary Search Tree (BST)**: Custom BST providing dynamic sorted views of the playlist by Title, Artist, Rating, Duration, or Year via in-order traversal.

---

## 🛠️ Data Structures & Complexity Analysis

| Data Structure / Feature | Implementation Details | Time Complexity | Space Complexity | Why Chosen? |
| :--- | :--- | :--- | :--- | :--- |
| **Playlist Storage** | Custom Doubly Linked List | **Add**: $O(1)$<br>**Delete**: $O(1)$ (with node ptr)<br>**Nav**: $O(1)$ | $O(N)$ | Allows seamless Next/Previous song navigation without reallocation overhead. |
| **Fast ID Search** | `std::unordered_map<int, Node*>` | **Search**: $O(1)$ Avg<br>**Insert/Erase**: $O(1)$ Avg | $O(N)$ | Eliminates linear search when accessing songs directly by unique ID. |
| **Undo Operations** | Custom Node-based Stack | **Push**: $O(1)$<br>**Pop**: $O(1)$ | $O(K)$ ($K$ undo steps) | Last-In-First-Out (LIFO) order perfectly matches revert operations. |
| **Recently Played History** | Custom Node-based Queue | **Enqueue**: $O(1)$<br>**Dequeue**: $O(1)$ | $O(C)$ ($C = 10$ capacity) | First-In-First-Out (FIFO) buffer maintains strict chronological playback history. |
| **Sorted Playlist View** | Custom Binary Search Tree | **Insert**: $O(\log N)$<br>**In-Order Display**: $O(N)$ | $O(N)$ | Hierarchical sorting without modifying the underlying sequence of the user's playlist. |
| **Shuffle Playlist** | Fisher-Yates Algorithm | **Shuffle**: $O(N)$ | $O(N)$ (Node pointers) | Produces an unbiased uniform random permutation of playlist nodes. |
| **Reverse Playlist** | In-place Pointer Swap | **Reverse**: $O(N)$ | $O(1)$ | Swaps `next` and `prev` node pointers directly without allocating new memory. |

---

## 📁 Folder Structure

```
MusicPlaylistManager/
├── main.cpp              # Interactive menu UI & CLI loop
├── Song.h                # Song data model declaration
├── Song.cpp              # Song entity implementation & serialization
├── DoublyLinkedList.h    # Custom Doubly Linked List header
├── DoublyLinkedList.cpp  # Doubly Linked List operations
├── Stack.h               # Custom Node-based Stack header for Undo
├── Stack.cpp             # Stack implementation
├── Queue.h               # Custom Node-based Queue header for History
├── Queue.cpp             # Queue implementation
├── BST.h                 # Custom Binary Search Tree header for Sorting
├── BST.cpp               # BST implementation & In-order traversal
├── FileManager.h         # Persistence manager header
├── FileManager.cpp       # Save/load file handlers
├── songs.txt             # Formatted sample playlist data file
└── README.md             # Documentation
```

---

## 🚀 How to Compile and Run

### Prerequisites
- GCC/G++ Compiler with C++17 support (`g++ --version`)
- Visual Studio Code or any terminal environment (Windows PowerShell / Command Prompt / Linux / macOS)

### 1. Compile the Project
Open terminal in the project directory `MusicPlaylistManager` and execute:

```bash
g++ -std=c++17 -Wall *.cpp -o PlaylistManager
```

### 2. Run the Application

#### On Windows:
```cmd
.\PlaylistManager.exe
```

#### On Linux / macOS:
```bash
./PlaylistManager
```

---

## 💻 Sample Terminal Interface

```
==============================
    MUSIC PLAYLIST MANAGER
==============================
 1  Add Song
 2  Delete Song
 3  Update Song
 4  Display Playlist
 5  Search Song
 6  Play Song / Control Playback
 7  Next Song
 8  Previous Song
 9  Shuffle Playlist
 10 Sort Playlist
 11 Favorites Management
 12 Recently Played Queue
 13 Analytics & Reports
 14 Save Playlist to File
 15 Load Playlist from File
 16 Undo Last Action
 17 Move Song Position / Reverse
 18 DSA Complexity & Design Explanation
 19 Exit
==============================
```

---

## 🔮 Future Improvements

1. **Self-Balancing Tree (AVL / Red-Black Tree)**: Upgrade standard BST to AVL Tree to guarantee $O(\log N)$ search/insertion performance under skewed inputs.
2. **Audio File Streaming Integrations**: Integrate SFML or FMOD library for real-time MP3 playback.
3. **Multi-Playlist Support**: Extend `Playlist` manager to handle multiple user-created playlists (e.g., Workout, Chill, Party).
4. **Graphical User Interface (GUI)**: Create a modern Qt or web-based UI layer connecting to this C++ core engine.
