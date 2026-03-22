# 🎸 RUN ME FIRST - Complete Setup Guide

This guide will get you from zero to profiling guitar tones in under 10 minutes.

## ⏱️ Quick Summary

1. **Build the C++ app** (one-time, ~5 minutes)
2. **Install Python tools** (one-time, ~2 minutes)
3. **Extract guitar from a song** (~60 seconds per song)
4. **Profile and play!** (ongoing)

---

## 📋 Step-by-Step Instructions

### Step 1: Build the Application (One Time Only)

Open Terminal and run:

```bash
cd /Users/dan.obrien/code/guitar-sound-mimic
./setup.sh
```

**What this does:**
- Downloads JUCE framework (~100MB)
- Compiles the C++ application (~2-5 minutes)
- Creates the application in `build/` directory

**If setup.sh fails**, install dependencies:
```bash
# Install Xcode Command Line Tools
xcode-select --install

# Install CMake
brew install cmake

# Try again
./setup.sh
```

**Expected output:**
```
✅ CMake found
✅ JUCE cloned successfully
✅ Build directory created
⚙️  Configuring CMake...
🔨 Building project...
✅ Build complete!
```

### Optional: Verify Installation with Tests

```bash
./run_tests.sh
```

This runs unit tests to ensure everything is working correctly.

**Expected output:**
```
🧪 Running tests...
=====================================
  Guitar Sound Mimic - Unit Tests   
=====================================
ToneProfiler: ✓ ALL PASSED (9 tests)
ToneMatcher: ✓ ALL PASSED (8 tests)
FrequencyAnalyzer: ✓ ALL PASSED (10 tests)
ToneProfile: ✓ ALL PASSED (10 tests)
Total: 37 tests, ALL PASSED ✓
✅ All tests passed!
```

---

### Step 2: Install Python Tools (One Time Only)

```bash
pip3 install demucs torch torchaudio soundfile numpy scipy
```

**If you don't have pip:**
```bash
# Install Homebrew (if not installed)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install Python
brew install python

# Try pip3 again
pip3 install demucs torch torchaudio
```

**This takes 1-2 minutes** and downloads ~500MB of dependencies.

---

### Step 3: Extract Guitar from a Song

Now the fun part! Let's extract a guitar tone.

```bash
# Download or find a song you like (e.g., a .mp3 file)
# Let's say you have: ~/Music/favorite_song.mp3

# Extract the guitar
python3 tools/separate_guitar.py ~/Music/favorite_song.mp3 my_guitar_tone.wav
```

**What happens:**
```
🎸 Separating guitar from: /Users/dan.obrien/Music/favorite_song.mp3
   (This may take 30-60 seconds...)
📦 Loading Demucs model...
📂 Loading audio file...
🧠 Running AI separation model...
💾 Saving guitar track to: my_guitar_tone.wav
✅ Guitar extraction complete!
```

**Result:** You now have `my_guitar_tone.wav` - a clean guitar-only track!

---

### Step 4: Run the Application

```bash
./build/GuitarSoundMimic_artefacts/Release/GuitarSoundMimic.app/Contents/MacOS/GuitarSoundMimic
```

**Or double-click** in Finder:
```bash
open ./build/GuitarSoundMimic_artefacts/Release/GuitarSoundMimic.app
```

---

### Step 5: Configure Audio

In the application:

1. **Menu Bar → Options → Audio Settings**
2. Select your devices:
   - **Input**: Your guitar interface (e.g., Focusrite, Universal Audio)
   - **Output**: Headphones or speakers
3. **Sample Rate**: 44100 Hz
4. **Buffer Size**: 256 samples (adjust for latency)
5. Click **OK**

**Don't have an audio interface?**
- Use built-in input/output for testing
- For real use, you'll want a proper guitar interface

---

### Step 6: Profile the Guitar Tone

1. **Play the extracted guitar file** (`my_guitar_tone.wav`) through your computer
   - Use any audio player (QuickTime, VLC, Music app, etc.)
   - Set volume to moderate level
   
2. **In the Guitar Sound Mimic app:**
   - Click **"Start Profiling"**
   - Status shows: "Profiling... Play the reference audio!"
   
3. **Let it run for 5-10 seconds** while the audio plays

4. Click **"Stop Profiling"**
   - Status shows: "Profile Generated!"

---

### Step 7: Apply to Your Guitar

1. **Plug in your guitar** to your audio interface

2. **In the app:**
   - Click **"Apply Profile"**
   - Status shows: "Applying Profile - Play Your Guitar!"

3. **Play your guitar!** 🎸
   - You should hear the profiled tone applied
   - Watch the input/output level meters

4. **Use "Bypass"** to hear your guitar without processing (for comparison)

---

## 🎉 You're Done!

You've successfully:
- ✅ Built the application
- ✅ Installed AI separation tools
- ✅ Extracted a guitar tone from a song
- ✅ Profiled the tone
- ✅ Applied it to your guitar

---

## 🎵 Try These Next

### Extract More Tones

```bash
# Hendrix tone
python3 tools/separate_guitar.py ~/Music/little_wing.mp3 hendrix_tone.wav

# Gilmour tone
python3 tools/separate_guitar.py ~/Music/comfortably_numb.mp3 gilmour_tone.wav

# Slash tone
python3 tools/separate_guitar.py ~/Music/sweet_child.mp3 slash_tone.wav
```

### Create a Tone Library

```bash
# Create a folder for your tones
mkdir guitar_tones

# Extract multiple songs
python3 tools/separate_guitar.py song1.mp3 guitar_tones/tone1.wav
python3 tools/separate_guitar.py song2.mp3 guitar_tones/tone2.wav
python3 tools/separate_guitar.py song3.mp3 guitar_tones/tone3.wav
```

---

## ❓ Troubleshooting

### Build Fails

**Error**: "CMake not found"
```bash
brew install cmake
```

**Error**: "Developer tools not found"
```bash
xcode-select --install
```

### Python Issues

**Error**: "demucs not found"
```bash
pip3 install demucs
```

**Error**: "torch not found"
```bash
pip3 install torch torchaudio
```

### Audio Issues

**No input/output:**
- Check Options → Audio Settings
- Verify cables are connected
- Try different buffer size

**High latency:**
- Reduce buffer size (128 or 64 samples)
- Use a dedicated audio interface

**Tone doesn't sound right:**
- Ensure you used guitar isolation tool
- Profile longer (5-10 seconds minimum)
- Try different reference audio

### Separation Issues

**Takes forever:**
- Normal! 30-60 seconds per song on CPU
- Use GPU for faster: requires NVIDIA GPU and CUDA setup

**Poor separation quality:**
- Demucs works best with commercial recordings
- Some heavily-processed tones are harder to separate
- Try different songs

---

## 📚 Next Steps

- **Read CHEATSHEET.md** for quick commands
- **Read README.md** for full documentation
- **Read tools/README.md** for advanced separation options
- **Experiment!** Try different songs and guitarists

---

## 🆘 Still Having Issues?

1. Check the **README.md** for detailed docs
2. Check the **BUILDING.md** for build problems
3. Check the **tools/README.md** for separation problems

---

## 🎸 Suggested First Profiles

Start with these iconic tones:

| Guitarist | Song | Characteristic |
|-----------|------|---------------|
| David Gilmour | Comfortably Numb | Smooth sustain, Hiwatt/Strat |
| Jimi Hendrix | Little Wing | Warm, clean-ish, Stratocaster |
| Slash | Sweet Child O' Mine | Les Paul, Marshall, classic rock |
| John Mayer | Gravity | Blues tone, Strat, Dumble-ish |
| SRV | Pride and Joy | Texas blues, Tube Screamer |

---

**Enjoy profiling guitar tones! 🎸🔥**

