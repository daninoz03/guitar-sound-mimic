# Guitar Sound Mimic

A real-time guitar tone profiling and matching system built with JUCE. This application can analyze reference audio (songs, recordings) to extract tone characteristics and apply them to live guitar input.

Python tools can **isolate guitar from full mixes** (Demucs) so you can profile tones from commercial tracks, not only guitar-only sources. Details: [tools/README.md](tools/README.md).

## Quick start

From a fresh clone to profiling a tone is roughly **10 minutes** after downloads (longer the first time Demucs runs).

### 1. Build the app (one time)

**Prerequisites:** CMake 3.15+, C++17 toolchain (on macOS: Xcode Command Line Tools).

```bash
cd /path/to/guitar-sound-mimic
./setup.sh
```

This clones JUCE, configures CMake, and builds into `build/`. If it fails, install tools and retry:

```bash
xcode-select --install    # macOS: compiler toolchain
brew install cmake        # macOS: if CMake is missing
```

**Optional:** `./run_tests.sh` checks that unit tests pass.

### 2. Install Python tools (one time)

```bash
pip3 install demucs torch torchaudio soundfile numpy scipy
```

### 3. Isolate guitar and profile

```bash
python3 tools/separate_guitar.py your_song.mp3 guitar_only.wav
```

Then launch the app, play `guitar_only.wav` while profiling, and use **Apply Profile** for your guitar input. UI steps and tips are in [Usage](#usage) below; separation options and edge cases are in [tools/README.md](tools/README.md).

**macOS — run the app:**

```bash
open ./build/GuitarSoundMimic_artefacts/Release/GuitarSoundMimic.app
```

Or run the binary directly:

```bash
./build/GuitarSoundMimic_artefacts/Release/GuitarSoundMimic.app/Contents/MacOS/GuitarSoundMimic
```

On Windows and Linux, use the executable under `build/GuitarSoundMimic_artefacts/Release/` for your platform (see [Building from source](#building-from-source)).

### Quick troubleshooting

| Issue | What to try |
|-------|-------------|
| `cmake` not found | Install CMake (e.g. `brew install cmake` on macOS). |
| `demucs` / `torch` errors | Re-run the `pip3 install` line above. |
| No audio I/O | **Options → Audio Settings**; reconnect the interface; try a smaller buffer size for latency. |
| Build errors | Xcode/CLI tools installed; JUCE cloned; see [BUILDING.md](BUILDING.md). |

### Build without `setup.sh`

```bash
git clone --depth 1 https://github.com/juce-framework/JUCE.git
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

Use [Building from source](#building-from-source) for platform-specific paths and options.

## Features

- **🎯 AI Guitar Isolation**: Extract guitar tracks from mixed songs using Demucs deep learning
- **Tone Profiling**: Analyze any audio source to extract frequency response, harmonic content, and dynamics
- **Real-time Matching**: Apply captured tone profiles to live guitar input with low latency
- **Frequency Analysis**: Advanced FFT-based analysis for accurate tone characterization
- **Dynamic Processing**: Automatically matches compression and saturation characteristics
- **Multi-band EQ**: 10-band parametric EQ for precise frequency response matching

## How It Works

1. **Profile Mode**: Play reference audio (e.g., your favorite guitar recording) through the app. It analyzes:
   - Frequency response across the spectrum
   - Harmonic content and overtones
   - Dynamic characteristics (compression ratio, threshold)
   - EQ curve

2. **Apply Mode**: Play your guitar through the app with the captured profile active. The app applies:
   - Multi-band EQ to match frequency response
   - Compression to match dynamics
   - Real-time DSP processing

## Testing

The project includes comprehensive unit tests for all core components:

```bash
# Build and run tests
cd build
cmake --build . --target GuitarSoundMimicTests
./GuitarSoundMimicTests_artefacts/Debug/GuitarSoundMimicTests
```

**Test Coverage:**
- ✅ ToneProfiler (audio analysis)
- ✅ ToneMatcher (real-time processing)
- ✅ FrequencyAnalyzer (DSP algorithms)
- ✅ ToneProfile (data structures)

See [Tests/README.md](Tests/README.md) for detailed testing documentation.

## Building from Source

### Prerequisites

- CMake 3.15 or higher
- C++17 compatible compiler (Xcode on macOS, Visual Studio on Windows, GCC/Clang on Linux)
- JUCE framework (will be setup in next step)

### Setup Instructions

1. **Clone JUCE framework**:
```bash
cd /path/to/guitar-sound-mimic
git clone https://github.com/juce-framework/JUCE.git
```

2. **Create build directory and run CMake**:
```bash
mkdir build
cd build
cmake ..
```

3. **Build the application**:
```bash
cmake --build . --config Release
```

4. **Run the application**:
   - **macOS**: `./build/GuitarSoundMimic_artefacts/Release/GuitarSoundMimic.app/Contents/MacOS/GuitarSoundMimic`
   - **Windows**: `build\GuitarSoundMimic_artefacts\Release\GuitarSoundMimic.exe`
   - **Linux**: `./build/GuitarSoundMimic_artefacts/Release/GuitarSoundMimic`

## Usage

### Recommended Workflow: Profile from Songs 🎵

Since most reference audio contains multiple instruments, use the AI separation tool first:

#### Step 1: Extract Guitar from a Song
```bash
# Install Python dependencies (one time only)
pip install demucs torch torchaudio

# Extract guitar from your favorite song
python tools/separate_guitar.py "favorite_song.mp3" "isolated_guitar.wav"
```

This takes about 30-60 seconds and produces a clean guitar-only track.

#### Step 2: Profile the Isolated Guitar
1. Launch Guitar Sound Mimic app
2. Play `isolated_guitar.wav` through your computer
3. Click **"Start Profiling"** in the app
4. Let it analyze for 5-10 seconds
5. Click **"Stop Profiling"**

#### Step 3: Apply to Your Guitar
1. Click **"Apply Profile"**
2. Plug in your guitar
3. Play and hear the profiled tone applied to your playing
4. Use **"Bypass"** to compare

### Alternative: Direct Recording Method

If you already have guitar-only audio (stems, isolated tracks, direct recordings):

1. Launch the application
2. Go to Options → Audio Settings
3. Select your audio interface
4. Play the guitar-only audio
5. Click **"Start Profiling"** → let it run 5-10 seconds → **"Stop Profiling"**
6. Click **"Apply Profile"** and play your guitar

### Setting Up Audio

1. Go to Options (menu bar) → Audio Settings
2. Select your audio interface:
   - **Input**: Guitar input (DI or audio interface)
   - **Output**: Speakers or headphones
4. Set buffer size (128 or 256 samples recommended for low latency)

## Technical Details

### Audio Processing Pipeline

```
Input → ToneProfiler (Analysis) → ToneProfile (Data)
Input → ToneMatcher (Processing) → Output

ToneMatcher Pipeline:
Input → Multi-band EQ → Compressor → Output
```

### Key Classes

- **ToneProfiler**: Analyzes audio to create tone profiles
  - FFT-based frequency analysis (4096-point FFT)
  - Harmonic content extraction
  - Dynamic range analysis

- **ToneMatcher**: Applies tone profiles in real-time
  - 10-band parametric EQ
  - Dynamic range compressor
  - Low-latency DSP processing

- **FrequencyAnalyzer**: Advanced frequency domain analysis
  - Fundamental frequency detection
  - Harmonic series extraction
  - Spectrum analysis

## Future Development

### Planned Features

- [ ] Save/Load tone profiles to disk
- [ ] Profile library and management
- [ ] VST3/AU/AAX plugin versions for DAW integration
- [ ] More sophisticated machine learning-based tone matching
- [ ] Cab simulator with impulse response support
- [ ] Effect detection (reverb, delay, modulation)
- [ ] A/B comparison mode
- [ ] Preset browser

### Roadmap to DAW Plugin

This application is designed to eventually become a DAW plugin:

1. **Phase 1** (Current): Standalone application with basic profiling
2. **Phase 2**: Enhanced analysis and better matching algorithms
3. **Phase 3**: Add plugin wrapper using JUCE's plugin architecture
4. **Phase 4**: VST3, AU, and AAX builds
5. **Phase 5**: Profile marketplace/sharing platform

## Project Structure

```
guitar-sound-mimic/
├── CMakeLists.txt          # Build configuration
├── JUCE/                   # JUCE framework (git clone)
├── Source/
│   ├── Main.cpp           # Application entry point
│   ├── MainComponent.h/cpp # Main UI and audio callback
│   ├── Audio/
│   │   ├── AudioEngine.h   # Shared audio types (ToneProfile)
│   │   ├── ToneProfiler.h/cpp  # Audio analysis engine
│   │   └── ToneMatcher.h/cpp   # Real-time processing engine
│   └── DSP/
│       └── FrequencyAnalyzer.h/cpp  # FFT and harmonic analysis
└── README.md
```

## System Requirements

- **macOS**: 10.13 or higher
- **Windows**: Windows 10 or higher
- **Linux**: Modern distribution with ALSA/JACK support
- **RAM**: 4GB minimum
- **Audio Interface**: Recommended for low-latency guitar input

## Troubleshooting

### High Latency
- Reduce buffer size in audio settings (try 128 or 64 samples)
- Use a dedicated audio interface with ASIO (Windows) or Core Audio (macOS) drivers

### No Audio Input/Output
- Check audio device settings in the app
- Ensure your audio interface is properly connected
- Verify input/output channels are correctly selected

### Profiling Fails
- Ensure audio is actually playing during profiling
- Play audio for at least 3 seconds
- Check input levels (should show activity on input meter)

## Contributing

This is a personal project, but suggestions and contributions are welcome! Areas that need work:
- Better tone matching algorithms
- Machine learning for amp/effect detection
- UI/UX improvements
- Additional DSP effects

## License

This project uses JUCE framework which has its own licensing terms. Please review JUCE licensing at https://juce.com/juce-7-licence

## Credits

Built with [JUCE](https://juce.com/) - the premier framework for audio application development.

Inspired by amp profilers like Kemper, Quad Cortex, and Neural DSP plugins.

