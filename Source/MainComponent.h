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
    // Custom LookAndFeel — dark audio-tool aesthetic
    struct GuitarLookAndFeel : public juce::LookAndFeel_V4
    {
        GuitarLookAndFeel()
        {
            setColour (juce::ResizableWindow::backgroundColourId, juce::Colour (0xff0b0b14));
        }

        void drawButtonBackground (juce::Graphics& g, juce::Button& btn,
                                   const juce::Colour&, bool isHighlighted, bool isDown) override
        {
            const auto bounds = btn.getLocalBounds().toFloat().reduced (0.5f);
            juce::Colour fill, border;

            if (! btn.isEnabled())
            {
                fill   = juce::Colour (0xff111120);
                border = juce::Colour (0xff1e1e2e);
            }
            else if (isDown)
            {
                fill   = juce::Colour (0xfff5a623);
                border = juce::Colour (0xffffc04a);
            }
            else if (isHighlighted)
            {
                fill   = juce::Colour (0xff252540);
                border = juce::Colour (0xfff5a623);
            }
            else
            {
                fill   = juce::Colour (0xff1a1a2c);
                border = juce::Colour (0xff3a3a5a);
            }

            g.setColour (fill);
            g.fillRoundedRectangle (bounds, 6.0f);
            g.setColour (border);
            g.drawRoundedRectangle (bounds, 6.0f, 1.5f);
        }

        void drawButtonText (juce::Graphics& g, juce::TextButton& btn,
                             bool /*isHighlighted*/, bool isDown) override
        {
            juce::Colour textCol;
            if (! btn.isEnabled())
                textCol = juce::Colour (0xff383852);
            else if (isDown)
                textCol = juce::Colour (0xff0b0b14);
            else
                textCol = juce::Colour (0xffe8e8f2);

            g.setColour (textCol);
            g.setFont (juce::Font (12.5f, juce::Font::bold));
            g.drawFittedText (btn.getButtonText(), btn.getLocalBounds(),
                              juce::Justification::centred, 2);
        }
    };
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

    GuitarLookAndFeel lf;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
