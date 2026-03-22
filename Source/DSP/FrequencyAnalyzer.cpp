#include "FrequencyAnalyzer.h"

FrequencyAnalyzer::FrequencyAnalyzer()
    : fft(fftOrder)
    , window(fftSize, juce::dsp::WindowingFunction<float>::hann)
{
    harmonicLevels.resize(numHarmonics, 0.0f);
    spectrum.resize(fftSize / 2, 0.0f);
}

FrequencyAnalyzer::~FrequencyAnalyzer()
{
}

void FrequencyAnalyzer::prepare(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    reset();
}

void FrequencyAnalyzer::reset()
{
    fundamentalFreq = 0.0f;
    std::fill(harmonicLevels.begin(), harmonicLevels.end(), 0.0f);
    std::fill(spectrum.begin(), spectrum.end(), 0.0f);
    std::fill(fftData.begin(), fftData.end(), 0.0f);
}

void FrequencyAnalyzer::analyzeBuffer(const juce::AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();
    
    if (numSamples < fftSize)
        return;
    
    // Mix to mono
    std::fill(fftData.begin(), fftData.end(), 0.0f);
    
    for (int channel = 0; channel < numChannels; ++channel)
    {
        const float* channelData = buffer.getReadPointer(channel);
        for (int i = 0; i < std::min(numSamples, fftSize); ++i)
        {
            fftData[i] += channelData[i] / numChannels;
        }
    }
    
    // Apply window
    window.multiplyWithWindowingTable(fftData.data(), fftSize);
    
    // Perform FFT
    fft.performFrequencyOnlyForwardTransform(fftData.data());
    
    // Store spectrum
    for (int i = 0; i < fftSize / 2; ++i)
    {
        spectrum[i] = fftData[i];
    }
    
    // Detect fundamental frequency
    detectFundamental(fftData.data(), fftSize);
    
    // Extract harmonics
    extractHarmonics(fftData.data(), fftSize);
}

void FrequencyAnalyzer::detectFundamental(const float* fftData, int fftSize)
{
    // Find peak in FFT (ignoring DC and very low frequencies)
    const int minBin = static_cast<int>((80.0f / currentSampleRate) * fftSize); // Start at 80Hz
    const int maxBin = static_cast<int>((1000.0f / currentSampleRate) * fftSize); // Guitar fundamentals usually below 1kHz
    
    int peakBin = minBin;
    float peakMagnitude = fftData[minBin];
    
    for (int i = minBin; i < maxBin && i < fftSize / 2; ++i)
    {
        if (fftData[i] > peakMagnitude)
        {
            peakMagnitude = fftData[i];
            peakBin = i;
        }
    }
    
    // Convert bin to frequency
    fundamentalFreq = (peakBin * currentSampleRate) / fftSize;
}

void FrequencyAnalyzer::extractHarmonics(const float* fftData, int fftSize)
{
    if (fundamentalFreq < 20.0f)
    {
        std::fill(harmonicLevels.begin(), harmonicLevels.end(), 0.0f);
        return;
    }
    
    // Find magnitude at each harmonic frequency
    for (int h = 0; h < numHarmonics; ++h)
    {
        float harmonicFreq = fundamentalFreq * (h + 1);
        int bin = static_cast<int>((harmonicFreq / currentSampleRate) * fftSize);
        
        if (bin < fftSize / 2)
        {
            harmonicLevels[h] = fftData[bin];
        }
        else
        {
            harmonicLevels[h] = 0.0f;
        }
    }
    
    // Normalize harmonics relative to fundamental
    float fundamental = harmonicLevels[0];
    if (fundamental > 0.0f)
    {
        for (int h = 0; h < numHarmonics; ++h)
        {
            harmonicLevels[h] /= fundamental;
        }
    }
}





