#include "SamplePlayer.h"

// Blues-pentatonic riff: E3 → ascending run → descending, with a held root at the end
const std::vector<SamplePlayer::RiffNote> SamplePlayer::riffNotes = {
    { 164.8f, 0.30f },  // E3
    { 196.0f, 0.25f },  // G3
    { 220.0f, 0.25f },  // A3
    { 246.9f, 0.25f },  // B3
    { 293.7f, 0.25f },  // D4
    { 329.6f, 0.50f },  // E4  (peak of run)
    { 293.7f, 0.20f },  // D4
    { 246.9f, 0.20f },  // B3
    { 220.0f, 0.20f },  // A3
    { 196.0f, 0.20f },  // G3
    { 164.8f, 0.60f },  // E3  (resolving root, held)
    { 123.5f, 0.70f },  // B2  (low finish)
};

//==============================================================================
SamplePlayer::SamplePlayer()
{
    delayLine.fill(0.0f);
}

void SamplePlayer::prepare(double sr, int /*samplesPerBlock*/)
{
    sampleRate = sr;
    reset();
}

void SamplePlayer::reset()
{
    delayLine.fill(0.0f);
    delayLength = 0;
    readPos = 0;
    prevSample = 0.0f;
    envGain = 0.0f;
    playing = false;
    currentNoteIndex = 0;
    samplesUntilNextNote = 0;
}

void SamplePlayer::startPlaying()
{
    reset();
    playing = true;
    // Trigger first note immediately
    samplesUntilNextNote = 0;
}

//==============================================================================
void SamplePlayer::triggerNote(float frequency)
{
    if (frequency <= 0.0f) return;

    delayLength = juce::jlimit(2, MAX_DELAY - 1,
                               static_cast<int>(std::round(sampleRate / frequency)));

    // Seed delay line with bandlimited noise (sounds more guitar-like)
    juce::Random rng;
    for (int i = 0; i < delayLength; ++i)
        delayLine[i] = rng.nextFloat() * 2.0f - 1.0f;

    readPos    = 0;
    prevSample = 0.0f;

    // Decay envelope: Karplus-Strong naturally decays; this adds a small
    // per-sample multiplier to shape sustain further.
    // Roughly 3-second decay at 44.1kHz ≈ 0.999978 per sample
    envDecayPerSample = std::pow(0.001f, 1.0f / (3.0f * static_cast<float>(sampleRate)));
    envGain = 1.0f;
}

float SamplePlayer::nextSample()
{
    if (delayLength <= 0) return 0.0f;

    float out = delayLine[readPos];

    // Karplus-Strong averaging filter (one-pole low-pass)
    float filtered = 0.5f * (out + prevSample);
    prevSample = out;

    delayLine[readPos] = filtered;
    readPos = (readPos + 1) % delayLength;

    envGain *= envDecayPerSample;
    return out * envGain;
}

//==============================================================================
void SamplePlayer::fillBuffer(juce::AudioBuffer<float>& buffer, int numSamples)
{
    if (!playing) return;

    const int totalNotes = static_cast<int>(riffNotes.size());

    for (int i = 0; i < numSamples; ++i)
    {
        // Schedule the next note when the timer expires
        if (samplesUntilNextNote <= 0)
        {
            if (currentNoteIndex >= totalNotes)
            {
                // Riff finished — zero remaining samples and stop
                for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
                    buffer.clear(ch, i, numSamples - i);

                playing = false;
                return;
            }

            const auto& note = riffNotes[currentNoteIndex];
            triggerNote(note.frequency);
            samplesUntilNextNote = static_cast<int>(note.durationSeconds * sampleRate);
            ++currentNoteIndex;
        }

        float s = nextSample();
        --samplesUntilNextNote;

        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            buffer.setSample(ch, i, s);
    }
}
