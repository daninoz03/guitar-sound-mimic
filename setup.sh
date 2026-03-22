#!/bin/bash

# Setup script for Guitar Sound Mimic
# This script will clone JUCE and set up the build environment

set -e

echo "🎸 Guitar Sound Mimic - Setup Script"
echo "===================================="
echo ""

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ]; then
    echo "❌ Error: CMakeLists.txt not found. Please run this script from the project root."
    exit 1
fi

# Check for CMake
if ! command -v cmake &> /dev/null; then
    echo "❌ Error: CMake is not installed."
    echo ""
    echo "Please install CMake:"
    echo "  macOS:   brew install cmake"
    echo "  Linux:   sudo apt install cmake"
    echo "  Windows: Download from https://cmake.org/download/"
    exit 1
fi

echo "✅ CMake found: $(cmake --version | head -n1)"
echo ""

# Clone JUCE if not already present
if [ ! -d "JUCE" ]; then
    echo "📦 Cloning JUCE framework..."
    git clone --depth 1 https://github.com/juce-framework/JUCE.git
    echo "✅ JUCE cloned successfully"
else
    echo "✅ JUCE already exists"
fi
echo ""

# Create build directory
if [ ! -d "build" ]; then
    echo "📁 Creating build directory..."
    mkdir build
    echo "✅ Build directory created"
else
    echo "✅ Build directory already exists"
fi
echo ""

# Run CMake
echo "⚙️  Configuring CMake..."
cd build
cmake ..
echo "✅ CMake configuration complete"
echo ""

# Build
echo "🔨 Building project..."
echo "   (This may take a few minutes...)"
cmake --build . --config Release
echo "✅ Build complete!"
echo ""

# Success message
echo "=========================================="
echo "✨ Setup complete! ✨"
echo ""
echo "To run the application:"
if [[ "$OSTYPE" == "darwin"* ]]; then
    echo "  ./build/GuitarSoundMimic_artefacts/Release/GuitarSoundMimic.app/Contents/MacOS/GuitarSoundMimic"
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    echo "  ./build/GuitarSoundMimic_artefacts/Release/GuitarSoundMimic"
else
    echo "  .\\build\\GuitarSoundMimic_artefacts\\Release\\GuitarSoundMimic.exe"
fi
echo ""
echo "See README.md for usage instructions."
echo "=========================================="





