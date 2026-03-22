# Quick Git Push Guide

## 🚀 Push to Remote (Work Laptop)

### 1. Initialize & Commit

```bash
cd /Users/dan.obrien/code/guitar-sound-mimic

# Initialize git (if not done)
git init

# Add all files
git add .

# Check what's being added
git status

# Commit
git commit -m "Initial commit: Guitar Sound Mimic v0.1.0

- AI-powered guitar tone profiling system
- Real-time tone matching with 10-band EQ
- Guitar isolation using Demucs deep learning
- 37 comprehensive unit tests
- Full documentation and setup scripts"
```

### 2. Create GitHub Repository

1. Go to: **https://github.com/new**
2. Repository name: `guitar-sound-mimic`
3. Description: `AI-powered guitar tone profiling and matching`
4. Choose Public or Private
5. **Don't** initialize with README
6. Click **Create repository**

### 3. Push to GitHub

```bash
# Add remote (replace YOUR_USERNAME)
git remote add origin https://github.com/YOUR_USERNAME/guitar-sound-mimic.git

# Push
git branch -M main
git push -u origin main
```

**Done!** Your code is now on GitHub.

---

## 💻 Clone on Personal Laptop

### 1. Clone Repository

```bash
cd ~/code  # or wherever you keep projects

# Clone (replace YOUR_USERNAME)
git clone https://github.com/YOUR_USERNAME/guitar-sound-mimic.git

cd guitar-sound-mimic
```

### 2. Install Dependencies

**macOS:**
```bash
# Install Homebrew if needed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install CMake
brew install cmake

# Install Xcode Command Line Tools
xcode-select --install
```

**Linux:**
```bash
sudo apt install build-essential cmake git
sudo apt install libasound2-dev libjack-jackd2-dev \
    libfreetype6-dev libx11-dev libxinerama-dev libxrandr-dev
```

### 3. Build & Run

```bash
# Build app (downloads JUCE, compiles everything)
./setup.sh

# Run tests
./run_tests.sh

# Install Python tools
pip3 install demucs torch torchaudio

# Run app
open ./build/GuitarSoundMimic_artefacts/Release/GuitarSoundMimic.app
```

**That's it!** You're running on your personal laptop now.

---

## 🔄 Sync Changes Between Machines

### Work Laptop → GitHub
```bash
# Make changes...
git add .
git commit -m "Add new feature"
git push
```

### Personal Laptop → Get Updates
```bash
git pull
./setup.sh  # Only if source code changed
```

### Personal Laptop → GitHub
```bash
# Make changes...
git add .
git commit -m "Improve algorithm"
git push
```

### Work Laptop → Get Updates
```bash
git pull
# (Can't build on work laptop due to restrictions, but code is synced)
```

---

## 📦 What Gets Pushed

**✅ Included (tracked by git):**
- Source code (`Source/`)
- Tests (`Tests/`)
- Documentation (`*.md`)
- Build scripts (`CMakeLists.txt`, `setup.sh`)
- Python tools (`tools/`)

**❌ Excluded (.gitignore):**
- `build/` directory (built on each machine)
- `JUCE/` framework (cloned on each machine)
- Compiled binaries
- IDE project files

**Repository size: ~200KB** (very small!)

---

## 🎯 Quick Reference

```bash
# First time setup (work laptop)
git init
git add .
git commit -m "Initial commit"
git remote add origin https://github.com/YOU/guitar-sound-mimic.git
git push -u origin main

# First time setup (personal laptop)
git clone https://github.com/YOU/guitar-sound-mimic.git
cd guitar-sound-mimic
./setup.sh

# Regular workflow
git pull              # Get latest changes
# ... make changes ...
git add .             # Stage changes
git commit -m "msg"   # Commit
git push              # Push to GitHub
```

---

## ✅ Verification

After pushing, check:
- [ ] Repository appears on GitHub
- [ ] README.md displays correctly
- [ ] All source files are there
- [ ] No `build/` or `JUCE/` directories
- [ ] Can clone on another machine

---

## 🆘 Common Issues

**"Permission denied"**
→ Set up SSH key or use HTTPS with token

**"Repository not found"**
→ Check repository name and URL

**"Can't push"**
→ Make sure remote is added: `git remote -v`

**"Merge conflict"**
→ Pull first: `git pull`, resolve conflicts, then push

---

## 📚 More Details

See **GIT_SETUP.md** for comprehensive guide including:
- SSH setup
- Branch strategies
- Repository settings
- Troubleshooting

---

**Ready to push?** Just run:

```bash
git init && git add . && git commit -m "Initial commit"
```

Then create GitHub repo and push! 🚀





