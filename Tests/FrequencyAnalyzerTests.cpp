#include <JuceHeader.h>
#include "../Source/DSP/FrequencyAnalyzer.h"

//==============================================================================
class FrequencyAnalyzerTests : public juce::UnitTest
{
public:
    FrequencyAnalyzerTests()
        : juce::UnitTest("FrequencyAnalyzer", "DSP")
    {
    }

    void runTest() override
    {
        beginTest("FrequencyAnalyzer initialization");
        {
            FrequencyAnalyzer analyzer;
            expect(analyzer.getFundamentalFrequency() == 0.0f, 
                   "Initial fundamental frequency should be 0");
            expect(analyzer.getHarmonicContent().size() > 0, 
                   "Should have harmonic content array");
        }

        beginTest("FrequencyAnalyzer prepare");
        {
            FrequencyAnalyzer analyzer;
            analyzer.prepare(44100.0, 512);
            expect(analyzer.getFundamentalFrequency() == 0.0f, 
                   "Fundamental frequency should still be 0 after prepare");
        }

        beginTest("FrequencyAnalyzer reset");
        {
            FrequencyAnalyzer analyzer;
            analyzer.prepare(44100.0, 512);
            analyzer.reset();
            expect(analyzer.getFundamentalFrequency() == 0.0f, 
                   "Fundamental frequency should be 0 after reset");
        }

        beginTest("FrequencyAnalyzer detects fundamental frequency");
        {
            FrequencyAnalyzer analyzer;
            const double sampleRate = 44100.0;
            const int blockSize = 4096; // Large block for good frequency resolution
            analyzer.prepare(sampleRate, blockSize);

            juce::AudioBuffer<float> buffer(2, blockSize);
            
            // Generate a pure tone at 440 Hz (A4)
            const float targetFreq = 440.0f;
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            {
                float* channelData = buffer.getWritePointer(channel);
                for (int i = 0; i < blockSize; ++i)
                {
                    float phase = (i / (float)sampleRate) * targetFreq * 
                                  2.0f * juce::MathConstants<float>::pi;
                    channelData[i] = std::sin(phase) * 0.5f;
                }
            }

            analyzer.analyzeBuffer(buffer);
            
            float detectedFreq = analyzer.getFundamentalFrequency();
            
            // Allow 5% tolerance for FFT bin resolution
            float tolerance = targetFreq * 0.05f;
            expect(std::abs(detectedFreq - targetFreq) < tolerance,
                   "Detected frequency (" + juce::String(detectedFreq) + 
                   " Hz) should be close to target (" + juce::String(targetFreq) + " Hz)");
        }

        beginTest("FrequencyAnalyzer detects low guitar frequencies");
        {
            FrequencyAnalyzer analyzer;
            const double sampleRate = 44100.0;
            const int blockSize = 8192; // Very large for low frequency resolution
            analyzer.prepare(sampleRate, blockSize);

            juce::AudioBuffer<float> buffer(2, blockSize);
            
            // Generate low E string frequency (82.41 Hz)
            const float targetFreq = 82.41f;
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            {
                float* channelData = buffer.getWritePointer(channel);
                for (int i = 0; i < blockSize; ++i)
                {
                    float phase = (i / (float)sampleRate) * targetFreq * 
                                  2.0f * juce::MathConstants<float>::pi;
                    channelData[i] = std::sin(phase) * 0.5f;
                }
            }

            analyzer.analyzeBuffer(buffer);
            
            float detectedFreq = analyzer.getFundamentalFrequency();
            
            // Low frequencies need more tolerance due to FFT bin width
            expect(detectedFreq > 50.0f && detectedFreq < 150.0f,
                   "Should detect low guitar frequencies (detected: " + 
                   juce::String(detectedFreq) + " Hz)");
        }

        beginTest("FrequencyAnalyzer extracts harmonics");
        {
            FrequencyAnalyzer analyzer;
            const double sampleRate = 44100.0;
            const int blockSize = 4096;
            analyzer.prepare(sampleRate, blockSize);

            juce::AudioBuffer<float> buffer(2, blockSize);
            
            // Generate a tone with harmonics
            const float fundamental = 200.0f;
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            {
                float* channelData = buffer.getWritePointer(channel);
                for (int i = 0; i < blockSize; ++i)
                {
                    float phase1 = (i / (float)sampleRate) * fundamental * 
                                   2.0f * juce::MathConstants<float>::pi;
                    float phase2 = (i / (float)sampleRate) * fundamental * 2.0f * 
                                   2.0f * juce::MathConstants<float>::pi;
                    float phase3 = (i / (float)sampleRate) * fundamental * 3.0f * 
                                   2.0f * juce::MathConstants<float>::pi;
                    
                    // Fundamental + 2nd harmonic + 3rd harmonic
                    channelData[i] = std::sin(phase1) * 0.5f + 
                                    std::sin(phase2) * 0.25f + 
                                    std::sin(phase3) * 0.125f;
                }
            }

            analyzer.analyzeBuffer(buffer);
            
            auto harmonics = analyzer.getHarmonicContent();
            expect(harmonics.size() > 0, "Should have harmonic content");
            
            // First harmonic should be normalized to 1.0
            if (harmonics.size() > 0)
            {
                expect(std::abs(harmonics[0] - 1.0f) < 0.1f,
                       "First harmonic should be approximately 1.0");
            }
        }

        beginTest("FrequencyAnalyzer handles silence");
        {
            FrequencyAnalyzer analyzer;
            const double sampleRate = 44100.0;
            const int blockSize = 4096;
            analyzer.prepare(sampleRate, blockSize);

            juce::AudioBuffer<float> buffer(2, blockSize);
            buffer.clear(); // Silent buffer

            analyzer.analyzeBuffer(buffer);
            
            // Should not crash with silence
            expect(true, "Should handle silence without crashing");
        }

        beginTest("FrequencyAnalyzer handles white noise");
        {
            FrequencyAnalyzer analyzer;
            const double sampleRate = 44100.0;
            const int blockSize = 4096;
            analyzer.prepare(sampleRate, blockSize);

            juce::AudioBuffer<float> buffer(2, blockSize);
            
            // Generate white noise
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            {
                float* channelData = buffer.getWritePointer(channel);
                for (int i = 0; i < blockSize; ++i)
                {
                    channelData[i] = juce::Random::getSystemRandom().nextFloat() * 2.0f - 1.0f;
                }
            }

            analyzer.analyzeBuffer(buffer);
            
            // Should not crash with noise
            expect(true, "Should handle white noise without crashing");
        }

        beginTest("FrequencyAnalyzer spectrum output");
        {
            FrequencyAnalyzer analyzer;
            const double sampleRate = 44100.0;
            const int blockSize = 4096;
            analyzer.prepare(sampleRate, blockSize);

            juce::AudioBuffer<float> buffer(2, blockSize);
            
            // Generate test tone
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            {
                float* channelData = buffer.getWritePointer(channel);
                for (int i = 0; i < blockSize; ++i)
                {
                    channelData[i] = std::sin((i / (float)sampleRate) * 440.0f * 
                                              2.0f * juce::MathConstants<float>::pi) * 0.5f;
                }
            }

            analyzer.analyzeBuffer(buffer);
            
            auto spectrum = analyzer.getSpectrum();
            expect(spectrum.size() > 0, "Spectrum should have data");
            expect(spectrum.size() == blockSize / 2, 
                   "Spectrum size should be half FFT size");
        }

        beginTest("FrequencyAnalyzer multiple analyses");
        {
            FrequencyAnalyzer analyzer;
            const double sampleRate = 44100.0;
            const int blockSize = 4096;
            analyzer.prepare(sampleRate, blockSize);

            // Analyze multiple different frequencies in sequence
            std::vector<float> testFreqs = {110.0f, 220.0f, 440.0f, 880.0f};

            for (float freq : testFreqs)
            {
                juce::AudioBuffer<float> buffer(2, blockSize);
                
                for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
                {
                    float* channelData = buffer.getWritePointer(channel);
                    for (int i = 0; i < blockSize; ++i)
                    {
                        float phase = (i / (float)sampleRate) * freq * 
                                      2.0f * juce::MathConstants<float>::pi;
                        channelData[i] = std::sin(phase) * 0.5f;
                    }
                }

                analyzer.analyzeBuffer(buffer);
                float detected = analyzer.getFundamentalFrequency();
                
                // Should detect something in a reasonable range
                expect(detected > 50.0f && detected < 2000.0f,
                       "Should detect frequency in reasonable range for " + 
                       juce::String(freq) + " Hz");
            }
        }
    }
};

static FrequencyAnalyzerTests frequencyAnalyzerTests;





