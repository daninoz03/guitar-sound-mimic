# Building Guitar Sound Mimic

Detailed build instructions for all platforms.

## Quick Start

```bash
# 1. Clone JUCE
git clone https://github.com/juce-framework/JUCE.git

# 2. Configure with CMake
mkdir build && cd build
cmake ..

# 3. Build
cmake --build . --config Release
```

## Platform-Specific Instructions

### macOS

#### Requirements
- Xcode 12 or later
- macOS 10.13 or later
- CMake 3.15+

#### Install CMake (if not installed)
```bash
brew install cmake
```

#### Build Steps
```bash
cd /Users/dan.obrien/code/guitar-sound-mimic
git clone https://github.com/juce-framework/JUCE.git

mkdir build && cd build
cmake .. -G "Xcode"  # Generate Xcode project
# OR
cmake ..             # Use Unix Makefiles

# Build with Xcode
open GuitarSoundMimic.xcodeproj
# Then build in Xcode

# OR build with command line
cmake --build . --config Release

# Run
./GuitarSoundMimic_artefacts/Release/GuitarSoundMimic.app/Contents/MacOS/GuitarSoundMimic
```

#### Code Signing (for Distribution)
```bash
# In CMakeLists.txt, add:
# set_target_properties(GuitarSoundMimic PROPERTIES
#     XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "Developer ID Application"
#     XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "YOUR_TEAM_ID")
```

### Windows

#### Requirements
- Visual Studio 2019 or later (Community Edition works)
- Windows 10 or later
- CMake 3.15+

#### Install CMake
Download from https://cmake.org/download/ or use chocolatey:
```powershell
choco install cmake
```

#### Build Steps
```powershell
cd C:\Users\YourName\code\guitar-sound-mimic
git clone https://github.com/juce-framework/JUCE.git

mkdir build
cd build
cmake .. -G "Visual Studio 16 2019"  # Or newer version

# Build
cmake --build . --config Release

# Run
.\GuitarSoundMimic_artefacts\Release\GuitarSoundMimic.exe
```

#### ASIO Support (Optional, for low latency)
1. Download ASIO SDK from Steinberg
2. Place in `JUCE/modules/juce_audio_devices/native/asiosdk`
3. Rebuild

### Linux

#### Requirements
- GCC 7+ or Clang 5+
- CMake 3.15+
- Development libraries

#### Install Dependencies

**Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install build-essential cmake git
sudo apt install libasound2-dev libjack-jackd2-dev \
    libfreetype6-dev libx11-dev libxinerama-dev libxrandr-dev \
    libxcursor-dev libxcomposite-dev mesa-common-dev \
    freeglut3-dev libcurl4-gnutls-dev
```

**Fedora/RHEL:**
```bash
sudo dnf install gcc-c++ cmake git
sudo dnf install alsa-lib-devel jack-audio-connection-kit-devel \
    freetype-devel libX11-devel libXinerama-devel libXrandr-devel \
    libXcursor-devel mesa-libGL-devel libcurl-devel
```

**Arch Linux:**
```bash
sudo pacman -S base-devel cmake git
sudo pacman -S alsa-lib jack freetype2 libx11 libxinerama \
    libxrandr libxcursor mesa libglvnd curl
```

#### Build Steps
```bash
cd ~/code/guitar-sound-mimic
git clone https://github.com/juce-framework/JUCE.git

mkdir build && cd build
cmake ..
cmake --build . --config Release -j$(nproc)

# Run
./GuitarSoundMimic_artefacts/Release/GuitarSoundMimic
```

## Build Configurations

### Debug Build
```bash
mkdir build-debug && cd build-debug
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .
```

Debug builds include:
- Debugging symbols
- Assertions enabled
- No optimization
- Larger binary size

### Release Build
```bash
mkdir build-release && cd build-release
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

Release builds include:
- Full optimization (-O3)
- No debug symbols
- Smaller binary size
- Best performance

### RelWithDebInfo Build
```bash
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build .
```

Useful for profiling: optimized but with debug symbols.

## Customizing the Build

### Change Company Name
Edit `CMakeLists.txt`:
```cmake
juce_add_gui_app(GuitarSoundMimic
    COMPANY_NAME "YourCompanyName"  # Change this
    PRODUCT_NAME "Guitar Sound Mimic"
    VERSION 0.1.0)
```

### Add Custom Compiler Flags
```cmake
target_compile_options(GuitarSoundMimic PRIVATE
    -Wall -Wextra -Wpedantic  # GCC/Clang
    # OR
    /W4                        # MSVC
)
```

### Link Additional Libraries
```cmake
target_link_libraries(GuitarSoundMimic
    PRIVATE
        juce::juce_audio_basics
        # ... existing libraries ...
        your_custom_library
)
```

## Troubleshooting

### "JUCE not found"
Make sure JUCE is cloned in the project root:
```bash
ls JUCE/modules  # Should list juce_audio_basics, etc.
```

### CMake version too old
```bash
# macOS
brew upgrade cmake

# Ubuntu
sudo snap install cmake --classic

# Or build from source
wget https://github.com/Kitware/CMake/releases/download/v3.27.0/cmake-3.27.0.tar.gz
tar -xzf cmake-3.27.0.tar.gz
cd cmake-3.27.0
./bootstrap && make && sudo make install
```

### Linker errors on Linux
Install missing dependencies:
```bash
# Check error message for missing library
# Example: "cannot find -lfreetype"
sudo apt-cache search libfreetype
sudo apt install libfreetype6-dev
```

### macOS: "Developer tools not found"
```bash
xcode-select --install
```

### Windows: "No CMAKE_CXX_COMPILER found"
Install Visual Studio with C++ workload, or install Build Tools for Visual Studio.

## Build Targets

```bash
# Build only the main target
cmake --build . --target GuitarSoundMimic

# Clean build
cmake --build . --target clean

# Rebuild from scratch
rm -rf build && mkdir build && cd build && cmake .. && cmake --build .
```

## IDE Integration

### Visual Studio Code
Install extensions:
- C/C++
- CMake Tools

Open folder and use CMake Tools to configure and build.

### CLion
Open project, CLion will automatically detect CMakeLists.txt and configure.

### Xcode
```bash
cmake .. -G Xcode
open GuitarSoundMimic.xcodeproj
```

## Performance Optimization

### Enable LTO (Link Time Optimization)
JUCE already includes `juce_recommended_lto_flags` for Release builds.

### Platform-Specific Optimizations
```cmake
if(APPLE)
    target_compile_options(GuitarSoundMimic PRIVATE -march=native)
elseif(UNIX)
    target_compile_options(GuitarSoundMimic PRIVATE -march=native)
elseif(MSVC)
    target_compile_options(GuitarSoundMimic PRIVATE /arch:AVX2)
endif()
```

## Next Steps

After building successfully:
1. Read the main README.md for usage instructions
2. Configure your audio interface
3. Start profiling guitar tones!





