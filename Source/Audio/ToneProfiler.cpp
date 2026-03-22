#include "ToneProfiler.h"

ToneProfiler::ToneProfiler()
    : fft(fftOrder)
    , window(fftSize, juce::dsp::WindowingFunction<float>::hann)
{
    frequencyAnalyzer = std::make_unique<FrequencyAnalyzer>();
}

ToneProfiler::~ToneProfiler()
{
}

void ToneProfiler::prepare(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    expectedSamplesPerBlock = samplesPerBlock;
    frequencyAnalyzer->prepare(sampleRate, samplesPerBlock);
    reset();
}

void ToneProfiler::reset()
{
    totalSamplesAnalyzed = 0;
    frequencySnapshots.clear();
    peakLevels.clear();
    rmsLevels.clear();
    recordedSamples.clear();
    std::fill(fftData.begin(), fftData.end(), 0.0f);
    std::fill(sampleAccumulator.begin(), sampleAccumulator.end(), 0.0f);
    accumulatorFill = 0;
}

void ToneProfiler::processBlock(juce::AudioBuffer<float>& buffer, int numSamples)
{
    totalSamplesAnalyzed += numSamples;

    // Record raw mono audio for playback verification (up to 30 s)
    const int numChannels = buffer.getNumChannels();
    const int spaceLeft = MAX_RECORD_SAMPLES - static_cast<int>(recordedSamples.size());
    const int toRecord = std::min(numSamples, spaceLeft);
    for (int i = 0; i < toRecord; ++i)
    {
        float mono = 0.0f;
        for (int ch = 0; ch < numChannels; ++ch)
            mono += buffer.getSample(ch, i) / static_cast<float>(numChannels);
        recordedSamples.push_back(mono);
    }

    analyzeFrequencyContent(buffer);
    analyzeDynamics(buffer);
    analyzeHarmonics(buffer);
}

void ToneProfiler::analyzeFrequencyContent(const juce::AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    // Accumulate incoming samples (mixed to mono) until we have a full FFT frame.
    // Audio blocks are typically 128-512 samples — much smaller than fftSize —
    // so we must buffer across multiple calls before we can run an FFT.
    for (int i = 0; i < numSamples; ++i)
    {
        float mono = 0.0f;
        for (int ch = 0; ch < numChannels; ++ch)
            mono += buffer.getSample(ch, i) / static_cast<float>(numChannels);

        sampleAccumulator[static_cast<size_t>(accumulatorFill++)] = mono;

        if (accumulatorFill >= fftSize)
        {
            // Copy accumulated frame into fftData (second half stays zero for FFT)
            std::copy(sampleAccumulator.begin(), sampleAccumulator.end(), fftData.begin());
            std::fill(fftData.begin() + fftSize, fftData.end(), 0.0f);

            window.multiplyWithWindowingTable(fftData.data(), fftSize);
            fft.performFrequencyOnlyForwardTransform(fftData.data());

            float maxMagnitude = 0.0f;
            for (int j = 0; j < fftSize / 2; ++j)
                maxMagnitude = std::max(maxMagnitude, fftData[static_cast<size_t>(j)]);

            if (maxMagnitude > 0.0f)
            {
                std::vector<float> snapshot;
                snapshot.reserve(static_cast<size_t>(fftSize / 2));
                for (int j = 0; j < fftSize / 2; ++j)
                    snapshot.push_back(fftData[static_cast<size_t>(j)] / maxMagnitude);
                frequencySnapshots.push_back(std::move(snapshot));
            }

            accumulatorFill = 0;
        }
    }
}

void ToneProfiler::analyzeDynamics(const juce::AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();
    
    float peak = 0.0f;
    float sumSquares = 0.0f;
    
    for (int channel = 0; channel < numChannels; ++channel)
    {
        const float* channelData = buffer.getReadPointer(channel);
        for (int i = 0; i < numSamples; ++i)
        {
            float sample = std::abs(channelData[i]);
            peak = std::max(peak, sample);
            sumSquares += channelData[i] * channelData[i];
        }
    }
    
    peakLevels.push_back(peak);
    rmsLevels.push_back(std::sqrt(sumSquares / (numSamples * numChannels)));
}

void ToneProfiler::analyzeHarmonics(const juce::AudioBuffer<float>& buffer)
{
    // Harmonic analysis using frequency analyzer
    frequencyAnalyzer->analyzeBuffer(buffer);
}

std::unique_ptr<ToneProfile> ToneProfiler::generateProfile()
{
    if (totalSamplesAnalyzed < MIN_SAMPLES_FOR_PROFILE)
    {
        juce::Logger::writeToLog("Insufficient samples for profile: " + 
                                juce::String(totalSamplesAnalyzed) + " / " + 
                                juce::String(MIN_SAMPLES_FOR_PROFILE));
        return nullptr;
    }
    
    auto profile = std::make_unique<ToneProfile>();
    profile->sampleRate = currentSampleRate;
    profile->totalSamplesAnalyzed = totalSamplesAnalyzed;
    
    // Average frequency snapshots to get overall frequency response
    if (!frequencySnapshots.empty())
    {
        const int numBins = static_cast<int>(frequencySnapshots[0].size());
        profile->frequencyResponse.resize(numBins, 0.0f);
        
        for (const auto& snapshot : frequencySnapshots)
        {
            for (int i = 0; i < numBins; ++i)
            {
                profile->frequencyResponse[i] += snapshot[i];
            }
        }
        
        // Average
        for (int i = 0; i < numBins; ++i)
        {
            profile->frequencyResponse[i] /= frequencySnapshots.size();
        }
        
        // Generate frequency labels
        profile->frequencies.reserve(numBins);
        for (int i = 0; i < numBins; ++i)
        {
            float frequency = (i * currentSampleRate) / fftSize;
            profile->frequencies.push_back(frequency);
        }
    }
    
    // Analyze dynamics to estimate compression characteristics
    if (!peakLevels.empty() && !rmsLevels.empty())
    {
        float avgPeak = 0.0f;
        float avgRms = 0.0f;
        
        for (float peak : peakLevels)
            avgPeak += peak;
        for (float rms : rmsLevels)
            avgRms += rms;
        
        avgPeak /= peakLevels.size();
        avgRms /= rmsLevels.size();
        
        // Simple heuristic: if peak/RMS ratio is low, signal is compressed
        float peakToRms = avgPeak / (avgRms + 0.0001f);
        profile->compressionRatio = juce::jmap(peakToRms, 2.0f, 10.0f, 4.0f, 1.0f);
        profile->compressionRatio = juce::jlimit(1.0f, 10.0f, profile->compressionRatio);
        profile->compressionThreshold = avgRms * 0.7f;
    }
    
    // Extract harmonic content (simplified - using frequency response peaks)
    profile->harmonicContent = frequencyAnalyzer->getHarmonicContent();
    
    // TODO: More sophisticated EQ band extraction
    // For now, we'll apply the frequency response directly
    
    juce::Logger::writeToLog("Profile generated successfully with " + 
                            juce::String(profile->frequencyResponse.size()) + " frequency bins");
    
    return profile;
}





