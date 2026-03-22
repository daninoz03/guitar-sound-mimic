#pragma once

#include <JuceHeader.h>
#include <array>
#include <vector>

//==============================================================================
/**
 * SamplePlayer synthesises a short guitar riff using the Karplus-Strong
 * plucked-string algorithm, then routes it through the ToneMatcher so the
 * user can hear what the profiled tone sounds like without needing a physical
 * audio interface.
 */
class SamplePlayer
{
public:
    SamplePlayer();
    ~SamplePlayer() = default;

    void prepare(double sampleRate, int samplesPerBlock);
    void reset();

    void startPlaying();
    bool isPlaying() const { return playing; }

    /** Fill buffer with synthesised guitar-riff audio (mono, copied to all channels). */
    void fillBuffer(juce::AudioBuffer<float>& buffer, int numSamples);

private:
    void triggerNote(float frequency);
    float nextSample();

    // Karplus-Strong delay line
    static constexpr int MAX_DELAY = 2048;
    std::array<float, MAX_DELAY> delayLine{};
    int delayLength = 0;
    int readPos = 0;
    float prevSample = 0.0f;   // for the one-sample averaging filter

    // Amplitude envelope (simple exponential decay per note)
    float envGain = 0.0f;
    float envDecayPerSample = 0.9999f;

    // Note scheduler  ---------------------------------------------------
    struct RiffNote { float frequency; float durationSeconds; };

    // Blues-pentatonic riff in E (sounds great on any guitar tone)
    static const std::vector<RiffNote> riffNotes;

    int  currentNoteIndex = 0;
    int  samplesUntilNextNote = 0;
    bool playing = false;

    double sampleRate = 44100.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SamplePlayer)
};
