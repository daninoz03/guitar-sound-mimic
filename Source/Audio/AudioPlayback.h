#pragma once

#include <JuceHeader.h>
#include <vector>

//==============================================================================
/**
 * Simple single-use playback of a pre-loaded mono PCM buffer.
 * All public methods are message-thread-safe; fillBuffer() must only be called
 * from the audio thread.
 */
class AudioPlayback
{
public:
    AudioPlayback() = default;
    ~AudioPlayback() = default;

    /** Load samples (mono).  Safe to call from the message thread. */
    void loadSamples (const std::vector<float>& samples, double sr)
    {
        data       = samples;
        sampleRate = sr;
        playPos    = 0;
        playing    = false;
    }

    bool hasSamples() const { return !data.empty(); }
    bool isPlaying()  const { return playing; }

    void startPlaying()
    {
        if (data.empty()) return;
        playPos = 0;
        playing = true;
    }

    void stop()
    {
        playing = false;
        playPos = 0;
    }

    /** Fill buffer from stored audio (mono → all channels).
     *  Called from the audio thread only. */
    void fillBuffer (juce::AudioBuffer<float>& buffer, int numSamples)
    {
        if (!playing || data.empty()) return;

        const int total = static_cast<int> (data.size());

        for (int i = 0; i < numSamples; ++i)
        {
            if (playPos >= total)
            {
                // Reached end — silence the rest of this block
                for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
                    buffer.clear (ch, i, numSamples - i);
                playing = false;
                return;
            }

            const float s = data[static_cast<size_t> (playPos++)];
            for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
                buffer.setSample (ch, i, s);
        }
    }

private:
    std::vector<float> data;
    double sampleRate = 44100.0;
    int    playPos    = 0;
    bool   playing    = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPlayback)
};
