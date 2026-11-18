# 🌐 Music Playlist Manager (Web Application)

This web application complements your C++ Terminal DSA engine with a modern interactive browser experience. It provides visual data structure inspectors (Doubly Linked List, Stack, Queue, BST, Hash Map) and an integrated Web Audio music player.

---

## 🚀 How to Run the Web App

### Method 1: Double-Click directly in File Explorer
1. Open the folder `C:\Users\abhi2\.gemini\antigravity\scratch\MusicPlaylistManager` in File Explorer.
2. Double-click **`index.html`** to open it in Chrome, Edge, Brave, or Firefox.

---

### Method 2: Open using VS Code Live Server Extension
1. In VS Code, open **`index.html`**.
2. Right-click inside `index.html` and select **"Open with Live Server"** (or press `Alt + L, Alt + O`).

---

## 💡 Web Application Features

1. **Master Playlist Engine**: Add, edit, delete, mark favorites, and search by ID, Title, Artist, or Genre with instant $O(1)$ Hash Map filtering.
2. **Doubly Linked List Visual Inspector**: Displays live node chains with bidirectional `head`, `prev`, `next`, and `tail` pointers.
3. **Undo Stack Inspector**: Visualizes LIFO stack frames whenever songs are deleted or updated, allowing 1-click Undo restoration.
4. **History Queue Inspector**: Shows FIFO buffer slots storing recently played songs.
5. **Binary Search Tree Visualizer**: Renders sorted tree nodes dynamically based on Title, Artist, Rating, Duration, or Year.
6. **Hash Map Inspector**: Displays direct key-value memory mapping from `Song ID ➔ Node*`.
7. **Interactive Audio Player**: Features Play/Pause, Next track (via DLL `next`), Previous track (via DLL `prev`), Shuffle (Fisher-Yates), In-place Reverse, progress bar, and synthesizer tone playback.
