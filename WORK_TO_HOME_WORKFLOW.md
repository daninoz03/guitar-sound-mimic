# Work Laptop → Personal Laptop Workflow

## Your Situation

✅ **Work Laptop:** Can write code, can't install dependencies  
✅ **Personal Laptop:** Full control, can install anything  
✅ **Solution:** Git sync between machines  

---

## 📤 Step 1: Push from Work Laptop (Now)

### Quick Commands

```bash
# Navigate to project
cd /Users/dan.obrien/code/guitar-sound-mimic

# Initialize git
git init

# Add all files
git add .

# Commit (this saves your work locally)
git commit -m "Initial commit: Guitar Sound Mimic v0.1.0"
```

### Create GitHub Repository

1. Open browser: **https://github.com/new**
2. Repository name: `guitar-sound-mimic`
3. Make it Private (recommended) or Public
4. **Don't** check "Initialize with README"
5. Click "Create repository"

### Push to GitHub

GitHub will show you commands. Use these:

```bash
# Add remote (replace YOUR_USERNAME with your GitHub username)
git remote add origin https://github.com/YOUR_USERNAME/guitar-sound-mimic.git

# Push code
git branch -M main
git push -u origin main
```

**✅ Done!** Your code is now safely on GitHub.

---

## 📥 Step 2: Clone on Personal Laptop (At Home)

### Install Prerequisites (One-Time)

**macOS:**
```bash
# Install Homebrew (package manager)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install CMake
brew install cmake

# Install Xcode Command Line Tools
xcode-select --install
```

**Linux (Ubuntu/Debian):**
```bash
sudo apt update
sudo apt install build-essential cmake git
sudo apt install libasound2-dev libjack-jackd2-dev libfreetype6-dev \
    libx11-dev libxinerama-dev libxrandr-dev libxcursor-dev
```

**Windows:**
- Install Visual Studio 2019+ with C++ workload
- Install CMake from https://cmake.org/download/
- Install Git from https://git-scm.com/

### Clone and Build

```bash
# Go to your projects directory
cd ~/code  # or wherever you keep projects

# Clone your repository (replace YOUR_USERNAME)
git clone https://github.com/YOUR_USERNAME/guitar-sound-mimic.git

# Enter directory
cd guitar-sound-mimic

# Build everything (takes ~5 minutes first time)
./setup.sh
```

This will:
- ✅ Download JUCE framework (~100MB)
- ✅ Configure build system
- ✅ Compile the application
- ✅ Create runnable app

### Run Tests

```bash
./run_tests.sh
```

Expected output:
```
✅ All tests passed!
```

### Install Python Tools (for guitar separation)

```bash
# Install AI dependencies
pip3 install demucs torch torchaudio
```

### Run the App!

**macOS:**
```bash
./build/GuitarSoundMimic_artefacts/Release/GuitarSoundMimic.app/Contents/MacOS/GuitarSoundMimic
```

Or double-click:
```bash
open ./build/GuitarSoundMimic_artefacts/Release/GuitarSoundMimic.app
```

**Linux:**
```bash
./build/GuitarSoundMimic_artefacts/Release/GuitarSoundMimic
```

**Windows:**
```bash
.\build\GuitarSoundMimic_artefacts\Release\GuitarSoundMimic.exe
```

---

## 🔄 Step 3: Keep in Sync

### Making Changes on Personal Laptop

```bash
# Make your changes to the code
# ...

# Check what changed
git status

# Add changes
git add .

# Commit with message
git commit -m "Added new feature"

# Push to GitHub
git push
```

### Getting Changes on Work Laptop

```bash
cd /Users/dan.obrien/code/guitar-sound-mimic

# Pull latest changes
git pull

# Now you have the latest code
# (Can't build/run here, but code is synced for editing)
```

### Getting Changes on Personal Laptop (Later)

```bash
cd ~/code/guitar-sound-mimic

# Pull latest changes
git pull

# Rebuild if source code changed
cd build
cmake --build . --config Release
```

---

## 📊 What Gets Synced

**Synced via Git (~200KB):**
- ✅ C++ source code
- ✅ Python tools
- ✅ Tests
- ✅ Documentation
- ✅ Build scripts

**Not Synced (rebuilt on each machine):**
- ❌ JUCE framework (100MB) - auto-downloaded
- ❌ Build artifacts - auto-built
- ❌ Compiled app - auto-compiled

This keeps your repo tiny and fast!

---

## 🎯 Typical Workflow

### Weekday (Work Laptop)
```bash
# Write code, add features, write tests
vim Source/Audio/ToneProfiler.cpp

# Commit changes
git add .
git commit -m "Improved frequency analysis"

# Push to GitHub
git push
```

### Evening (Personal Laptop)
```bash
# Get latest changes
git pull

# Build if needed
./setup.sh

# Test it out
./run_tests.sh
./build/GuitarSoundMimic_artefacts/Release/GuitarSoundMimic.app/Contents/MacOS/GuitarSoundMimic

# Try with real guitar
python3 tools/separate_guitar.py ~/Music/song.mp3 guitar.wav
# Profile in app, play guitar!
```

### Weekend (Personal Laptop)
```bash
# Experiment with major changes
git checkout -b experiment

# Make big changes
# ...

# Test
./run_tests.sh

# If good, merge
git checkout main
git merge experiment
git push

# Now available on work laptop Monday
```

---

## 🔐 Keep It Private

If you want the code private:
1. Create as **Private** repository on GitHub
2. Only you can access it
3. Free for personal use

You can make it public later if you want!

---

## 💡 Pro Tips

### 1. Use Branches for Experiments
```bash
git checkout -b feature/better-eq
# Experiment safely
git checkout main  # Back to stable
```

### 2. Commit Often
```bash
# Small, frequent commits are better than big ones
git commit -m "Fixed bug in ToneMatcher"
git commit -m "Added test for edge case"
```

### 3. Pull Before Push
```bash
# Always pull first to avoid conflicts
git pull
# Then make changes and push
git push
```

### 4. Use .gitignore (Already Set Up!)
The `.gitignore` file prevents committing:
- Build files
- IDE configs  
- Large audio files
- Temporary files

Already configured for you! ✅

---

## 🆘 Troubleshooting

### "Can't Install CMake on Work Laptop"
**That's OK!** You're only using work laptop for:
- Writing code
- Committing to git
- Pushing to GitHub

You'll build/run on personal laptop.

### "Repository Too Large"
Check what's being added:
```bash
git status
```

If you see `build/` or `JUCE/`, they shouldn't be there.
`.gitignore` should prevent this (already configured).

### "Permission Denied"
On GitHub, use Personal Access Token:
1. GitHub → Settings → Developer settings → Personal access tokens
2. Generate new token
3. Use as password when pushing

Or set up SSH keys (see GIT_SETUP.md)

### "Merge Conflict"
```bash
# Pull first
git pull

# Git will mark conflicts in files
# Edit files to resolve

# Then commit
git add .
git commit -m "Resolved merge conflict"
git push
```

---

## ✅ Quick Checklist

**On Work Laptop (Now):**
- [ ] `cd /Users/dan.obrien/code/guitar-sound-mimic`
- [ ] `git init`
- [ ] `git add .`
- [ ] `git commit -m "Initial commit"`
- [ ] Create GitHub repository
- [ ] `git remote add origin https://github.com/YOU/guitar-sound-mimic.git`
- [ ] `git push -u origin main`

**On Personal Laptop (Later):**
- [ ] Install CMake (`brew install cmake` on Mac)
- [ ] `git clone https://github.com/YOU/guitar-sound-mimic.git`
- [ ] `cd guitar-sound-mimic`
- [ ] `./setup.sh`
- [ ] `./run_tests.sh`
- [ ] `pip3 install demucs torch torchaudio`
- [ ] Run and enjoy! 🎸

---

## 📚 More Help

- **PUSH_TO_GIT.md** - Quick push guide
- **GIT_SETUP.md** - Comprehensive git guide
- **START_HERE.md** - Project overview

---

## 🎸 Summary

1. **Work:** Write code → Commit → Push to GitHub
2. **Home:** Pull from GitHub → Build → Run → Test with real guitar!
3. **Repeat:** Make changes anywhere → Push → Pull on other machine

**You're all set!** Ready to push? See **PUSH_TO_GIT.md** for commands.





