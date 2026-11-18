@echo off
cd /d "%~dp0"
title Music Playlist Manager
if exist "C:\msys64\ucrt64\bin" (
    set "PATH=C:\msys64\ucrt64\bin;%PATH%"
)
PlaylistManager.exe
