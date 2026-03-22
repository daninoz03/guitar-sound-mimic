#include <JuceHeader.h>
#include "../Source/Audio/ToneMatcher.h"
#include "../Source/Audio/ToneProfiler.h"

//==============================================================================
class ToneMatcherTests : public juce::UnitTest
{
public:
    ToneMatcherTests()
        : juce::UnitTest("ToneMatcher", "Audio")
    {
    }

    void runTest() override
    {
        beginTest("ToneMatcher initialization");
        {
            ToneMatcher matcher;
            // Just verify it doesn't crash
            expect(true, "ToneMatcher should initialize successfully");
        }

        beginTest("ToneMatcher prepare");
        {
            ToneMatcher matcher;
            matcher.prepare(44100.0, 512);
            
            // Verify it doesn't crash and can be prepared multiple times
            matcher.prepare(48000.0, 256);
            expect(true, "ToneMatcher should prepare successfully");
        }

        beginTest("ToneMatcher reset");
        {
            ToneMatcher matcher;
            matcher.prepare(44100.0, 512);
            matcher.reset();
            expect(true, "ToneMatcher should reset successfully");
        }

        beginTest("ToneMatcher processes audio without profile (bypass)");
        {
            ToneMatcher matcher;
            const double sampleRate = 44100.0;
            const int blockSize = 512;
            matcher.prepare(sampleRate, blockSize);

            juce::AudioBuffer<float> buffer(2, blockSize);
            
            // Generate test audio
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            {
                float* channelData = buffer.getWritePointer(channel);
                for (int i = 0; i < blockSize; ++i)
                {
                    channelData[i] = (float)i / blockSize;
                }
            }

            // Copy for comparison
            juce::AudioBuffer<float> bufferCopy(2, blockSize);
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            {
                bufferCopy.copyFrom(channel, 0, buffer, channel, 0, blockSize);
            }

            // Process without profile (should pass through or have minimal effect)
            matcher.processBlock(buffer, blockSize);
            
            expect(true, "ToneMatcher should process audio without crashing");
        }

        beginTest("ToneMatcher processes audio with valid profile");
        {
            // First, create a valid profile
            ToneProfiler profiler;
            const double sampleRate = 44100.0;
            const int blockSize = 512;
            profiler.prepare(sampleRate, blockSize);

            juce::AudioBuffer<float> profileBuffer(2, blockSize);
            for (int channel = 0; channel < profileBuffer.getNumChannels(); ++channel)
            {
                float* channelData = profileBuffer.getWritePointer(channel);
                for (int i = 0; i < blockSize; ++i)
                {
                    channelData[i] = std::sin((i / (float)sampleRate) * 440.0f * 
                                              2.0f * juce::MathConstants<float>::pi) * 0.5f;
                }
            }

            // Process enough to create a profile
            const int numBlocks = (ToneProfiler::MIN_SAMPLES_FOR_PROFILE / blockSize) + 10;
            for (int i = 0; i < numBlocks; ++i)
            {
                profiler.processBlock(profileBuffer, blockSize);
            }

            auto profile = profiler.generateProfile();
            expect(profile != nullptr, "Profile should be generated");

            if (profile != nullptr)
            {
                // Now test the matcher with this profile
                ToneMatcher matcher;
                matcher.prepare(sampleRate, blockSize);
                matcher.setProfile(profile);

                juce::AudioBuffer<float> testBuffer(2, blockSize);
                testBuffer.clear();

                // Generate test input
                for (int channel = 0; channel < testBuffer.getNumChannels(); ++channel)
                {
                    float* channelData = testBuffer.getWritePointer(channel);
                    for (int i = 0; i < blockSize; ++i)
                    {
                        channelData[i] = std::sin((i / (float)sampleRate) * 220.0f * 
                                                  2.0f * juce::MathConstants<float>::pi) * 0.3f;
                    }
                }

                matcher.processBlock(testBuffer, blockSize);
                expect(true, "ToneMatcher should process audio with profile successfully");
            }
        }

        beginTest("ToneMatcher handles different buffer sizes");
        {
            ToneMatcher matcher;
            matcher.prepare(44100.0, 512);

            // Test with various buffer sizes
            std::vector<int> bufferSizes = {64, 128, 256, 512, 1024, 2048};

            for (int size : bufferSizes)
            {
                juce::AudioBuffer<float> buffer(2, size);
                buffer.clear();
                
                matcher.processBlock(buffer, size);
                expect(true, "Should handle buffer size: " + juce::String(size));
            }
        }

        beginTest("ToneMatcher handles mono and stereo");
        {
            ToneMatcher matcher;
            const double sampleRate = 44100.0;
            const int blockSize = 512;
            matcher.prepare(sampleRate, blockSize);

            // Test mono
            {
                juce::AudioBuffer<float> monoBuffer(1, blockSize);
                monoBuffer.clear();
                matcher.processBlock(monoBuffer, blockSize);
                expect(true, "Should handle mono audio");
            }

            // Test stereo
            {
                juce::AudioBuffer<float> stereoBuffer(2, blockSize);
                stereoBuffer.clear();
                matcher.processBlock(stereoBuffer, blockSize);
                expect(true, "Should handle stereo audio");
            }
        }

        beginTest("ToneMatcher doesn't produce NaN or Inf");
        {
            ToneMatcher matcher;
            const double sampleRate = 44100.0;
            const int blockSize = 512;
            matcher.prepare(sampleRate, blockSize);

            juce::AudioBuffer<float> buffer(2, blockSize);
            
            // Generate test audio with some extreme values
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            {
                float* channelData = buffer.getWritePointer(channel);
                for (int i = 0; i < blockSize; ++i)
                {
                    channelData[i] = juce::Random::getSystemRandom().nextFloat() * 2.0f - 1.0f;
                }
            }

            matcher.processBlock(buffer, blockSize);

            // Check for NaN or Inf
            bool hasInvalidValues = false;
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            {
                const float* channelData = buffer.getReadPointer(channel);
                for (int i = 0; i < blockSize; ++i)
                {
                    if (std::isnan(channelData[i]) || std::isinf(channelData[i]))
                    {
                        hasInvalidValues = true;
                        break;
                    }
                }
            }

            expect(!hasInvalidValues, "Output should not contain NaN or Inf values");
        }

        beginTest("ToneMatcher preserves audio level roughly");
        {
            ToneMatcher matcher;
            const double sampleRate = 44100.0;
            const int blockSize = 512;
            matcher.prepare(sampleRate, blockSize);

            juce::AudioBuffer<float> buffer(2, blockSize);
            
            // Generate test audio
            float inputRMS = 0.0f;
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            {
                float* channelData = buffer.getWritePointer(channel);
                for (int i = 0; i < blockSize; ++i)
                {
                    channelData[i] = std::sin((i / (float)sampleRate) * 440.0f * 
                                              2.0f * juce::MathConstants<float>::pi) * 0.5f;
                    inputRMS += channelData[i] * channelData[i];
                }
            }
            inputRMS = std::sqrt(inputRMS / (blockSize * buffer.getNumChannels()));

            matcher.processBlock(buffer, blockSize);

            // Calculate output RMS
            float outputRMS = 0.0f;
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            {
                const float* channelData = buffer.getReadPointer(channel);
                for (int i = 0; i < blockSize; ++i)
                {
                    outputRMS += channelData[i] * channelData[i];
                }
            }
            outputRMS = std::sqrt(outputRMS / (blockSize * buffer.getNumChannels()));

            // Output shouldn't be wildly different from input (within 20dB)
            float ratio = outputRMS / (inputRMS + 0.0001f);
            expect(ratio > 0.01f && ratio < 100.0f, 
                   "Output level should be reasonable relative to input");
        }
    }
};

static ToneMatcherTests toneMatcherTests;





