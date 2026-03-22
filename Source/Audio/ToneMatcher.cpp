#include "ToneMatcher.h"

//==============================================================================
ToneMatcher::Coeffs ToneMatcher::makePeakFilter (double sr, double freq,
                                                   double Q, double gainDb) noexcept
{
    // Standard Audio EQ Cookbook peak-filter formula
    const double A  = std::pow (10.0, gainDb / 40.0);
    const double w0 = 2.0 * juce::MathConstants<double>::pi * freq / sr;
    const double alpha = std::sin (w0) / (2.0 * Q);
    const double cosW  = std::cos (w0);

    const double a0inv = 1.0 / (1.0 + alpha / A);

    Coeffs c;
    c.b0 = static_cast<float> ((1.0 + alpha * A) * a0inv);
    c.b1 = static_cast<float> ((-2.0 * cosW)     * a0inv);
    c.b2 = static_cast<float> ((1.0 - alpha * A) * a0inv);
    c.a1 = static_cast<float> ((-2.0 * cosW)     * a0inv);
    c.a2 = static_cast<float> ((1.0 - alpha / A) * a0inv);
    return c;
}

//==============================================================================
void ToneMatcher::prepare (double sampleRate, int /*samplesPerBlock*/)
{
    currentSampleRate = sampleRate;

    // Initialise all bands to 0 dB (identity peak filter)
    Coeffs identity = makePeakFilter (sampleRate, 1000.0, 1.0, 0.0);
    activeCoeffs.fill (identity);
    pendingCoeffs.fill (identity);

    reset();
    prepared = true;
}

void ToneMatcher::reset()
{
    for (auto& channel : filterState)
        for (auto& f : channel)
            f.reset();
}

//==============================================================================
void ToneMatcher::computeCoeffs (const ToneProfile& profile,
                                   std::array<Coeffs, NUM_BANDS>& out) const noexcept
{
    const int numBins = static_cast<int> (profile.frequencyResponse.size());
    if (numBins == 0) return;

    // Spectral mean for relative normalisation
    float sum = 0.0f;
    for (float v : profile.frequencyResponse) sum += v;
    const float mean = juce::jmax (0.0001f, sum / static_cast<float> (numBins));

    constexpr std::array<double, NUM_BANDS> centreFreqs {
        80.0, 150.0, 300.0, 500.0, 1000.0,
        2000.0, 4000.0, 8000.0, 12000.0, 16000.0
    };

    for (int band = 0; band < NUM_BANDS; ++band)
    {
        int bin = static_cast<int> (centreFreqs[band] / profile.sampleRate
                                    * static_cast<double> (numBins) * 2.0);
        bin = juce::jlimit (0, numBins - 1, bin);

        const float mag   = profile.frequencyResponse[static_cast<size_t> (bin)];
        const float ratio = (mag + 0.0001f) / mean;
        const double gainDb = juce::jlimit (-12.0, 12.0,
                                static_cast<double> (juce::Decibels::gainToDecibels (ratio)));

        out[static_cast<size_t> (band)] =
            makePeakFilter (currentSampleRate, centreFreqs[band], 1.0, gainDb);
    }
}

void ToneMatcher::setProfile (const std::unique_ptr<ToneProfile>& profile)
{
    if (profile == nullptr || !profile->isValid())
    {
        juce::Logger::writeToLog ("ToneMatcher: cannot set invalid profile");
        return;
    }

    // Compute new coefficients on the message thread and store them.
    // Signal the audio thread via a release store so the writes above are
    // visible before the flag is seen.
    computeCoeffs (*profile, pendingCoeffs);
    coeffsPending.store (true, std::memory_order_release);

    juce::Logger::writeToLog ("ToneMatcher: profile queued for audio thread");
}

//==============================================================================
void ToneMatcher::processBlock (juce::AudioBuffer<float>& buffer, int numSamples)
{
    if (! prepared) return;

    // Pick up any pending coefficient update atomically (acquire load ensures
    // pendingCoeffs writes are visible after the flag).
    if (coeffsPending.load (std::memory_order_acquire))
    {
        activeCoeffs = pendingCoeffs;
        coeffsPending.store (false, std::memory_order_relaxed);
        // Reset filter state so the new EQ starts from a clean slate
        reset();
    }

    const int numChannels = juce::jmin (buffer.getNumChannels(), MAX_CHANNELS);

    for (int ch = 0; ch < numChannels; ++ch)
    {
        float* data = buffer.getWritePointer (ch);
        auto&  filters = filterState[static_cast<size_t> (ch)];

        for (int i = 0; i < numSamples; ++i)
        {
            float s = data[i];
            for (int band = 0; band < NUM_BANDS; ++band)
                s = filters[static_cast<size_t> (band)].process (s, activeCoeffs[static_cast<size_t> (band)]);
            data[i] = juce::jlimit (-1.0f, 1.0f, s);
        }
    }
}
