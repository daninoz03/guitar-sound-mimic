# Git Repository Setup Guide

## 📦 Pushing to Remote Repository

### Step 1: Initialize Git (if not already done)

```bash
cd /Users/dan.obrien/code/guitar-sound-mimic
git init
```

### Step 2: Add All Files

```bash
# Add all source files
git add .

# Check what will be committed
git status
```

**Expected files to be added:**
- ✅ Source code (`Source/`)
- ✅ Tests (`Tests/`)
- ✅ Documentation (`*.md`)
- ✅ Build files (`CMakeLists.txt`)
- ✅ Scripts (`setup.sh`, etc.)
- ✅ Tools (`tools/`)

**Files that should NOT be added (already in .gitignore):**
- ❌ `build/` directory
- ❌ `JUCE/` (will be cloned on other machine)
- ❌ Compiled binaries
- ❌ IDE files

### Step 3: Make First Commit

```bash
git commit -m "Initial commit: Guitar Sound Mimic v0.1.0

- C++ audio profiling and matching engine
- AI-powered guitar isolation (Demucs)
- 37 unit tests
- Complete documentation"
```

### Step 4: Create Remote Repository

**Option A: GitHub**
1. Go to https://github.com/new
2. Name: `guitar-sound-mimic`
3. Description: "AI-powered guitar tone profiling and matching"
4. Make it Public or Private
5. **Don't** initialize with README (we have one)
6. Click "Create repository"

**Option B: GitLab**
1. Go to https://gitlab.com/projects/new
2. Create blank project
3. Name: `guitar-sound-mimic`
4. Make it Public or Private

### Step 5: Connect to Remote

**GitHub:**
```bash
git remote add origin https://github.com/YOUR_USERNAME/guitar-sound-mimic.git
git branch -M main
git push -u origin main
```

**GitLab:**
```bash
git remote add origin https://gitlab.com/YOUR_USERNAME/guitar-sound-mimic.git
git branch -M main
git push -u origin main
```

---

## 🖥️ Cloning on Your Personal Laptop

### Step 1: Clone the Repository

```bash
cd ~/code  # or wherever you keep projects
git clone https://github.com/YOUR_USERNAME/guitar-sound-mimic.git
cd guitar-sound-mimic
```

### Step 2: Install Dependencies

**macOS:**
```bash
# Install Homebrew (if not installed)
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
sudo apt install libasound2-dev libjack-jackd2-dev \
    libfreetype6-dev libx11-dev libxinerama-dev libxrandr-dev \
    libxcursor-dev libxcomposite-dev mesa-common-dev \
    freeglut3-dev libcurl4-gnutls-dev
```

**Windows:**
- Install Visual Studio 2019+ with C++ workload
- Install CMake from https://cmake.org/download/
- Install Git from https://git-scm.com/

### Step 3: Build the App

```bash
./setup.sh
```

This will:
- Clone JUCE framework (~100MB)
- Build the application (~5 minutes)

### Step 4: Run Tests (Optional)

```bash
./run_tests.sh
```

### Step 5: Install Python Tools (For Guitar Separation)

```bash
# Install Python dependencies
pip3 install demucs torch torchaudio

# Or use requirements file
pip3 install -r tools/requirements.txt
```

### Step 6: Run the App!

```bash
# macOS
./build/GuitarSoundMimic_artefacts/Release/GuitarSoundMimic.app/Contents/MacOS/GuitarSoundMimic

# Or double-click
open ./build/GuitarSoundMimic_artefacts/Release/GuitarSoundMimic.app
```

---

## 📊 Repository Size Expectations

**What you're pushing:**
- Source code: ~50KB
- Tests: ~30KB
- Documentation: ~100KB
- Scripts: ~5KB
- **Total: ~200KB** (very small!)

**What won't be pushed (in .gitignore):**
- JUCE framework (100MB) - cloned on each machine
- Build artifacts (varies) - built on each machine
- IDE files - machine-specific

---

## 🔄 Keeping Repositories in Sync

### On Work Laptop (After Making Changes)

```bash
# Stage changes
git add .

# Commit
git commit -m "Added new feature"

# Push to remote
git push
```

### On Personal Laptop (Getting Latest Changes)

```bash
# Pull latest changes
git pull

# Rebuild if source code changed
cd build
cmake ..
cmake --build . --config Release
```

---

## 🌿 Branch Strategy

### For Experimentation
```bash
# Create feature branch
git checkout -b feature/new-tone-matching

# Make changes, commit
git add .
git commit -m "Improved tone matching algorithm"

# Push branch
git push -u origin feature/new-tone-matching

# Later, merge to main
git checkout main
git merge feature/new-tone-matching
git push
```

---

## 📝 Recommended Repository Settings

### GitHub Repository Settings

**Visibility:** Your choice (Public or Private)

**Description:**
```
AI-powered guitar tone profiling and matching system. 
Analyze any guitar recording to extract tone characteristics, 
then apply them to your guitar in real-time.
```

**Topics/Tags:**
- `audio`
- `music`
- `guitar`
- `dsp`
- `cpp`
- `juce`
- `machine-learning`
- `audio-processing`

**README Preview:**
GitHub will automatically show your `README.md` on the repository page.

---

## 🚨 Important: What NOT to Push

**Never commit these:**
- ❌ Build artifacts (`build/`)
- ❌ JUCE framework (`JUCE/`)
- ❌ Compiled binaries (`.exe`, `.app`, `.so`)
- ❌ IDE project files (`.xcodeproj`, `.vscode/`)
- ❌ Large audio files (test with small samples)
- ❌ API keys or credentials

**All excluded in `.gitignore` ✅**

---

## 📱 Mobile Git Apps (Optional)

If you want to browse code on mobile:

- **Working Copy** (iOS) - Full Git client
- **GitJournal** (iOS/Android) - Markdown-focused

---

## 🔐 Using SSH Instead of HTTPS (Optional)

For easier pushing without password:

```bash
# Generate SSH key
ssh-keygen -t ed25519 -C "your_email@example.com"

# Add to GitHub/GitLab
cat ~/.ssh/id_ed25519.pub
# Copy and paste to GitHub Settings → SSH Keys

# Use SSH URL
git remote set-url origin git@github.com:YOUR_USERNAME/guitar-sound-mimic.git
```

---

## ✅ Verification Checklist

Before pushing, verify:

- [ ] All source files added
- [ ] Tests added
- [ ] Documentation added
- [ ] Scripts are executable (`chmod +x *.sh`)
- [ ] `.gitignore` excludes build files
- [ ] No sensitive data in commits
- [ ] README.md looks good
- [ ] License file included

---

## 🎯 Quick Commands Summary

```bash
# On work laptop (first time)
git init
git add .
git commit -m "Initial commit"
git remote add origin https://github.com/YOU/guitar-sound-mimic.git
git push -u origin main

# On personal laptop (first time)
git clone https://github.com/YOU/guitar-sound-mimic.git
cd guitar-sound-mimic
./setup.sh

# Later updates (work laptop)
git add .
git commit -m "Update message"
git push

# Later updates (personal laptop)
git pull
```

---

## 📚 Resources

- [GitHub Docs](https://docs.github.com/)
- [Git Cheat Sheet](https://education.github.com/git-cheat-sheet-education.pdf)
- [Pro Git Book](https://git-scm.com/book/en/v2)

---

**You're all set to push and work from your personal laptop!** 🚀





