#pragma once

#include <JuceHeader.h>
#include "Audio/AudioEngine.h"
#include "Audio/ToneProfiler.h"
#include "Audio/ToneMatcher.h"
#include "Audio/SamplePlayer.h"
#include "Audio/AudioPlayback.h"

//==============================================================================
class MainComponent : public juce::AudioAppComponent,
                      public juce::Timer
{
public:
    MainComponent();
    ~MainComponent() override;

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void paint   (juce::Graphics& g) override;
    void resized () override;

private:
    void timerCallback() override;

    // Button handlers
    void startProfiling();
    void applyProfile();
    void bypassProcessing();
    void listenToSampleDemo();
    void playRecording();
    void playGuitarOnly();

    // Guitar frequency isolation
    void applyGuitarBandpass();   // C++ bandpass filter, runs instantly
    void runDemucsAsync();        // reserved for future Demucs integration
    void checkDemucsProgress();
    bool saveRecordingToWav (const juce::File& dest);
    void loadGuitarOnlyWav  (const juce::File& src);

    // ---- processing modes -----------------------------------------------
    enum class ProcessingMode
    {
        Bypass,
        Profiling,
        Matching,
        PreviewSample,
        PlayingRecording,
        PlayingGuitarOnly
    };

    ProcessingMode currentMode = ProcessingMode::Bypass;

    // ---- audio objects --------------------------------------------------
    std::unique_ptr<ToneProfiler>  profiler;
    std::unique_ptr<ToneMatcher>   matcher;
    std::unique_ptr<ToneProfile>   currentProfile;
    std::unique_ptr<SamplePlayer>  samplePlayer;
    std::unique_ptr<AudioPlayback> recordingPlayback;
    std::unique_ptr<AudioPlayback> guitarOnlyPlayback;

    // Demucs subprocess
    std::unique_ptr<juce::ChildProcess> demucsProcess;
    juce::File tempRecordingFile;
    juce::File tempGuitarFile;

    // ---- UI components --------------------------------------------------
    // Row 1 — profiling controls
    juce::TextButton profileButton;
    juce::TextButton applyButton;
    juce::TextButton bypassButton;

    // Row 2 — listening / diagnostic tools
    juce::TextButton listenDemoButton;      // renamed "Listen to Sample Applied Demo"
    juce::TextButton playRecordingButton;   // play back raw mic recording
    juce::TextButton guitarOnlyButton;      // play Demucs-separated guitar track

    // Status + levels
    juce::Label statusLabel;
    juce::Label inputLevelLabel;
    juce::Label outputLevelLabel;

    // Audio level monitoring (written on audio thread, read on message thread)
    std::atomic<float> inputLevel  { 0.0f };
    std::atomic<float> outputLevel { 0.0f };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
