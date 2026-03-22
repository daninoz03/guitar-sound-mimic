#pragma once

#include <JuceHeader.h>
#include "AudioEngine.h"
#include "../DSP/FrequencyAnalyzer.h"

//==============================================================================
/**
 * ToneProfiler analyzes incoming audio to create a tone profile.
 * It captures frequency response, harmonic content, and dynamics.
 */
class ToneProfiler
{
public:
    ToneProfiler();
    ~ToneProfiler();

    void prepare(double sampleRate, int samplesPerBlock);
    void reset();
    
    /**
     * Process an audio block and accumulate analysis data.
     */
    void processBlock(juce::AudioBuffer<float>& buffer, int numSamples);
    
    /**
     * Generate a ToneProfile from accumulated analysis data.
     * Returns nullptr if insufficient data has been collected.
     */
    std::unique_ptr<ToneProfile> generateProfile();
    
    int getSamplesAnalyzed() const { return totalSamplesAnalyzed; }

    /** Raw mono recording captured during profiling (up to 30 s). */
    const std::vector<float>& getRecordedSamples() const { return recordedSamples; }
    double getRecordedSampleRate() const { return currentSampleRate; }

    static constexpr int MIN_SAMPLES_FOR_PROFILE = 132300;
    static constexpr int MAX_RECORD_SAMPLES = 44100 * 30; // 30 s

private:
    void analyzeFrequencyContent(const juce::AudioBuffer<float>& buffer);
    void analyzeDynamics(const juce::AudioBuffer<float>& buffer);
    void analyzeHarmonics(const juce::AudioBuffer<float>& buffer);

    std::unique_ptr<FrequencyAnalyzer> frequencyAnalyzer;
    
    // Accumulated analysis data
    std::vector<std::vector<float>> frequencySnapshots;
    std::vector<float> peakLevels;
    std::vector<float> rmsLevels;

    // Raw mono recording for playback verification
    std::vector<float> recordedSamples;
    
    double currentSampleRate = 44100.0;
    int totalSamplesAnalyzed = 0;
    int expectedSamplesPerBlock = 512;
    
    // FFT for analysis
    static constexpr int fftOrder = 11; // 2048 samples — still fine resolution,
                                        // but reached far sooner than 4096
    static constexpr int fftSize = 1 << fftOrder;

    juce::dsp::FFT fft;
    juce::dsp::WindowingFunction<float> window;
    std::array<float, fftSize * 2> fftData;

    // Sample accumulator — audio blocks are usually much smaller than fftSize,
    // so we buffer incoming mono samples until we have a full FFT frame.
    std::array<float, fftSize> sampleAccumulator{};
    int accumulatorFill = 0;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ToneProfiler)
};





