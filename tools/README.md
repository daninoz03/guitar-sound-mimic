# Guitar Source Separation Tools

This directory contains tools for extracting guitar tracks from mixed audio using AI.

## Why Source Separation?

Most reference audio (songs, recordings) contains multiple instruments. To accurately profile just the guitar tone, we need to isolate it from the mix first.

## Installation

### Option 1: Full AI Model (Recommended)
```bash
pip install demucs torch torchaudio soundfile numpy scipy
```

### Option 2: Quick Install
```bash
pip install -r tools/requirements.txt
```

### Option 3: Simple Method Only
```bash
pip install soundfile numpy scipy
```

## Usage

### Basic Usage
```bash
# Extract guitar from a song
python tools/separate_guitar.py song.mp3 guitar_only.wav

# Then load guitar_only.wav in the Guitar Sound Mimic app
```

### Advanced Options
```bash
# Use simple frequency-based method (faster but lower quality)
python tools/separate_guitar.py -m simple song.mp3 guitar.wav

# Process multiple files
for file in songs/*.mp3; do
    python tools/separate_guitar.py "$file" "separated/$(basename "$file" .mp3)_guitar.wav"
done
```

## How It Works

### Method 1: Demucs (AI-Based) ⭐ Recommended
- Uses state-of-the-art deep learning model from Meta/Facebook
- Separates audio into: drums, bass, guitar/other, vocals
- Very high quality separation
- Takes 30-60 seconds per song
- Requires ~500MB model download on first run

**Quality**: Excellent - can cleanly separate guitar even from complex mixes

### Method 2: Simple (Frequency-Based)
- Applies bandpass filter (80Hz - 5kHz)
- Fast but much lower quality
- Doesn't remove other instruments, just filters frequencies
- Use only if Demucs isn't available

**Quality**: Basic - may include bass, keyboards, and other mid-range instruments

## Examples

### Example 1: Profile a Guitar Solo
```bash
# Download a song with a guitar solo
# Extract just the guitar
python tools/separate_guitar.py "epic_guitar_solo.mp3" "solo_guitar.wav"

# Open Guitar Sound Mimic app
# Load solo_guitar.wav and profile it
```

### Example 2: Batch Processing
```bash
mkdir separated_guitars

# Process all songs in a directory
for song in my_favorite_songs/*.mp3; do
    output="separated_guitars/$(basename "$song" .mp3)_guitar.wav"
    python tools/separate_guitar.py "$song" "$output"
done
```

### Example 3: Profile Specific Guitarists
```bash
# Extract David Gilmour's tone from Pink Floyd
python tools/separate_guitar.py "comfortably_numb.mp3" "gilmour_solo.wav"

# Extract Hendrix's tone
python tools/separate_guitar.py "little_wing.mp3" "hendrix_guitar.wav"

# Extract Slash's tone
python tools/separate_guitar.py "sweet_child.mp3" "slash_solo.wav"
```

## Performance Tips

### GPU Acceleration (Optional)
If you have an NVIDIA GPU, Demucs can run much faster:

```bash
# Install CUDA-enabled PyTorch
pip install torch torchaudio --index-url https://download.pytorch.org/whl/cu118

# The script will automatically use GPU if available
```

### Processing Time
- **Demucs (CPU)**: ~30-60 seconds per song
- **Demucs (GPU)**: ~5-10 seconds per song
- **Simple method**: ~1-2 seconds per song

## Workflow

```
┌─────────────────┐
│   Song File     │
│  (Mixed Audio)  │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  separate_      │
│  guitar.py      │
│  (Demucs AI)    │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  Guitar Track   │
│  (Isolated)     │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ Guitar Sound    │
│ Mimic App       │
│ (Profile Tone)  │
└─────────────────┘
```

## Troubleshooting

### "demucs not found"
```bash
pip install demucs
```

### "torch not found"
```bash
pip install torch torchaudio
```

### Out of Memory
- Use CPU instead of GPU
- Process shorter clips
- Close other applications

### Poor Separation Quality
- Demucs works best with well-mixed commercial recordings
- Simple method is not suitable for complex mixes
- Some guitar tones (especially heavily processed) may be harder to separate

## Integration with Main App

### Current: Manual Workflow
1. Run `separate_guitar.py` to extract guitar
2. Load the resulting WAV file in the app
3. Profile the isolated guitar

### Future: Automatic Integration
Planned features:
- [ ] "Import Song" button in app
- [ ] Automatic source separation
- [ ] Progress bar for separation
- [ ] C++ integration using LibTorch
- [ ] Real-time separation (for advanced use cases)

## Advanced: Custom Models

You can use different Demucs models:

```python
# In separate_guitar.py, line 44, change:
model = get_model('htdemucs')      # High quality (default)
# to:
model = get_model('htdemucs_ft')   # Fine-tuned version
model = get_model('mdx_extra')     # Alternative model
```

## Credits

- **Demucs**: Developed by Meta/Facebook Research
  - Paper: https://arxiv.org/abs/2111.03600
  - GitHub: https://github.com/facebookresearch/demucs

## License

The separation tools use open-source models with permissive licenses. See individual dependencies for details.





