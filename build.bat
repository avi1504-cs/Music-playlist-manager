@echo off
cd /d "%~dp0"
echo Compiling Music Playlist Manager...
if exist "C:\msys64\ucrt64\bin" (
    set "PATH=C:\msys64\ucrt64\bin;%PATH%"
)
g++ -std=c++17 -Wall BST.cpp DoublyLinkedList.cpp FileManager.cpp Playlist.cpp Queue.cpp Song.cpp Stack.cpp main.cpp -o PlaylistManager.exe -lwinmm -lcomdlg32 -static
if %ERRORLEVEL% EQU 0 (
    echo [✓] Build successful: PlaylistManager.exe
) else (
    echo [X] Build failed!
)
