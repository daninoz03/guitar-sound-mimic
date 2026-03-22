#pragma once

#include <JuceHeader.h>
#include <array>
#include <atomic>
#include "AudioEngine.h"

//==============================================================================
/**
 * ToneMatcher applies a ToneProfile to incoming audio via a 10-band peak EQ.
 *
 * Thread-safety: setProfile() can be called from the message thread at any
 * time.  The new coefficients are picked up at the start of the next audio
 * block using an atomic handoff, so coefficient updates always happen on the
 * audio thread and never race with ongoing processing.
 */
class ToneMatcher
{
public:
    ToneMatcher() = default;
    ~ToneMatcher() = default;

    void prepare(double sampleRate, int samplesPerBlock);
    void reset();

    /** Call from the message thread. New EQ is applied on the next audio block. */
    void setProfile(const std::unique_ptr<ToneProfile>& profile);

    /** Call from the audio thread only. */
    void processBlock(juce::AudioBuffer<float>& buffer, int numSamples);

private:
    // ---- simple biquad -------------------------------------------------
    struct Coeffs
    {
        float b0 = 1.0f, b1 = 0.0f, b2 = 0.0f;
        float a1 = 0.0f, a2 = 0.0f;
    };

    struct BiquadState
    {
        float x1 = 0.0f, x2 = 0.0f, y1 = 0.0f, y2 = 0.0f;

        float process (float x, const Coeffs& c) noexcept
        {
            float y = c.b0 * x + c.b1 * x1 + c.b2 * x2
                               - c.a1 * y1 - c.a2 * y2;
            if (! std::isfinite (y)) { y = 0.0f; x1 = x2 = y1 = y2 = 0.0f; }
            x2 = x1;  x1 = x;
            y2 = y1;  y1 = y;
            return y;
        }

        void reset() noexcept { x1 = x2 = y1 = y2 = 0.0f; }
    };

    static Coeffs makePeakFilter (double sr, double freq, double Q, double gainDb) noexcept;

    // ---- EQ bands -------------------------------------------------------
    static constexpr int NUM_BANDS = 10;
    static constexpr int MAX_CHANNELS = 2;

    // Active coefficients — read only on the audio thread
    std::array<Coeffs, NUM_BANDS> activeCoeffs;
    // Per-channel filter state
    std::array<std::array<BiquadState, NUM_BANDS>, MAX_CHANNELS> filterState;

    // Pending coefficients — written by message thread, read on audio thread
    // Protected by the acquire/release ordering of coeffsPending.
    std::array<Coeffs, NUM_BANDS> pendingCoeffs;
    std::atomic<bool> coeffsPending { false };

    // ---- state ----------------------------------------------------------
    double currentSampleRate = 44100.0;
    bool prepared = false;

    void computeCoeffs (const ToneProfile& profile,
                        std::array<Coeffs, NUM_BANDS>& out) const noexcept;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ToneMatcher)
};
