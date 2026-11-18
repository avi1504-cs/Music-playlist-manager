// Custom Data Structures in JavaScript mirroring C++ logic

class Song {
    constructor(id, title, artist, album, genre, durationSeconds, year, rating, favorite = false, cover = 'synthwave_cover.png', audioUrl = '') {
        this.id = id;
        this.title = title;
        this.artist = artist;
        this.album = album;
        this.genre = genre;
        this.durationSeconds = parseInt(durationSeconds);
        this.year = parseInt(year);
        this.rating = parseFloat(rating);
        this.favorite = favorite;
        this.cover = cover;
        this.audioUrl = audioUrl;
    }

    getFormattedDuration() {
        const min = Math.floor(this.durationSeconds / 60);
        const sec = this.durationSeconds % 60;
        return `${String(min).padStart(2, '0')}:${String(sec).padStart(2, '0')}`;
    }
}

// Doubly Linked List Node
class Node {
    constructor(song) {
        this.data = song;
        this.prev = null;
        this.next = null;
    }
}

// Custom Doubly Linked List
class DoublyLinkedList {
    constructor() {
        this.head = null;
        this.tail = null;
        this.sizeCount = 0;
    }

    isEmpty() { return this.sizeCount === 0; }
    getSize() { return this.sizeCount; }

    append(song) {
        const newNode = new Node(song);
        if (this.isEmpty()) {
            this.head = this.tail = newNode;
        } else {
            this.tail.next = newNode;
            newNode.prev = this.tail;
            this.tail = newNode;
        }
        this.sizeCount++;
        return newNode;
    }

    prepend(song) {
        const newNode = new Node(song);
        if (this.isEmpty()) {
            this.head = this.tail = newNode;
        } else {
            newNode.next = this.head;
            this.head.prev = newNode;
            this.head = newNode;
        }
        this.sizeCount++;
        return newNode;
    }

    insertAt(position, song) {
        if (position < 1 || position > this.sizeCount + 1) return null;
        if (position === 1) return this.prepend(song);
        if (position === this.sizeCount + 1) return this.append(song);

        let current = this.head;
        for (let i = 1; i < position - 1; i++) {
            current = current.next;
        }

        const newNode = new Node(song);
        newNode.next = current.next;
        newNode.prev = current;
        if (current.next) current.next.prev = newNode;
        current.next = newNode;
        this.sizeCount++;
        return newNode;
    }

    removeById(id) {
        let curr = this.head;
        while (curr) {
            if (curr.data.id === id) {
                const deletedSong = curr.data;
                if (curr === this.head && curr === this.tail) {
                    this.head = this.tail = null;
                } else if (curr === this.head) {
                    this.head = this.head.next;
                    this.head.prev = null;
                } else if (curr === this.tail) {
                    this.tail = this.tail.prev;
                    this.tail.next = null;
                } else {
                    curr.prev.next = curr.next;
                    curr.next.prev = curr.prev;
                }
                this.sizeCount--;
                return deletedSong;
            }
            curr = curr.next;
        }
        return null;
    }

    findNodeById(id) {
        let curr = this.head;
        while (curr) {
            if (curr.data.id === id) return curr;
            curr = curr.next;
        }
        return null;
    }

    reverse() {
        if (this.isEmpty() || this.head === this.tail) return;
        let curr = this.head;
        let temp = null;
        while (curr) {
            temp = curr.prev;
            curr.prev = curr.next;
            curr.next = temp;
            curr = curr.prev;
        }
        if (temp) {
            this.tail = this.head;
            this.head = temp.prev;
        }
    }

    shuffle() {
        if (this.sizeCount <= 1) return;
        const nodes = [];
        let curr = this.head;
        while (curr) {
            nodes.push(curr);
            curr = curr.next;
        }

        // Fisher-Yates Shuffle
        for (let i = nodes.length - 1; i > 0; i--) {
            const j = Math.floor(Math.random() * (i + 1));
            [nodes[i], nodes[j]] = [nodes[j], nodes[i]];
        }

        this.head = nodes[0];
        this.head.prev = null;
        curr = this.head;
        for (let i = 1; i < nodes.length; i++) {
            curr.next = nodes[i];
            nodes[i].prev = curr;
            curr = nodes[i];
        }
        curr.next = null;
        this.tail = curr;
    }
}

// Custom Stack for Undo
class Stack {
    constructor() {
        this.items = [];
    }
    push(item) { this.items.push(item); }
    pop() { return this.items.pop(); }
    peek() { return this.items[this.items.length - 1]; }
    isEmpty() { return this.items.length === 0; }
    size() { return this.items.length; }
}

// Custom Queue for Recently Played History
class Queue {
    constructor(capacity = 10) {
        this.items = [];
        this.capacity = capacity;
    }
    enqueue(song) {
        if (this.items.length >= this.capacity) {
            this.items.shift(); // Dequeue oldest
        }
        this.items.push(song);
    }
    dequeue() { return this.items.shift(); }
    isEmpty() { return this.items.length === 0; }
    size() { return this.items.length; }
}

// Custom Binary Search Tree
class BSTNode {
    constructor(song) {
        this.song = song;
        this.left = null;
        this.right = null;
    }
}

class BST {
    constructor(criteria = 'TITLE') {
        this.root = null;
        this.criteria = criteria;
    }

    isLessThan(a, b) {
        switch (this.criteria) {
            case 'TITLE': return a.title.toLowerCase() < b.title.toLowerCase();
            case 'ARTIST': return a.artist.toLowerCase() < b.artist.toLowerCase();
            case 'RATING': return a.rating > b.rating; // Descending for rating
            case 'DURATION': return a.durationSeconds < b.durationSeconds;
            case 'YEAR': return a.year < b.year;
            default: return a.id < b.id;
        }
    }

    insert(song) {
        const newNode = new BSTNode(song);
        if (!this.root) {
            this.root = newNode;
            return;
        }
        let curr = this.root;
        while (true) {
            if (this.isLessThan(song, curr.song)) {
                if (!curr.left) { curr.left = newNode; break; }
                curr = curr.left;
            } else {
                if (!curr.right) { curr.right = newNode; break; }
                curr = curr.right;
            }
        }
    }

    inorder() {
        const result = [];
        function traverse(node) {
            if (!node) return;
            traverse(node.left);
            result.push(node.song);
            traverse(node.right);
        }
        traverse(this.root);
        return result;
    }
}

// Web Application State & Controller
class PlaylistApp {
    constructor() {
        this.dll = new DoublyLinkedList();
        this.idMap = new Map(); // O(1) Hash Map ID -> Node
        this.undoStack = new Stack();
        this.recentlyPlayedQueue = new Queue(10);
        this.currentlyPlayingNode = null;
        this.isPlaying = false;
        
        // HTML5 Audio Engine — do NOT set crossOrigin globally; it breaks CDN
        // URLs that don't send CORS headers and blocks local blob: URLs.
        this.audioElement = new Audio();
        this.audioElement.volume = 0.8;
        
        // Web Audio Synthesizer Context (fallback when real audio fails)
        this.audioCtx = null;
        this.synthInterval = null;
        this.isSeeking = false;

        this.initAudioEngine();
        this.initInitialData();
        this.initDOM();
        this.renderAll();
    }

    initAudioEngine() {
        // Track time updates for seek bar and time labels
        this.audioElement.addEventListener('timeupdate', () => {
            if (!this.audioElement.duration) return;
            const current = Math.floor(this.audioElement.currentTime);
            const total = Math.floor(this.audioElement.duration);
            const percent = (current / total) * 100;

            const seekBar = document.getElementById('player-seek-bar');
            if (seekBar && !this.isSeeking) {
                seekBar.value = percent;
            }
            
            const currLabel = document.getElementById('player-current-time');
            if (currLabel) currLabel.textContent = this.formatTime(current);
            
            const totalLabel = document.getElementById('player-total-time');
            if (totalLabel) totalLabel.textContent = this.formatTime(total);
        });

        // Automatically play next song when current track ends (using DLL next pointer)
        this.audioElement.addEventListener('ended', () => {
            this.playNext();
        });
    }

    formatTime(sec) {
        if (isNaN(sec)) return "00:00";
        const m = Math.floor(sec / 60);
        const s = Math.floor(sec % 60);
        return `${String(m).padStart(2, '0')}:${String(s).padStart(2, '0')}`;
    }

    initInitialData() {
        const defaultCover = "data:image/svg+xml;utf8,<svg xmlns='http://www.w3.org/2000/svg' width='100' height='100' viewBox='0 0 100 100'><rect width='100' height='100' fill='%231e293b'/><circle cx='50' cy='50' r='35' fill='%230f172a' stroke='%238b5cf6' stroke-width='4'/><circle cx='50' cy='50' r='12' fill='%23ec4899'/></svg>";
        const initialSongs = [
            new Song(101, "Blinding Lights", "The Weeknd", "After Hours", "Synthwave", 200, 2019, 4.9, true, defaultCover, "https://actions.google.com/sounds/v1/ambiences/rain_heavy.ogg"),
            new Song(102, "Shape of You", "Ed Sheeran", "Divide", "Pop", 233, 2017, 4.7, false, defaultCover, "https://actions.google.com/sounds/v1/alarms/digital_watch_alarm.ogg"),
            new Song(103, "Bohemian Rhapsody", "Queen", "A Night at the Opera", "Rock", 354, 1975, 5.0, true, defaultCover, "https://actions.google.com/sounds/v1/science_fiction/laser_beam.ogg"),
            new Song(104, "Hotel California", "Eagles", "Hotel California", "Classic Rock", 390, 1976, 4.9, true, defaultCover, "https://actions.google.com/sounds/v1/cartoon/boing_spring.ogg"),
            new Song(105, "Stay", "Kid LAROI & Justin Bieber", "F*CK LOVE 3", "Pop", 141, 2021, 4.3, false, defaultCover, "https://actions.google.com/sounds/v1/ambiences/rain_heavy.ogg"),
            new Song(106, "Levitating", "Dua Lipa", "Future Nostalgia", "Dance Pop", 203, 2020, 4.6, false, defaultCover, "https://actions.google.com/sounds/v1/alarms/digital_watch_alarm.ogg"),
            new Song(107, "Sunflower", "Post Malone & Swae Lee", "Spider-Man", "Hip-Hop", 158, 2018, 4.8, true, defaultCover, "https://actions.google.com/sounds/v1/science_fiction/laser_beam.ogg"),
            new Song(108, "Someone Like You", "Adele", "21", "Soul", 285, 2011, 4.7, false, defaultCover, "https://actions.google.com/sounds/v1/ambiences/rain_heavy.ogg")
        ];

        initialSongs.forEach(song => this.addSongToEngine(song, false));
    }

    rebuildIdMap() {
        this.idMap.clear();
        let curr = this.dll.head;
        while (curr) {
            this.idMap.set(curr.data.id, curr);
            curr = curr.next;
        }
    }

    addSongToEngine(song, updateUndo = true) {
        if (this.idMap.has(song.id)) return false;
        const node = this.dll.append(song);
        this.idMap.set(song.id, node);
        this.renderAll();
        return true;
    }

    deleteSongById(id) {
        const node = this.idMap.get(id);
        if (!node) return false;

        let pos = 1;
        let curr = this.dll.head;
        while (curr && curr !== node) {
            curr = curr.next;
            pos++;
        }

        this.undoStack.push({
            type: 'DELETE',
            song: node.data,
            position: pos
        });

        if (this.currentlyPlayingNode === node) {
            this.currentlyPlayingNode = node.next || node.prev;
        }

        this.dll.removeById(id);
        this.idMap.delete(id);
        this.renderAll();
        return true;
    }

    undoLastAction() {
        if (this.undoStack.isEmpty()) return false;
        const action = this.undoStack.pop();

        if (action.type === 'DELETE') {
            const newNode = this.dll.insertAt(action.position, action.song);
            this.idMap.set(action.song.id, newNode);
        }
        this.renderAll();
        return true;
    }

    playSong(node) {
        if (!node) return;
        this.currentlyPlayingNode = node;
        this.isPlaying = true;
        this.recentlyPlayedQueue.enqueue(node.data);

        this.stopProceduralSynthMelody();

        // Assign audio URL — prefer the song's own URL, fall back to SoundHelix demo
        const streamUrl = node.data.audioUrl || `https://www.soundhelix.com/examples/mp3/SoundHelix-Song-${((node.data.id - 101) % 8) + 1}.mp3`;
        
        // IMPORTANT: Never set crossOrigin on the audio element.
        // Setting crossOrigin='anonymous' causes CORS failures for any server that
        // does not send Access-Control-Allow-Origin headers (most CDNs / local files).
        // blob: URLs created via URL.createObjectURL are always same-origin — no CORS.
        this.audioElement.removeAttribute('crossOrigin');

        this.audioElement.src = streamUrl;
        this.audioElement.currentTime = 0;
        this.audioElement.load();

        const playPromise = this.audioElement.play();
        if (playPromise !== undefined) {
            playPromise.catch(err => {
                console.warn('Audio playback error, falling back to synth melody:', err.message);
                this.startProceduralSynthMelody();
            });
        }

        this.updatePlayerBar();
        this.renderAll();
    }

    playNext() {
        if (!this.currentlyPlayingNode) {
            this.playSong(this.dll.head);
            return;
        }
        const nextNode = this.currentlyPlayingNode.next || this.dll.head;
        this.playSong(nextNode);
    }

    playPrev() {
        if (!this.currentlyPlayingNode) {
            this.playSong(this.dll.head);
            return;
        }
        const prevNode = this.currentlyPlayingNode.prev || this.dll.tail;
        this.playSong(prevNode);
    }

    togglePlayPause() {
        if (!this.currentlyPlayingNode && this.dll.head) {
            this.playSong(this.dll.head);
            return;
        }

        if (this.isPlaying) {
            this.audioElement.pause();
            this.stopProceduralSynthMelody();
            this.isPlaying = false;
        } else {
            const playPromise = this.audioElement.play();
            if (playPromise !== undefined) {
                playPromise.catch(() => this.startProceduralSynthMelody());
            }
            this.isPlaying = true;
        }
        this.updatePlayerBar();
    }

    startProceduralSynthMelody() {
        this.stopProceduralSynthMelody();
        try {
            const AudioCtx = window.AudioContext || window.webkitAudioContext;
            if (!this.audioCtx) this.audioCtx = new AudioCtx();
            if (this.audioCtx.state === 'suspended') {
                this.audioCtx.resume();
            }

            const notes = [261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25];
            let step = 0;
            this.synthTimeSeconds = 0;

            this.synthInterval = setInterval(() => {
                if (!this.isPlaying) return;

                this.synthTimeSeconds++;
                const songDur = (this.currentlyPlayingNode && this.currentlyPlayingNode.data) ? this.currentlyPlayingNode.data.durationSeconds : 200;
                const percent = Math.min((this.synthTimeSeconds / songDur) * 100, 100);

                const seekBar = document.getElementById('player-seek-bar');
                if (seekBar && !this.isSeeking) seekBar.value = percent;

                const currLabel = document.getElementById('player-current-time');
                if (currLabel) currLabel.textContent = this.formatTime(this.synthTimeSeconds);

                const totalLabel = document.getElementById('player-total-time');
                if (totalLabel) totalLabel.textContent = this.formatTime(songDur);

                if (this.synthTimeSeconds >= songDur) {
                    this.playNext();
                    return;
                }

                const osc = this.audioCtx.createOscillator();
                const gain = this.audioCtx.createGain();
                
                osc.type = 'triangle';
                const baseFreq = notes[(step + (this.currentlyPlayingNode ? this.currentlyPlayingNode.data.id : 0)) % notes.length];
                osc.frequency.setValueAtTime(baseFreq, this.audioCtx.currentTime);

                gain.gain.setValueAtTime(0.15, this.audioCtx.currentTime);
                gain.gain.exponentialRampToValueAtTime(0.001, this.audioCtx.currentTime + 0.35);

                osc.connect(gain);
                gain.connect(this.audioCtx.destination);

                osc.start();
                osc.stop(this.audioCtx.currentTime + 0.35);
                step++;
            }, 1000);
        } catch (e) {
            console.log("Synth error:", e);
        }
    }

    stopProceduralSynthMelody() {
        if (this.synthInterval) {
            clearInterval(this.synthInterval);
            this.synthInterval = null;
        }
    }

    initDOM() {
        // Tab switching
        document.querySelectorAll('.nav-item').forEach(item => {
            item.addEventListener('click', (e) => {
                e.preventDefault();
                document.querySelectorAll('.nav-item').forEach(n => n.classList.remove('active'));
                document.querySelectorAll('.tab-page').forEach(p => p.classList.remove('active'));

                item.classList.add('active');
                const tabId = item.getAttribute('data-tab');
                document.getElementById(`tab-${tabId}`).classList.add('active');
            });
        });

        // Search input
        document.getElementById('search-input').addEventListener('input', (e) => {
            this.renderPlaylistTable(e.target.value);
        });

        // Sort select
        document.getElementById('sort-select').addEventListener('change', (e) => {
            this.renderPlaylistTable('', e.target.value);
        });

        // Player Bar Buttons
        document.getElementById('btn-player-toggle').addEventListener('click', () => this.togglePlayPause());
        document.getElementById('btn-player-next').addEventListener('click', () => this.playNext());
        document.getElementById('btn-player-prev').addEventListener('click', () => this.playPrev());
        document.getElementById('btn-player-shuffle').addEventListener('click', () => {
            this.dll.shuffle();
            this.rebuildIdMap();
            this.renderAll();
        });
        document.getElementById('btn-player-reverse').addEventListener('click', () => {
            this.dll.reverse();
            this.rebuildIdMap();
            this.renderAll();
        });

        // Seek Bar Scrubbing
        const seekBar = document.getElementById('player-seek-bar');
        if (seekBar) {
            seekBar.addEventListener('mousedown', () => { this.isSeeking = true; });
            seekBar.addEventListener('mouseup', () => { this.isSeeking = false; });
            seekBar.addEventListener('change', (e) => {
                if (this.audioElement.duration) {
                    this.audioElement.currentTime = (e.target.value / 100) * this.audioElement.duration;
                }
            });
        }

        // Volume Bar Adjustment
        const volumeBar = document.getElementById('volume-bar');
        if (volumeBar) {
            volumeBar.addEventListener('input', (e) => {
                const vol = e.target.value / 100;
                this.audioElement.volume = vol;
                const icon = document.getElementById('volume-icon');
                if (icon) {
                    icon.className = vol === 0 ? 'fa-solid fa-volume-xmark' : (vol < 0.5 ? 'fa-solid fa-volume-low' : 'fa-solid fa-volume-high');
                }
            });
        }

        // Quick Undo Button
        document.getElementById('btn-quick-undo').addEventListener('click', () => this.undoLastAction());
        document.getElementById('btn-shuffle').addEventListener('click', () => {
            this.dll.shuffle();
            this.rebuildIdMap();
            this.renderAll();
        });
        document.getElementById('btn-reverse').addEventListener('click', () => {
            this.dll.reverse();
            this.rebuildIdMap();
            this.renderAll();
        });

        // Direct Choose from File Manager Quick Import
        const quickImportBtn = document.getElementById('btn-quick-import');
        const directAudioInput = document.getElementById('direct-audio-input');
        if (quickImportBtn && directAudioInput) {
            quickImportBtn.addEventListener('click', () => {
                directAudioInput.click();
            });

            directAudioInput.addEventListener('change', (e) => {
                if (e.target.files && e.target.files[0]) {
                    const file = e.target.files[0];
                    const blobUrl = URL.createObjectURL(file);
                    const title = file.name.replace(/\.[^/.]+$/, "");
                    const defaultCover = "data:image/svg+xml;utf8,<svg xmlns='http://www.w3.org/2000/svg' width='100' height='100' viewBox='0 0 100 100'><rect width='100' height='100' fill='%231e293b'/><circle cx='50' cy='50' r='35' fill='%230f172a' stroke='%238b5cf6' stroke-width='4'/><circle cx='50' cy='50' r='12' fill='%2310b981'/></svg>";

                    // Generate a unique ID: find current max ID and increment
                    let maxId = 100;
                    this.idMap.forEach((node, id) => { if (id > maxId) maxId = id; });
                    const newId = maxId + 1;

                    const doAddAndPlay = (dur) => {
                        const newSong = new Song(newId, title, "Local Track", "Computer Audio", "Audio", dur, new Date().getFullYear(), 5.0, true, defaultCover, blobUrl);
                        this.addSongToEngine(newSong);
                        // Automatically play the newly imported file
                        const node = this.idMap.get(newId);
                        if (node) this.playSong(node);
                    };

                    const tempAudio = new Audio();
                    let metaLoaded = false;

                    tempAudio.onloadedmetadata = () => {
                        metaLoaded = true;
                        const dur = (tempAudio.duration && !isNaN(tempAudio.duration)) ? Math.round(tempAudio.duration) : 210;
                        doAddAndPlay(dur);
                    };
                    tempAudio.onerror = () => {
                        if (!metaLoaded) doAddAndPlay(210);
                    };
                    // Fallback: if metadata never fires within 2s, add with default duration
                    setTimeout(() => {
                        if (!metaLoaded) {
                            metaLoaded = true;
                            doAddAndPlay(210);
                        }
                    }, 2000);

                    tempAudio.src = blobUrl;
                    // Reset input so same file can be selected again
                    e.target.value = '';
                }
            });
        }

        // Add Song Modal File Input auto-detection
        const audioFileInput = document.getElementById('form-audio-file');
        if (audioFileInput) {
            audioFileInput.addEventListener('change', (e) => {
                if (e.target.files && e.target.files[0]) {
                    const file = e.target.files[0];
                    const titleInput = document.getElementById('form-title');
                    if (!titleInput.value.trim()) {
                        titleInput.value = file.name.replace(/\.[^/.]+$/, "");
                    }
                    const blobUrl = URL.createObjectURL(file);
                    const tempAudio = new Audio(blobUrl);
                    tempAudio.onloadedmetadata = () => {
                        if (tempAudio.duration && !isNaN(tempAudio.duration)) {
                            document.getElementById('form-duration').value = Math.round(tempAudio.duration);
                        }
                    };
                }
            });
        }

        // Add Song Modal
        const modal = document.getElementById('song-modal');
        document.getElementById('btn-open-add-modal').addEventListener('click', () => {
            document.getElementById('song-form').reset();
            document.getElementById('form-song-id').value = '';
            document.getElementById('modal-title').textContent = 'Add New Song';
            modal.classList.remove('hidden');
        });

        document.getElementById('btn-close-modal').addEventListener('click', () => modal.classList.add('hidden'));
        document.getElementById('btn-cancel-modal').addEventListener('click', () => modal.classList.add('hidden'));

        document.getElementById('song-form').addEventListener('submit', (e) => {
            e.preventDefault();
            const idVal = document.getElementById('form-song-id').value;
            const title = document.getElementById('form-title').value;
            const artist = document.getElementById('form-artist').value;
            const album = document.getElementById('form-album').value || 'Single';
            const genre = document.getElementById('form-genre').value || 'Pop';
            const duration = parseInt(document.getElementById('form-duration').value) || 200;
            const year = parseInt(document.getElementById('form-year').value) || 2024;
            const rating = parseFloat(document.getElementById('form-rating').value) || 4.5;

            const audioFileInputEl = document.getElementById('form-audio-file');
            let uploadedAudioUrl = '';

            if (audioFileInputEl && audioFileInputEl.files && audioFileInputEl.files[0]) {
                const audioFile = audioFileInputEl.files[0];
                uploadedAudioUrl = URL.createObjectURL(audioFile);
            }

            if (idVal) {
                // Edit mode
                const node = this.idMap.get(parseInt(idVal));
                if (node) {
                    node.data.title = title;
                    node.data.artist = artist;
                    node.data.album = album;
                    node.data.genre = genre;
                    node.data.durationSeconds = duration;
                    node.data.year = year;
                    node.data.rating = rating;
                    if (uploadedAudioUrl) node.data.audioUrl = uploadedAudioUrl;
                }
            } else {
                // Add mode — generate unique ID by taking max existing ID + 1
                let maxId = 100;
                this.idMap.forEach((node, id) => { if (id > maxId) maxId = id; });
                const newId = maxId + 1;
                const defaultCover = "data:image/svg+xml;utf8,<svg xmlns='http://www.w3.org/2000/svg' width='100' height='100' viewBox='0 0 100 100'><rect width='100' height='100' fill='%231e293b'/><circle cx='50' cy='50' r='35' fill='%230f172a' stroke='%238b5cf6' stroke-width='4'/><circle cx='50' cy='50' r='12' fill='%23ec4899'/></svg>";
                const newSong = new Song(newId, title, artist, album, genre, duration, year, rating, false, defaultCover, uploadedAudioUrl);
                this.addSongToEngine(newSong);

                if (uploadedAudioUrl) {
                    const node = this.idMap.get(newId);
                    if (node) this.playSong(node);
                }
            }

            modal.classList.add('hidden');
            this.renderAll();
        });
    }

    updatePlayerBar() {
        const toggleBtn = document.getElementById('btn-player-toggle');
        if (this.isPlaying) {
            toggleBtn.innerHTML = '<i class="fa-solid fa-pause"></i>';
        } else {
            toggleBtn.innerHTML = '<i class="fa-solid fa-play"></i>';
        }

        if (this.currentlyPlayingNode) {
            const s = this.currentlyPlayingNode.data;
            document.getElementById('player-title').textContent = s.title;
            document.getElementById('player-artist').textContent = `${s.artist} • ${s.album}`;
            document.getElementById('player-total-time').textContent = s.getFormattedDuration();
            document.getElementById('player-cover').src = s.cover;

            const favBtn = document.getElementById('btn-player-fav');
            favBtn.innerHTML = s.favorite ? '<i class="fa-solid fa-heart" style="color: #ec4899;"></i>' : '<i class="fa-regular fa-heart"></i>';
            favBtn.onclick = () => {
                s.favorite = !s.favorite;
                this.updatePlayerBar();
                this.renderAll();
            };
        }
    }

    renderAll() {
        this.renderPlaylistTable();
        this.renderFavoritesTable();
        this.renderRecentlyPlayedTable();
        this.renderDLLVisualizer();
        this.renderStackVisualizer();
        this.renderQueueVisualizer();
        this.renderBSTVisualizer();
        this.renderHashMapVisualizer();
        this.renderAnalytics();

        document.getElementById('undo-count-label').textContent = `Stack: ${this.undoStack.size()} actions`;
    }

    renderPlaylistTable(query = '', sortCrit = 'ID') {
        const tbody = document.getElementById('playlist-tbody');
        tbody.innerHTML = '';

        let songsToDisplay = [];
        if (sortCrit !== 'ID') {
            const bst = new BST(sortCrit);
            let curr = this.dll.head;
            while (curr) {
                bst.insert(curr.data);
                curr = curr.next;
            }
            songsToDisplay = bst.inorder();
        } else {
            let curr = this.dll.head;
            while (curr) {
                songsToDisplay.push(curr.data);
                curr = curr.next;
            }
        }

        if (query.trim()) {
            const q = query.toLowerCase();
            songsToDisplay = songsToDisplay.filter(s => 
                s.id.toString().includes(q) ||
                s.title.toLowerCase().includes(q) ||
                s.artist.toLowerCase().includes(q) ||
                s.album.toLowerCase().includes(q) ||
                s.genre.toLowerCase().includes(q)
            );
        }

        songsToDisplay.forEach((song, idx) => {
            const node = this.idMap.get(song.id);
            const tr = document.createElement('tr');
            if (this.currentlyPlayingNode && this.currentlyPlayingNode.data.id === song.id) {
                tr.classList.add('playing');
            }

            tr.innerHTML = `
                <td>${idx + 1}</td>
                <td>
                    <div class="song-title-cell">
                        <button class="btn-table-play" data-id="${song.id}">
                            <i class="fa-solid ${this.currentlyPlayingNode && this.currentlyPlayingNode.data.id === song.id && this.isPlaying ? 'fa-pause' : 'fa-play'}"></i>
                        </button>
                        <span>${song.title}</span>
                    </div>
                </td>
                <td>${song.artist}</td>
                <td>${song.album}</td>
                <td><span class="badge">${song.genre}</span></td>
                <td>${song.getFormattedDuration()}</td>
                <td>${song.year}</td>
                <td>⭐ ${song.rating.toFixed(1)}</td>
                <td>
                    <div class="action-btns">
                        <span class="fav-star" data-id="${song.id}">${song.favorite ? '★' : '☆'}</span>
                        <button class="btn-icon-sm edit" data-id="${song.id}"><i class="fa-solid fa-pen"></i></button>
                        <button class="btn-icon-sm danger delete" data-id="${song.id}"><i class="fa-solid fa-trash"></i></button>
                    </div>
                </td>
            `;

            tr.querySelector('.btn-table-play').onclick = () => {
                if (this.currentlyPlayingNode && this.currentlyPlayingNode.data.id === song.id) {
                    this.togglePlayPause();
                } else {
                    this.playSong(node);
                }
            };

            tr.querySelector('.fav-star').onclick = () => {
                song.favorite = !song.favorite;
                this.renderAll();
            };

            tr.querySelector('.delete').onclick = () => {
                this.deleteSongById(song.id);
            };

            tr.querySelector('.edit').onclick = () => {
                document.getElementById('form-song-id').value = song.id;
                document.getElementById('form-title').value = song.title;
                document.getElementById('form-artist').value = song.artist;
                document.getElementById('form-album').value = song.album;
                document.getElementById('form-genre').value = song.genre;
                document.getElementById('form-duration').value = song.durationSeconds;
                document.getElementById('form-year').value = song.year;
                document.getElementById('form-rating').value = song.rating;
                document.getElementById('modal-title').textContent = 'Edit Song Details';
                document.getElementById('song-modal').classList.remove('hidden');
            };

            tbody.appendChild(tr);
        });
    }

    renderFavoritesTable() {
        const tbody = document.getElementById('favorites-tbody');
        tbody.innerHTML = '';
        let curr = this.dll.head;
        let count = 1;

        while (curr) {
            if (curr.data.favorite) {
                const s = curr.data;
                const tr = document.createElement('tr');
                tr.innerHTML = `
                    <td>${count++}</td>
                    <td><strong>${s.title}</strong></td>
                    <td>${s.artist}</td>
                    <td>${s.album}</td>
                    <td>${s.genre}</td>
                    <td>${s.getFormattedDuration()}</td>
                    <td>⭐ ${s.rating.toFixed(1)}</td>
                    <td><button class="btn-icon-sm danger" onclick="app.toggleFav(${s.id})"><i class="fa-solid fa-heart-crack"></i></button></td>
                `;
                tbody.appendChild(tr);
            }
            curr = curr.next;
        }
    }

    toggleFav(id) {
        const songNode = this.idMap.get(id);
        if (songNode) {
            songNode.data.favorite = !songNode.data.favorite;
            this.renderAll();
        }
    }

    renderRecentlyPlayedTable() {
        const tbody = document.getElementById('recently-played-tbody');
        tbody.innerHTML = '';
        const items = [...this.recentlyPlayedQueue.items].reverse();

        items.forEach((s, idx) => {
            const tr = document.createElement('tr');
            tr.innerHTML = `
                <td>#${idx + 1}</td>
                <td>${s.id}</td>
                <td><strong>${s.title}</strong></td>
                <td>${s.artist}</td>
                <td>${s.album}</td>
                <td>${s.getFormattedDuration()}</td>
                <td><button class="btn-table-play" onclick="app.playSongById(${s.id})"><i class="fa-solid fa-play"></i></button></td>
            `;
            tbody.appendChild(tr);
        });
    }

    playSongById(id) {
        const node = this.idMap.get(id);
        if (node) this.playSong(node);
    }

    renderDLLVisualizer() {
        const canvas = document.getElementById('dll-nodes-canvas');
        canvas.innerHTML = '';

        let curr = this.dll.head;
        while (curr) {
            const nodeDiv = document.createElement('div');
            let nodeClass = 'dll-node';
            if (curr === this.dll.head) nodeClass += ' head-node';
            if (curr === this.dll.tail) nodeClass += ' tail-node';

            nodeDiv.className = nodeClass;
            nodeDiv.innerHTML = `
                <div class="node-badge">${curr === this.dll.head ? 'HEAD' : curr === this.dll.tail ? 'TAIL' : `Node #${curr.data.id}`}</div>
                <div class="node-title">${curr.data.title}</div>
                <div class="node-artist">${curr.data.artist}</div>
                <div class="node-ptr">prev: ${curr.prev ? curr.prev.data.id : 'null'} | next: ${curr.next ? curr.next.data.id : 'null'}</div>
            `;
            canvas.appendChild(nodeDiv);

            if (curr.next) {
                const arrow = document.createElement('div');
                arrow.className = 'dll-arrow';
                arrow.innerHTML = '⇄';
                canvas.appendChild(arrow);
            }
            curr = curr.next;
        }
    }

    renderStackVisualizer() {
        const canvas = document.getElementById('stack-frames-container');
        canvas.innerHTML = '';

        if (this.undoStack.isEmpty()) {
            canvas.innerHTML = '<div style="color: var(--text-muted); text-align: center;">Stack is currently empty. Delete or update a song to push frames onto the Stack.</div>';
            return;
        }

        this.undoStack.items.forEach((action, idx) => {
            const frame = document.createElement('div');
            frame.className = 'stack-frame';
            frame.innerHTML = `
                <div>
                    <strong>[${action.type}]</strong> "${action.song.title}"
                    <div style="font-size: 0.75rem; color: var(--text-muted);">Original Position: ${action.position}</div>
                </div>
                <span class="badge" style="background: var(--accent-pink);">Top - ${idx + 1}</span>
            `;
            canvas.appendChild(frame);
        });
    }

    renderQueueVisualizer() {
        const canvas = document.getElementById('queue-slots-container');
        canvas.innerHTML = '';

        if (this.recentlyPlayedQueue.isEmpty()) {
            canvas.innerHTML = '<div style="color: var(--text-muted); text-align: center;">Queue is empty. Play songs to populate history slots.</div>';
            return;
        }

        const flexDiv = document.createElement('div');
        flexDiv.style.display = 'flex';
        flexDiv.style.gap = '12px';

        this.recentlyPlayedQueue.items.forEach((song, idx) => {
            const slot = document.createElement('div');
            slot.style.background = 'rgba(15, 23, 42, 0.9)';
            slot.style.border = '1px solid var(--accent-secondary)';
            slot.style.borderRadius = '8px';
            slot.style.padding = '12px 16px';
            slot.innerHTML = `
                <div style="font-size: 0.7rem; color: var(--accent-secondary);">Slot ${idx + 1} (FIFO)</div>
                <div style="font-weight: 700;">${song.title}</div>
            `;
            flexDiv.appendChild(slot);
        });
        canvas.appendChild(flexDiv);
    }

    renderBSTVisualizer() {
        const canvas = document.getElementById('bst-tree-container');
        canvas.innerHTML = '';
        const bst = new BST('TITLE');
        let curr = this.dll.head;
        while (curr) {
            bst.insert(curr.data);
            curr = curr.next;
        }
        const sorted = bst.inorder();

        const grid = document.createElement('div');
        grid.style.display = 'grid';
        grid.style.gridTemplateColumns = 'repeat(auto-fill, minmax(200px, 1fr))';
        grid.style.gap = '14px';
        grid.style.width = '100%';

        sorted.forEach((song) => {
            const card = document.createElement('div');
            card.style.background = 'rgba(15, 23, 42, 0.9)';
            card.style.border = '1px solid var(--border-color)';
            card.style.borderRadius = '10px';
            card.style.padding = '14px';
            card.innerHTML = `
                <div style="color: var(--accent-primary); font-weight: 700;">🌲 ${song.title}</div>
                <div style="font-size: 0.8rem; color: var(--text-muted);">${song.artist}</div>
            `;
            grid.appendChild(card);
        });
        canvas.appendChild(grid);
    }

    renderHashMapVisualizer() {
        const canvas = document.getElementById('hash-map-container');
        canvas.innerHTML = '';

        const grid = document.createElement('div');
        grid.style.display = 'grid';
        grid.style.gridTemplateColumns = 'repeat(auto-fill, minmax(220px, 1fr))';
        grid.style.gap = '12px';
        grid.style.width = '100%';

        this.idMap.forEach((node, id) => {
            const bucket = document.createElement('div');
            bucket.style.background = 'rgba(15, 23, 42, 0.9)';
            bucket.style.border = '1px solid var(--accent-secondary)';
            bucket.style.borderRadius = '8px';
            bucket.style.padding = '12px';
            bucket.innerHTML = `
                <div style="font-size: 0.75rem; color: var(--accent-secondary); font-family: monospace;">Key (ID): ${id}</div>
                <div style="font-weight: 700;">Value ➔ Node [ "${node.data.title}" ]</div>
            `;
            grid.appendChild(bucket);
        });
        canvas.appendChild(grid);
    }

    renderAnalytics() {
        const totalCount = this.dll.getSize();
        let totalSecs = 0;
        let totalRating = 0;
        let favCount = 0;

        let longest = null;
        let shortest = null;
        let highest = null;

        let curr = this.dll.head;
        while (curr) {
            const s = curr.data;
            totalSecs += s.durationSeconds;
            totalRating += s.rating;
            if (s.favorite) favCount++;

            if (!longest || s.durationSeconds > longest.durationSeconds) longest = s;
            if (!shortest || s.durationSeconds < shortest.durationSeconds) shortest = s;
            if (!highest || s.rating > highest.rating) highest = s;

            curr = curr.next;
        }

        document.getElementById('stat-total-songs').textContent = totalCount;
        const mins = Math.floor(totalSecs / 60);
        const secs = totalSecs % 60;
        document.getElementById('stat-total-dur').textContent = `${mins}m ${secs}s`;
        document.getElementById('stat-avg-rating').textContent = totalCount ? (totalRating / totalCount).toFixed(2) : '0.0';
        document.getElementById('stat-favorites-count').textContent = favCount;

        document.getElementById('record-longest-song').textContent = longest ? `"${longest.title}" (${longest.getFormattedDuration()})` : 'N/A';
        document.getElementById('record-shortest-song').textContent = shortest ? `"${shortest.title}" (${shortest.getFormattedDuration()})` : 'N/A';
        document.getElementById('record-highest-rated').textContent = highest ? `"${highest.title}" (${highest.rating.toFixed(1)} / 5.0)` : 'N/A';
    }
}

// Global App Instance
let app;
document.addEventListener('DOMContentLoaded', () => {
    app = new PlaylistApp();
});
