#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
 * FrequencyAnalyzer performs detailed frequency and harmonic analysis on audio.
 */
class FrequencyAnalyzer
{
public:
    FrequencyAnalyzer();
    ~FrequencyAnalyzer();

    void prepare(double sampleRate, int samplesPerBlock);
    void reset();
    
    /**
     * Analyze a buffer of audio samples.
     */
    void analyzeBuffer(const juce::AudioBuffer<float>& buffer);
    
    /**
     * Get the fundamental frequency detected in the last analysis.
     */
    float getFundamentalFrequency() const { return fundamentalFreq; }
    
    /**
     * Get harmonic content (relative levels of first N harmonics).
     */
    std::vector<float> getHarmonicContent() const { return harmonicLevels; }
    
    /**
     * Get the full spectrum magnitude (FFT bins).
     */
    const std::vector<float>& getSpectrum() const { return spectrum; }

private:
    void detectFundamental(const float* fftData, int fftSize);
    void extractHarmonics(const float* fftData, int fftSize);

    double currentSampleRate = 44100.0;
    
    static constexpr int fftOrder = 12; // 4096 samples
    static constexpr int fftSize = 1 << fftOrder;
    static constexpr int numHarmonics = 8;
    
    juce::dsp::FFT fft;
    juce::dsp::WindowingFunction<float> window;
    std::array<float, fftSize * 2> fftData;
    
    float fundamentalFreq = 0.0f;
    std::vector<float> harmonicLevels;
    std::vector<float> spectrum;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FrequencyAnalyzer)
};





