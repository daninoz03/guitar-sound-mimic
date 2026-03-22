#!/usr/bin/env python3
"""
Guitar Source Separation Tool
Extracts guitar track from mixed audio using Demucs AI model.
"""

import argparse
import sys
import os
from pathlib import Path

try:
    import torch
    import torchaudio
    from demucs.pretrained import get_model
    from demucs.apply import apply_model
    DEMUCS_AVAILABLE = True
except ImportError:
    DEMUCS_AVAILABLE = False

def separate_guitar_demucs(input_file, output_file):
    """
    Separate guitar from mixed audio using Demucs.
    
    Args:
        input_file: Path to input audio file (MP3, WAV, FLAC, etc.)
        output_file: Path to output guitar-only WAV file
    """
    if not DEMUCS_AVAILABLE:
        print("❌ Error: Demucs is not installed.")
        print("\nInstall with:")
        print("  pip install demucs")
        print("  or")
        print("  pip install -r tools/requirements.txt")
        sys.exit(1)
    
    print(f"🎸 Separating guitar from: {input_file}")
    print("   (This may take 30-60 seconds...)")
    
    # Load the model
    print("📦 Loading Demucs model...")
    model = get_model('htdemucs')  # High-quality model
    model.cpu()  # Use CPU (or model.cuda() for GPU)
    model.eval()
    
    # Load audio
    print("📂 Loading audio file...")
    wav, sr = torchaudio.load(input_file)
    
    # Resample if needed
    if sr != model.samplerate:
        print(f"🔄 Resampling from {sr}Hz to {model.samplerate}Hz...")
        resampler = torchaudio.transforms.Resample(sr, model.samplerate)
        wav = resampler(wav)
        sr = model.samplerate
    
    # Ensure stereo
    if wav.shape[0] == 1:
        wav = wav.repeat(2, 1)
    
    # Apply separation
    print("🧠 Running AI separation model...")
    ref = wav.mean(0)
    wav = (wav - ref.mean()) / ref.std()
    
    with torch.no_grad():
        sources = apply_model(model, wav[None], device='cpu')[0]
    
    sources = sources * ref.std() + ref.mean()
    
    # Extract guitar/other (Demucs separates into: drums, bass, other, vocals)
    # Guitar is typically in "other" stem
    # Model outputs: [drums, bass, other, vocals]
    stems_names = ['drums', 'bass', 'other', 'vocals']
    other_idx = stems_names.index('other')
    guitar_audio = sources[other_idx]
    
    # Save
    print(f"💾 Saving guitar track to: {output_file}")
    torchaudio.save(output_file, guitar_audio.cpu(), sr)
    
    print("✅ Guitar extraction complete!")
    print(f"\n🎵 Guitar track saved: {output_file}")
    print("\nYou can now load this file in the Guitar Sound Mimic app for profiling.")
    
    return output_file

def separate_guitar_simple(input_file, output_file):
    """
    Simple frequency-based guitar isolation (fallback method).
    Not as good as AI separation, but works without dependencies.
    """
    try:
        import soundfile as sf
        import numpy as np
        from scipy import signal
    except ImportError:
        print("❌ Error: Required libraries not installed.")
        print("\nInstall with:")
        print("  pip install soundfile numpy scipy")
        sys.exit(1)
    
    print(f"🎸 Simple guitar isolation from: {input_file}")
    print("⚠️  Note: This is a basic method. For best results, use Demucs.")
    
    # Load audio
    audio, sr = sf.read(input_file)
    
    # Convert to mono if stereo
    if len(audio.shape) > 1:
        audio = np.mean(audio, axis=1)
    
    # Apply bandpass filter for guitar frequencies (80Hz - 5kHz)
    nyquist = sr / 2
    low = 80.0 / nyquist
    high = 5000.0 / nyquist
    
    sos = signal.butter(4, [low, high], btype='band', output='sos')
    filtered = signal.sosfilt(sos, audio)
    
    # Convert back to stereo
    output = np.stack([filtered, filtered], axis=1)
    
    # Save
    sf.write(output_file, output, sr)
    
    print("✅ Basic isolation complete!")
    print(f"💾 Saved: {output_file}")
    print("\n⚠️  For much better results, install Demucs:")
    print("   pip install demucs")

def main():
    parser = argparse.ArgumentParser(
        description='Extract guitar track from mixed audio using AI',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python separate_guitar.py song.mp3 guitar_only.wav
  python separate_guitar.py -m simple song.mp3 guitar.wav
  
The output file can then be loaded in Guitar Sound Mimic for profiling.
        """
    )
    
    parser.add_argument('input', help='Input audio file (MP3, WAV, FLAC, etc.)')
    parser.add_argument('output', help='Output guitar file (WAV format)')
    parser.add_argument('-m', '--method', 
                       choices=['demucs', 'simple'],
                       default='demucs',
                       help='Separation method (default: demucs)')
    
    args = parser.parse_args()
    
    # Validate input file
    if not os.path.exists(args.input):
        print(f"❌ Error: Input file not found: {args.input}")
        sys.exit(1)
    
    # Create output directory if needed
    output_dir = os.path.dirname(args.output)
    if output_dir and not os.path.exists(output_dir):
        os.makedirs(output_dir)
    
    # Run separation
    try:
        if args.method == 'demucs':
            separate_guitar_demucs(args.input, args.output)
        else:
            separate_guitar_simple(args.input, args.output)
    except Exception as e:
        print(f"\n❌ Error during separation: {e}")
        sys.exit(1)

if __name__ == '__main__':
    main()





