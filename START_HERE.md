# 🎸 START HERE

Welcome to **Guitar Sound Mimic** - Your AI-powered guitar tone profiling system!

## 📋 Your Questions Answered

### Q1: Can the app filter out only the guitar sound?

**YES!** ✅ 

I've included **AI-powered guitar isolation** using Demucs (state-of-the-art deep learning from Meta/Facebook).

**How it works:**
```bash
# You have a song with guitar, drums, bass, vocals, etc.
python tools/separate_guitar.py song_with_everything.mp3 guitar_only.wav

# Now you have ONLY the guitar to profile!
```

The app itself analyzes whatever audio you feed it, but the included Python tool extracts just the guitar from mixed tracks first.

### Q2: How do I run this?

**Three simple steps:**

```bash
# 1. Build the app (one-time, ~5 minutes)
cd /Users/dan.obrien/code/guitar-sound-mimic
./setup.sh

# 2. Install Python AI tools (one-time, ~2 minutes)
pip3 install demucs torch torchaudio

# 3. Run it!
./build/GuitarSoundMimic_artefacts/Release/GuitarSoundMimic.app/Contents/MacOS/GuitarSoundMimic
```

## 🚀 Complete Workflow

### Extract Guitar from Any Song
```bash
python3 tools/separate_guitar.py ~/Music/comfortably_numb.mp3 gilmour_tone.wav
```
Takes 30-60 seconds. Produces clean guitar-only audio.

### Profile the Tone
1. Open app
2. Play `gilmour_tone.wav` through your computer
3. Click "Start Profiling" → wait 5-10 sec → "Stop Profiling"

### Apply to Your Guitar
1. Connect guitar to audio interface
2. Click "Apply Profile"
3. Play guitar - sounds like David Gilmour! 🎸

## 📁 Project Files

### Documentation (Start Here!)
- **START_HERE.md** ← You are here
- **RUN_ME_FIRST.md** - Detailed step-by-step setup
- **CHEATSHEET.md** - Quick command reference
- **WORKFLOW.md** - Visual pipeline diagram

### Detailed Docs
- **README.md** - Full documentation
- **BUILDING.md** - Build instructions (all platforms)
- **QUICKSTART.md** - Beginner's guide
- **tools/README.md** - Guitar separation details

### Source Code
- **Source/** - C++ application code
  - Audio processing
  - Tone profiling
  - Real-time matching
- **tools/** - Python guitar isolation

### Setup
- **setup.sh** - One-command setup (macOS/Linux)
- **CMakeLists.txt** - Build configuration

## 🎯 What You Can Do

✅ Extract guitar from **any song** (even with full band)  
✅ Profile legendary tones (Gilmour, Hendrix, Clapton, etc.)  
✅ Apply them to **your guitar in real-time**  
✅ Low latency (< 10ms) for live playing  
✅ **37 unit tests** ensure quality and reliability  
✅ Works on macOS, Windows, Linux  
✅ Future: Convert to VST/AU plugin for DAWs  

## ⚡ Quick Commands

```bash
# Build everything
./setup.sh

# Run tests (verify everything works)
./run_tests.sh

# Install AI tools
pip3 install demucs torch torchaudio

# Extract guitar from song
python3 tools/separate_guitar.py input.mp3 output.wav

# Run the app
open ./build/GuitarSoundMimic_artefacts/Release/GuitarSoundMimic.app
```

## 🎵 Try These Iconic Tones

```bash
# David Gilmour (Pink Floyd)
python3 tools/separate_guitar.py comfortably_numb.mp3 gilmour.wav

# Jimi Hendrix
python3 tools/separate_guitar.py little_wing.mp3 hendrix.wav

# Slash (Guns N' Roses)
python3 tools/separate_guitar.py sweet_child.mp3 slash.wav

# Then profile each one in the app!
```

## 🏗️ Architecture

```
┌────────────────────────┐
│   Original Song        │
│   (with all instruments)│
└──────────┬─────────────┘
           │
           ▼
┌────────────────────────┐
│  AI Separation         │
│  (Demucs Model)        │
└──────────┬─────────────┘
           │
           ▼
┌────────────────────────┐
│  Guitar Track Only     │
└──────────┬─────────────┘
           │
           ▼
┌────────────────────────┐
│  Tone Profiler         │
│  (FFT Analysis)        │
└──────────┬─────────────┘
           │
           ▼
┌────────────────────────┐
│  Tone Profile Data     │
└──────────┬─────────────┘
           │
           ▼
┌────────────────────────┐
│  Your Guitar Input     │
└──────────┬─────────────┘
           │
           ▼
┌────────────────────────┐
│  Real-time Matcher     │
│  (EQ + Compression)    │
└──────────┬─────────────┘
           │
           ▼
┌────────────────────────┐
│  Sounds like reference!│
└────────────────────────┘
```

## 🎓 Technology Used

- **C++17** - Core application
- **JUCE Framework** - Audio processing & UI
- **Python 3** - Guitar isolation
- **Demucs AI** - Deep learning separation
- **FFT Analysis** - Frequency profiling
- **DSP Filters** - Real-time tone matching

## 📚 Recommended Reading Order

1. **START_HERE.md** ← You are here!
2. **RUN_ME_FIRST.md** - Get it running
3. **TESTING.md** - Run unit tests
4. **CHEATSHEET.md** - Quick reference
5. **WORKFLOW.md** - Visual guide
6. **README.md** - Deep dive

## 🎸 Philosophy

This project gives you the power to:
- Sound like ANY guitarist you admire
- Analyze what makes great tones great
- Build a library of legendary tones
- Learn from the masters by playing through their tones

## 💻 Working on Personal Laptop?

**Can't install dependencies on work laptop?**

1. **Push to GitHub:**
   ```bash
   git init
   git add .
   git commit -m "Initial commit"
   git remote add origin https://github.com/YOU/guitar-sound-mimic.git
   git push -u origin main
   ```

2. **Clone on personal laptop:**
   ```bash
   git clone https://github.com/YOU/guitar-sound-mimic.git
   cd guitar-sound-mimic
   ./setup.sh
   ```

See **PUSH_TO_GIT.md** for step-by-step guide!

## 🔮 Future Plans

- [ ] Save/load tone profiles
- [ ] VST3/AU/AAX plugin for DAWs
- [ ] Machine learning-based matching
- [ ] Cab simulator with impulse responses
- [ ] Effect detection (reverb, delay, etc.)
- [ ] Tone marketplace/sharing

## ✨ Credits

**Built by:** You (dan.obrien)  
**Framework:** JUCE (https://juce.com)  
**AI Separation:** Demucs by Meta AI  
**Inspired by:** Kemper, Quad Cortex, Neural DSP  

---

## 🎯 Next Step

**Open RUN_ME_FIRST.md** for detailed setup instructions!

```bash
# Or just run this now:
./setup.sh
```

---

🎸 **Happy Tone Profiling!** 🎸

