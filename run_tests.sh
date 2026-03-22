#!/bin/bash

# Script to build and run unit tests

set -e

echo "🧪 Guitar Sound Mimic - Unit Tests"
echo "=================================="
echo ""

# Check if build directory exists
if [ ! -d "build" ]; then
    echo "❌ Build directory not found. Please run setup.sh first."
    exit 1
fi

cd build

echo "🔨 Building tests..."
cmake --build . --target GuitarSoundMimicTests --config Debug

echo ""
echo "🧪 Running tests..."
echo ""

# Run tests and capture exit code
if ./GuitarSoundMimicTests_artefacts/Debug/GuitarSoundMimicTests; then
    echo ""
    echo "✅ All tests passed!"
    exit 0
else
    echo ""
    echo "❌ Some tests failed!"
    exit 1
fi





