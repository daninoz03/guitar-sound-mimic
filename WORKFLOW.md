# Complete Workflow Diagram

## The Complete Pipeline

```
┌─────────────────────────────────────────────────────────────────────┐
│                                                                       │
│                    GUITAR SOUND MIMIC WORKFLOW                        │
│                                                                       │
└─────────────────────────────────────────────────────────────────────┘


PHASE 1: PREPARATION (One Time Setup)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

┌─────────────┐      ┌─────────────┐      ┌─────────────┐
│   Install   │  →   │   Build     │  →   │  Install    │
│   CMake     │      │   C++ App   │      │  Python AI  │
└─────────────┘      └─────────────┘      └─────────────┘
  brew install         ./setup.sh          pip install
     cmake                                    demucs


PHASE 2: GUITAR ISOLATION (Per Song)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

┌─────────────────────┐
│   Original Song     │
│  (Mixed Audio)      │
│                     │
│  🎸 Guitar          │
│  🥁 Drums           │
│  🎹 Bass            │
│  🎤 Vocals          │
└──────────┬──────────┘
           │
           ▼
┌─────────────────────┐
│  Demucs AI Model    │
│  (Deep Learning)    │
│                     │
│  Source Separation  │
└──────────┬──────────┘
           │
           ├──────────┬──────────┬──────────┐
           ▼          ▼          ▼          ▼
      ┌────────┐ ┌────────┐ ┌────────┐ ┌────────┐
      │ Drums  │ │  Bass  │ │Guitar/ │ │Vocals  │
      │        │ │        │ │ Other  │ │        │
      └────────┘ └────────┘ └───┬────┘ └────────┘
                                 │
                                 ▼
                        ┌─────────────────┐
                        │  guitar_only    │
                        │     .wav        │
                        └─────────────────┘
                              ✅ Ready!


PHASE 3: TONE PROFILING (In App)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

┌─────────────────┐
│  guitar_only    │
│     .wav        │
└────────┬────────┘
         │
         │ Play through computer
         ▼
┌──────────────────────────────┐
│  Guitar Sound Mimic App      │
│                              │
│  [Start Profiling] ←── Click │
└────────┬─────────────────────┘
         │
         │ Analyze 5-10 seconds
         ▼
┌──────────────────────────────┐
│     ToneProfiler             │
│  ┌────────────────────────┐  │
│  │ • FFT Analysis         │  │
│  │ • Frequency Response   │  │
│  │ • Harmonic Content     │  │
│  │ • Dynamic Range        │  │
│  │ • Compression Ratio    │  │
│  └────────────────────────┘  │
└────────┬─────────────────────┘
         │
         ▼
┌──────────────────────────────┐
│      ToneProfile             │
│  ┌────────────────────────┐  │
│  │ Frequency Response:    │  │
│  │ [graph data]           │  │
│  │                        │  │
│  │ EQ Bands: 10           │  │
│  │ Compression: 3.2:1     │  │
│  │ Harmonics: [...]       │  │
│  └────────────────────────┘  │
└──────────────────────────────┘
         ✅ Profile Generated!


PHASE 4: REAL-TIME MATCHING (Live Guitar)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

┌──────────────────┐
│   Your Guitar    │
│                  │
│    🎸            │
└────────┬─────────┘
         │
         │ Audio Interface
         ▼
┌──────────────────────────────┐
│  Guitar Sound Mimic App      │
│                              │
│  [Apply Profile] ←── Click   │
└────────┬─────────────────────┘
         │
         │ Real-time processing
         ▼
┌──────────────────────────────┐
│      ToneMatcher             │
│  ┌────────────────────────┐  │
│  │ 10-Band EQ             │  │
│  │       ↓                │  │
│  │ Compressor             │  │
│  │       ↓                │  │
│  │ Gain Adjustment        │  │
│  └────────────────────────┘  │
│                              │
│  Uses ToneProfile data       │
└────────┬─────────────────────┘
         │
         │ Output (< 10ms latency)
         ▼
┌──────────────────┐
│   Speakers /     │
│   Headphones     │
│                  │
│    🔊            │
└──────────────────┘
    Sound like
  your favorite
   guitarist! 🎉


COMPARISON MODE
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

┌────────────────┐     [Apply Profile]     ┌──────────────────┐
│  Your Guitar   │  ──────────────────→    │  Processed       │
│  (Original)    │                          │  (w/ Profile)    │
└────────────────┘                          └──────────────────┘
                                                     ↓
                          [Bypass]                 Uses tone from
                             ↓                    reference song!
                      Original sound
                      
         Toggle between them to compare!
```

## File Flow

```
Input Files:
┌──────────────────────────────────┐
│ favorite_song.mp3                │  ← Your favorite guitarist
│ gilmour_solo.mp3                 │  ← David Gilmour
│ hendrix_little_wing.mp3          │  ← Jimi Hendrix
└──────────────────────────────────┘
                 ↓
         [separate_guitar.py]
                 ↓
Isolated Tracks:
┌──────────────────────────────────┐
│ favorite_guitar.wav              │  ← Guitar only
│ gilmour_guitar.wav               │  ← Guitar only
│ hendrix_guitar.wav               │  ← Guitar only
└──────────────────────────────────┘
                 ↓
      [Guitar Sound Mimic App]
                 ↓
      [Profile + Apply Mode]
                 ↓
        Your guitar sounds
        like the reference! 🎸
```

## Technology Stack

```
┌─────────────────────────────────────────────────────────┐
│                                                           │
│  LAYER 1: User Interface (JUCE Framework)                │
│  ┌─────────────────────────────────────────────────┐    │
│  │ • Buttons (Profile, Apply, Bypass)              │    │
│  │ • Level Meters (Input/Output)                   │    │
│  │ • Audio Device Settings                         │    │
│  └─────────────────────────────────────────────────┘    │
│                          ↕                               │
│  LAYER 2: Audio Processing (C++)                         │
│  ┌─────────────────────────────────────────────────┐    │
│  │ ToneProfiler:          ToneMatcher:             │    │
│  │ • FFT (4096-point)     • 10-band EQ             │    │
│  │ • Harmonic Analysis    • Compressor             │    │
│  │ • Dynamic Analysis     • Real-time DSP          │    │
│  └─────────────────────────────────────────────────┘    │
│                          ↕                               │
│  LAYER 3: DSP Algorithms                                 │
│  ┌─────────────────────────────────────────────────┐    │
│  │ • JUCE DSP Module                               │    │
│  │ • IIR Filters                                   │    │
│  │ • FFT Engine                                    │    │
│  │ • Windowing Functions                           │    │
│  └─────────────────────────────────────────────────┘    │
│                          ↕                               │
│  LAYER 4: Audio I/O (JUCE)                              │
│  ┌─────────────────────────────────────────────────┐    │
│  │ • CoreAudio (macOS)                             │    │
│  │ • ASIO (Windows)                                │    │
│  │ • ALSA/JACK (Linux)                             │    │
│  └─────────────────────────────────────────────────┘    │
│                                                           │
└─────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────┐
│                                                           │
│  EXTERNAL TOOL: Guitar Isolation (Python)                │
│  ┌─────────────────────────────────────────────────┐    │
│  │ • Demucs AI Model (Meta/Facebook)               │    │
│  │ • PyTorch (Deep Learning)                       │    │
│  │ • TorchAudio (Audio I/O)                        │    │
│  └─────────────────────────────────────────────────┘    │
│                                                           │
└─────────────────────────────────────────────────────────┘
```

## Data Structures

```
ToneProfile
├── frequencyResponse: [f1, f2, ..., fN]    // Magnitude at each freq
├── frequencies: [20Hz, 40Hz, ..., 20kHz]   // Frequency labels
├── harmonicContent: [h1, h2, ..., h8]      // Harmonic levels
├── compressionRatio: 3.2                    // Dynamic compression
├── compressionThreshold: 0.5                // Compression threshold
├── eqBands: [                               // Parametric EQ bands
│   {freq: 80Hz, gain: +2dB, Q: 1.0},
│   {freq: 150Hz, gain: -1dB, Q: 1.0},
│   ...
│   ]
├── sampleRate: 44100                        // Hz
├── totalSamplesAnalyzed: 220500            // ~5 seconds
└── profileName: "Gilmour Solo"             // Optional
```

## Signal Flow (Real-time Processing)

```
Guitar → Interface → App Input
                        ↓
                  [Input Gain]
                        ↓
                  [EQ Band 1: 80Hz]
                        ↓
                  [EQ Band 2: 150Hz]
                        ↓
                       ...
                        ↓
                  [EQ Band 10: 16kHz]
                        ↓
                  [Compressor]
                  (ratio: 3.2:1)
                        ↓
                  [Output Gain]
                        ↓
              App Output → Interface → Speakers
                                           ↓
                                    Sounds like
                                   reference tone!
```

## Performance Characteristics

```
Component              | Performance
-----------------------|---------------------------
Guitar Separation      | 30-60 sec per song (CPU)
                      | 5-10 sec per song (GPU)
-----------------------|---------------------------
Profile Generation     | 5-10 seconds minimum
                      | Real-time analysis
-----------------------|---------------------------
Real-time Processing   | < 10ms latency
                      | 128-256 sample buffer
                      | 44.1kHz or 48kHz
-----------------------|---------------------------
CPU Usage (playback)   | ~5-10% (single core)
-----------------------|---------------------------
Memory Usage          | ~50MB (app)
                      | ~500MB (Demucs model)
```

---

## Summary

1. **Separate** guitar from mixed audio (AI)
2. **Profile** the isolated guitar (FFT + analysis)
3. **Match** your guitar to the profile (real-time EQ + compression)
4. **Sound** like your favorite guitarists!

🎸 **That's the complete workflow!** 🎸





