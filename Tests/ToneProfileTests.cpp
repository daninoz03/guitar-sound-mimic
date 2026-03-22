#include <JuceHeader.h>
#include "../Source/Audio/AudioEngine.h"

//==============================================================================
class ToneProfileTests : public juce::UnitTest
{
public:
    ToneProfileTests()
        : juce::UnitTest("ToneProfile", "Data")
    {
    }

    void runTest() override
    {
        beginTest("ToneProfile default construction");
        {
            ToneProfile profile;
            expect(!profile.isValid(), "Empty profile should not be valid");
            expect(profile.frequencyResponse.empty(), "Should have no frequency response");
            expect(profile.frequencies.empty(), "Should have no frequencies");
            expect(profile.totalSamplesAnalyzed == 0, "Should have zero samples");
        }

        beginTest("ToneProfile validity check");
        {
            ToneProfile profile;
            
            // Empty profile is invalid
            expect(!profile.isValid(), "Empty profile should be invalid");
            
            // Add frequency data but no samples - still invalid
            profile.frequencyResponse.push_back(1.0f);
            expect(!profile.isValid(), "Profile with no samples should be invalid");
            
            // Add samples - now valid
            profile.totalSamplesAnalyzed = 1000;
            expect(profile.isValid(), "Profile with data and samples should be valid");
        }

        beginTest("ToneProfile stores frequency response");
        {
            ToneProfile profile;
            
            const int numBins = 100;
            for (int i = 0; i < numBins; ++i)
            {
                profile.frequencyResponse.push_back(static_cast<float>(i) / numBins);
                profile.frequencies.push_back(static_cast<float>(i) * 20.0f);
            }
            
            expect(profile.frequencyResponse.size() == numBins, 
                   "Should store all frequency response data");
            expect(profile.frequencies.size() == numBins, 
                   "Should store all frequency labels");
        }

        beginTest("ToneProfile stores harmonic content");
        {
            ToneProfile profile;
            
            profile.harmonicContent = {1.0f, 0.5f, 0.3f, 0.2f, 0.1f};
            
            expect(profile.harmonicContent.size() == 5, 
                   "Should store harmonic content");
            expect(profile.harmonicContent[0] == 1.0f, 
                   "First harmonic should be 1.0");
        }

        beginTest("ToneProfile stores compression parameters");
        {
            ToneProfile profile;
            
            profile.compressionRatio = 4.0f;
            profile.compressionThreshold = 0.7f;
            profile.saturationAmount = 0.3f;
            
            expect(profile.compressionRatio == 4.0f, "Should store compression ratio");
            expect(profile.compressionThreshold == 0.7f, "Should store threshold");
            expect(profile.saturationAmount == 0.3f, "Should store saturation");
        }

        beginTest("ToneProfile stores EQ bands");
        {
            ToneProfile profile;
            
            ToneProfile::EQBand band1{100.0f, 2.0f, 1.0f};
            ToneProfile::EQBand band2{1000.0f, -3.0f, 0.7f};
            ToneProfile::EQBand band3{5000.0f, 1.5f, 1.2f};
            
            profile.eqBands.push_back(band1);
            profile.eqBands.push_back(band2);
            profile.eqBands.push_back(band3);
            
            expect(profile.eqBands.size() == 3, "Should store all EQ bands");
            expect(profile.eqBands[0].frequency == 100.0f, "Should store first band");
            expect(profile.eqBands[1].gain == -3.0f, "Should store gain correctly");
        }

        beginTest("ToneProfile stores sample rate and metadata");
        {
            ToneProfile profile;
            
            profile.sampleRate = 48000.0;
            profile.totalSamplesAnalyzed = 240000;
            profile.profileName = "Test Profile";
            
            expect(profile.sampleRate == 48000.0, "Should store sample rate");
            expect(profile.totalSamplesAnalyzed == 240000, "Should store sample count");
            expect(profile.profileName == "Test Profile", "Should store name");
        }

        beginTest("ToneProfile copy construction");
        {
            ToneProfile original;
            original.frequencyResponse = {1.0f, 0.8f, 0.6f, 0.4f};
            original.frequencies = {100.0f, 200.0f, 300.0f, 400.0f};
            original.compressionRatio = 3.0f;
            original.sampleRate = 44100.0;
            original.totalSamplesAnalyzed = 100000;
            original.profileName = "Original";
            
            ToneProfile copy = original;
            
            expect(copy.frequencyResponse == original.frequencyResponse, 
                   "Copy should have same frequency response");
            expect(copy.frequencies == original.frequencies, 
                   "Copy should have same frequencies");
            expect(copy.compressionRatio == original.compressionRatio, 
                   "Copy should have same compression ratio");
            expect(copy.sampleRate == original.sampleRate, 
                   "Copy should have same sample rate");
            expect(copy.totalSamplesAnalyzed == original.totalSamplesAnalyzed, 
                   "Copy should have same sample count");
            expect(copy.profileName == original.profileName, 
                   "Copy should have same name");
        }

        beginTest("ToneProfile realistic data");
        {
            // Create a profile with realistic guitar tone data
            ToneProfile profile;
            
            // Simulate frequency response (typical guitar emphasis in mids)
            for (int i = 0; i < 2048; ++i)
            {
                float freq = (i / 2048.0f) * 22050.0f; // Up to Nyquist
                float response = 1.0f;
                
                // Boost in mid frequencies (guitar fundamental range)
                if (freq > 200.0f && freq < 3000.0f)
                    response = 1.5f;
                
                // Roll off highs
                if (freq > 5000.0f)
                    response *= (10000.0f - freq) / 5000.0f;
                
                profile.frequencyResponse.push_back(response);
                profile.frequencies.push_back(freq);
            }
            
            // Realistic compression for electric guitar
            profile.compressionRatio = 3.5f;
            profile.compressionThreshold = 0.6f;
            
            // Some saturation
            profile.saturationAmount = 0.2f;
            
            // Metadata
            profile.sampleRate = 44100.0;
            profile.totalSamplesAnalyzed = 220500; // ~5 seconds
            profile.profileName = "Realistic Guitar Tone";
            
            expect(profile.isValid(), "Realistic profile should be valid");
            expect(profile.frequencyResponse.size() == 2048, 
                   "Should have full frequency response");
        }

        beginTest("ToneProfile validation edge cases");
        {
            // Test various validity edge cases
            ToneProfile profile1;
            profile1.totalSamplesAnalyzed = 1;
            expect(!profile1.isValid(), "Profile with 1 sample but no frequency data is invalid");
            
            ToneProfile profile2;
            profile2.frequencyResponse.push_back(1.0f);
            profile2.totalSamplesAnalyzed = 0;
            expect(!profile2.isValid(), "Profile with frequency data but 0 samples is invalid");
            
            ToneProfile profile3;
            profile3.frequencyResponse.push_back(1.0f);
            profile3.totalSamplesAnalyzed = 1;
            expect(profile3.isValid(), "Profile with minimal valid data should be valid");
        }
    }
};

static ToneProfileTests toneProfileTests;





