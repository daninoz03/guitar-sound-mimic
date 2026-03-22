# Quick Start Guide

## Prerequisites

### Install CMake (if not already installed)
```bash
# Check if you have CMake
cmake --version

# If not installed, use Homebrew
brew install cmake

# If you don't have Homebrew:
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

### Install Git (usually pre-installed on macOS)
```bash
git --version
```

## Step-by-Step Setup

### 1. Open Terminal and Navigate to Project
```bash
cd /Users/dan.obrien/code/guitar-sound-mimic
```

### 2. Run the Setup Script
```bash
./setup.sh
```

This will:
- Clone the JUCE framework
- Configure CMake
- Build the application (takes 2-5 minutes)

**OR** do it manually:

```bash
# Clone JUCE
git clone --depth 1 https://github.com/juce-framework/JUCE.git

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build (this takes a few minutes)
cmake --build . --config Release
```

### 3. Run the Application
```bash
./build/GuitarSoundMimic_artefacts/Release/GuitarSoundMimic.app/Contents/MacOS/GuitarSoundMimic
```

Or double-click:
```bash
open ./build/GuitarSoundMimic_artefacts/Release/GuitarSoundMimic.app
```

## Initial Setup

### Configure Audio Device

1. **Open the app**
2. **Go to menu bar** → Click "Options" (or the app menu) → "Audio Settings"
3. **Select your audio interface**:
   - **Input Device**: Your guitar interface (e.g., Focusrite, Universal Audio, etc.)
   - **Output Device**: Your speakers/headphones
   - **Sample Rate**: 44100 Hz or 48000 Hz
   - **Buffer Size**: Start with 256 samples (adjust lower for less latency)

### Test Audio Levels

1. Play your guitar
2. Check the **Input level meter** - should show activity
3. If no signal:
   - Check cables
   - Verify input device selection
   - Check input gain on your interface

## Using the App

### Create a Tone Profile (Current Version - Full Mix)

**IMPORTANT**: In the current version, you need guitar-only audio for best results.

#### Method 1: Use Guitar-Only Recordings
Find or create recordings with ONLY guitar:
- Isolated guitar tracks (YouTube sometimes has these)
- Your own recordings
- Stems from songs

#### Method 2: Record Reference Audio
1. Play a song through your computer speakers
2. Click **"Start Profiling"**
3. Let it run for 5-10 seconds
4. Click **"Stop Profiling"**
5. Status will show "Profile Generated!"

### Apply the Profile to Your Guitar

1. Click **"Apply Profile"**
2. Play your guitar
3. Listen to the processed tone
4. Use **"Bypass"** to compare original vs. processed

### Tips

- **Profile longer sections** (5-10 seconds) for better results
- **Use clean audio** without distortion/clipping
- **Match volume levels** - don't profile at very low volume
- **Experiment!** Try different reference sources

## Troubleshooting

### "Command not found: cmake"
```bash
brew install cmake
```

### "No audio input/output"
- Check **Options → Audio Settings**
- Ensure your interface is connected
- Try restarting the app

### "Build fails"
Make sure Xcode Command Line Tools are installed:
```bash
xcode-select --install
```

### High Latency
- Reduce buffer size in Audio Settings
- Use a professional audio interface (not built-in audio)
- Close other audio applications

### Profile doesn't sound right
- Current version analyzes full mix (drums, bass, etc.)
- See main README for planned guitar isolation feature
- Use guitar-only audio for best results

## Next Steps

See **README.md** for:
- Detailed feature documentation
- Future roadmap
- Contributing guidelines

See **BUILDING.md** for:
- Advanced build options
- Cross-platform instructions
- Customization options





