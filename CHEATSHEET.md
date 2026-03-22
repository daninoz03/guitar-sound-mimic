# Guitar Sound Mimic - Quick Reference

## 🚀 First Time Setup

### 1. Build the App
```bash
cd /Users/dan.obrien/code/guitar-sound-mimic
./setup.sh
```

### 2. Run Tests (Optional but Recommended)
```bash
./run_tests.sh
```

### 3. Install Python Tools (for guitar isolation)
```bash
pip install demucs torch torchaudio
```

## 🎵 Workflow: Profile a Song

### Extract Guitar from Mixed Audio
```bash
# Basic usage
python tools/separate_guitar.py input.mp3 guitar_only.wav

# Examples
python tools/separate_guitar.py "Pink Floyd - Comfortably Numb.mp3" gilmour_tone.wav
python tools/separate_guitar.py "Eric Clapton - Layla.mp3" clapton_tone.wav
```

### Profile the Isolated Guitar
1. Run the app: `./build/GuitarSoundMimic_artefacts/Release/GuitarSoundMimic.app/Contents/MacOS/GuitarSoundMimic`
2. Play `guitar_only.wav` through your computer
3. Click **"Start Profiling"** (let run 5-10 seconds)
4. Click **"Stop Profiling"**

### Apply to Your Guitar
1. Connect your guitar to audio interface
2. Click **"Apply Profile"**
3. Play your guitar - it now has the profiled tone!

## 📁 Project Structure

```
guitar-sound-mimic/
├── setup.sh                    # Build everything
├── tools/
│   ├── separate_guitar.py     # Extract guitar from songs
│   ├── requirements.txt       # Python dependencies
│   └── README.md              # Detailed separation docs
├── Source/                     # C++ source code
└── build/                      # Compiled application
```

## 🎛️ Audio Settings

**Menu Bar → Options → Audio Settings**

| Setting | Recommended Value |
|---------|------------------|
| Input Device | Your guitar interface |
| Output Device | Headphones/Speakers |
| Sample Rate | 44100 Hz or 48000 Hz |
| Buffer Size | 128-256 samples |

## 🎸 Tips & Tricks

### Best Results
- ✅ Use AI separation for songs with multiple instruments
- ✅ Profile 5-10 seconds of audio (not just 1-2 seconds)
- ✅ Use sections with active guitar playing
- ✅ Match your input volume to the profiled volume

### Common Mistakes
- ❌ Profiling full mix without separation (includes drums/bass/vocals)
- ❌ Profiling too short (< 3 seconds)
- ❌ Very quiet/loud audio
- ❌ Clipped/distorted input

## 🐛 Troubleshooting

| Problem | Solution |
|---------|----------|
| Can't build | `xcode-select --install` then `brew install cmake` |
| No audio in/out | Check Options → Audio Settings |
| Python errors | `pip install demucs torch torchaudio` |
| High latency | Reduce buffer size, use audio interface |
| Profile sounds wrong | Use guitar isolation tool first |

## ⌨️ Commands Reference

### Build Commands
```bash
# Clean rebuild
rm -rf build && ./setup.sh

# Build manually
mkdir build && cd build
cmake ..
cmake --build . --config Release

# Run tests
./run_tests.sh

# Or manually
cd build
cmake --build . --target GuitarSoundMimicTests
./GuitarSoundMimicTests_artefacts/Debug/GuitarSoundMimicTests
```

### Guitar Separation Commands
```bash
# Standard (best quality)
python tools/separate_guitar.py song.mp3 output.wav

# Simple method (faster, lower quality)
python tools/separate_guitar.py -m simple song.mp3 output.wav

# Batch process folder
for f in songs/*.mp3; do 
    python tools/separate_guitar.py "$f" "separated/$(basename "$f" .mp3)_guitar.wav"
done
```

### Running the App
```bash
# macOS
./build/GuitarSoundMimic_artefacts/Release/GuitarSoundMimic.app/Contents/MacOS/GuitarSoundMimic

# Or double-click
open ./build/GuitarSoundMimic_artefacts/Release/GuitarSoundMimic.app
```

## 📊 Performance

| Operation | Time (approx) |
|-----------|--------------|
| Build app | 2-5 minutes |
| Separate guitar (Demucs) | 30-60 sec/song |
| Profile tone | 5-10 seconds |
| Real-time processing | < 10ms latency |

## 🎯 Example Workflows

### Workflow 1: Profile David Gilmour's Tone
```bash
# 1. Get a Pink Floyd song
# 2. Extract guitar
python tools/separate_guitar.py comfortably_numb.mp3 gilmour_solo.wav

# 3. Launch app and profile gilmour_solo.wav
# 4. Play your guitar through the app
```

### Workflow 2: Create a Tone Library
```bash
# Separate multiple songs
python tools/separate_guitar.py song1.mp3 tones/tone1_guitar.wav
python tools/separate_guitar.py song2.mp3 tones/tone2_guitar.wav
python tools/separate_guitar.py song3.mp3 tones/tone3_guitar.wav

# Profile each one in the app
# Save profiles for later (future feature)
```

### Workflow 3: A/B Test Different Tones
```bash
# Extract two different guitar tones
python tools/separate_guitar.py blues_song.mp3 blues_tone.wav
python tools/separate_guitar.py rock_song.mp3 rock_tone.wav

# Profile blues_tone.wav → Apply → Play guitar
# Profile rock_tone.wav → Apply → Play guitar
# Use Bypass to compare
```

## 📚 Documentation

- **README.md** - Full documentation
- **BUILDING.md** - Build instructions (all platforms)
- **QUICKSTART.md** - Beginner's guide
- **tools/README.md** - Guitar separation details
- **CHEATSHEET.md** - This file!

## 🔗 Quick Links

- JUCE Framework: https://juce.com
- Demucs (separation): https://github.com/facebookresearch/demucs
- Report Issues: Create issue in your repository

## 💡 Pro Tips

1. **Profile multiple sections** of the same song for better accuracy
2. **Use solos** where guitar is prominent (even in full mix)
3. **Match playing style** - fingerpicking profiles won't match heavy strumming
4. **Experiment!** Different songs = different tones
5. **GPU acceleration** for faster separation: `pip install torch --index-url https://download.pytorch.org/whl/cu118`

## 🎵 Suggested Songs to Profile

Good reference tracks with distinctive guitar tones:

- **David Gilmour**: "Comfortably Numb", "Shine On You Crazy Diamond"
- **Jimi Hendrix**: "Little Wing", "All Along The Watchtower"
- **Slash**: "Sweet Child O' Mine", "November Rain"
- **Eric Clapton**: "Layla", "Wonderful Tonight"
- **John Mayer**: "Slow Dancing In A Burning Room", "Gravity"
- **SRV**: "Pride and Joy", "Texas Flood"
- **Eddie Van Halen**: "Eruption", "Ain't Talkin' 'Bout Love"

---

**Need more help?** See the full README.md or tools/README.md for detailed documentation.

