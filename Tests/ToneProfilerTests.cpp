#include <JuceHeader.h>
#include "../Source/Audio/ToneProfiler.h"

//==============================================================================
class ToneProfilerTests : public juce::UnitTest
{
public:
    ToneProfilerTests()
        : juce::UnitTest("ToneProfiler", "Audio")
    {
    }

    void runTest() override
    {
        beginTest("ToneProfiler initialization");
        {
            ToneProfiler profiler;
            expect(profiler.getSamplesAnalyzed() == 0, "Initial sample count should be 0");
        }

        beginTest("ToneProfiler prepare");
        {
            ToneProfiler profiler;
            profiler.prepare(44100.0, 512);
            expect(profiler.getSamplesAnalyzed() == 0, "Sample count should still be 0 after prepare");
        }

        beginTest("ToneProfiler reset");
        {
            ToneProfiler profiler;
            profiler.prepare(44100.0, 512);
            
            // Process some audio
            juce::AudioBuffer<float> buffer(2, 512);
            buffer.clear();
            profiler.processBlock(buffer, 512);
            
            expect(profiler.getSamplesAnalyzed() > 0, "Should have processed samples");
            
            profiler.reset();
            expect(profiler.getSamplesAnalyzed() == 0, "Sample count should be 0 after reset");
        }

        beginTest("ToneProfiler processes audio blocks");
        {
            ToneProfiler profiler;
            const double sampleRate = 44100.0;
            const int blockSize = 512;
            profiler.prepare(sampleRate, blockSize);

            juce::AudioBuffer<float> buffer(2, blockSize);
            
            // Generate a simple sine wave at 440 Hz (A4)
            const float frequency = 440.0f;
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            {
                float* channelData = buffer.getWritePointer(channel);
                for (int i = 0; i < blockSize; ++i)
                {
                    float phase = (i / (float)sampleRate) * frequency * 2.0f * juce::MathConstants<float>::pi;
                    channelData[i] = std::sin(phase);
                }
            }

            profiler.processBlock(buffer, blockSize);
            
            expect(profiler.getSamplesAnalyzed() == blockSize, 
                   "Should have processed exactly one block of samples");
        }

        beginTest("ToneProfiler accumulates samples across multiple blocks");
        {
            ToneProfiler profiler;
            const double sampleRate = 44100.0;
            const int blockSize = 512;
            profiler.prepare(sampleRate, blockSize);

            juce::AudioBuffer<float> buffer(2, blockSize);
            buffer.clear();

            // Process multiple blocks
            const int numBlocks = 10;
            for (int i = 0; i < numBlocks; ++i)
            {
                profiler.processBlock(buffer, blockSize);
            }

            expect(profiler.getSamplesAnalyzed() == blockSize * numBlocks,
                   "Should accumulate samples across all blocks");
        }

        beginTest("ToneProfiler generates profile with sufficient data");
        {
            ToneProfiler profiler;
            const double sampleRate = 44100.0;
            const int blockSize = 512;
            profiler.prepare(sampleRate, blockSize);

            juce::AudioBuffer<float> buffer(2, blockSize);
            
            // Generate audio with some content
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            {
                float* channelData = buffer.getWritePointer(channel);
                for (int i = 0; i < blockSize; ++i)
                {
                    channelData[i] = juce::Random::getSystemRandom().nextFloat() * 2.0f - 1.0f;
                }
            }

            // Process enough samples to meet minimum requirement
            const int numBlocks = (ToneProfiler::MIN_SAMPLES_FOR_PROFILE / blockSize) + 10;
            for (int i = 0; i < numBlocks; ++i)
            {
                profiler.processBlock(buffer, blockSize);
            }

            auto profile = profiler.generateProfile();
            
            expect(profile != nullptr, "Profile should be generated with sufficient data");
            if (profile != nullptr)
            {
                expect(profile->isValid(), "Generated profile should be valid");
                expect(profile->sampleRate == sampleRate, "Profile should store correct sample rate");
                expect(!profile->frequencyResponse.empty(), "Profile should have frequency response data");
            }
        }

        beginTest("ToneProfiler fails to generate profile with insufficient data");
        {
            ToneProfiler profiler;
            profiler.prepare(44100.0, 512);

            juce::AudioBuffer<float> buffer(2, 512);
            buffer.clear();

            // Process only a few blocks (not enough for a profile)
            profiler.processBlock(buffer, 512);
            profiler.processBlock(buffer, 512);

            auto profile = profiler.generateProfile();
            
            expect(profile == nullptr, "Profile should not be generated with insufficient data");
        }

        beginTest("ToneProfiler analyzes frequency content");
        {
            ToneProfiler profiler;
            const double sampleRate = 44100.0;
            const int blockSize = 4096; // Larger block for better frequency resolution
            profiler.prepare(sampleRate, blockSize);

            juce::AudioBuffer<float> buffer(2, blockSize);
            
            // Generate a sine wave at known frequency
            const float testFreq = 1000.0f; // 1 kHz
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            {
                float* channelData = buffer.getWritePointer(channel);
                for (int i = 0; i < blockSize; ++i)
                {
                    float phase = (i / (float)sampleRate) * testFreq * 2.0f * juce::MathConstants<float>::pi;
                    channelData[i] = std::sin(phase) * 0.5f;
                }
            }

            // Process enough blocks
            const int numBlocks = (ToneProfiler::MIN_SAMPLES_FOR_PROFILE / blockSize) + 5;
            for (int i = 0; i < numBlocks; ++i)
            {
                profiler.processBlock(buffer, blockSize);
            }

            auto profile = profiler.generateProfile();
            
            expect(profile != nullptr, "Profile should be generated");
            if (profile != nullptr)
            {
                expect(profile->frequencyResponse.size() > 0, 
                       "Profile should contain frequency response data");
                expect(profile->frequencies.size() == profile->frequencyResponse.size(),
                       "Frequencies and response should have same size");
            }
        }

        beginTest("ToneProfiler handles silence gracefully");
        {
            ToneProfiler profiler;
            const double sampleRate = 44100.0;
            const int blockSize = 512;
            profiler.prepare(sampleRate, blockSize);

            juce::AudioBuffer<float> buffer(2, blockSize);
            buffer.clear(); // Silent audio

            // Process enough blocks
            const int numBlocks = (ToneProfiler::MIN_SAMPLES_FOR_PROFILE / blockSize) + 10;
            for (int i = 0; i < numBlocks; ++i)
            {
                profiler.processBlock(buffer, blockSize);
            }

            auto profile = profiler.generateProfile();
            
            // Should still generate a profile, even with silence
            expect(profile != nullptr, "Profile should be generated even with silence");
        }
    }
};

static ToneProfilerTests toneProfilerTests;





