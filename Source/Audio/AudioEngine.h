#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
 * Represents a tone profile extracted from reference audio.
 * Contains frequency response, harmonic characteristics, and dynamic properties.
 */
struct ToneProfile
{
    // Frequency response data (magnitude at different frequencies)
    std::vector<float> frequencyResponse;
    std::vector<float> frequencies;
    
    // Harmonic analysis
    std::vector<float> harmonicContent; // Relative levels of harmonics
    
    // Dynamic characteristics
    float compressionRatio = 1.0f;
    float compressionThreshold = 0.0f;
    float saturationAmount = 0.0f;
    
    // EQ characteristics (parametric bands)
    struct EQBand
    {
        float frequency;
        float gain;
        float Q;
    };
    std::vector<EQBand> eqBands;
    
    // Metadata
    double sampleRate = 44100.0;
    int totalSamplesAnalyzed = 0;
    juce::String profileName;
    
    bool isValid() const
    {
        return !frequencyResponse.empty() && totalSamplesAnalyzed > 0;
    }
};





