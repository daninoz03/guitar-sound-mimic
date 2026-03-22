#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    profiler         = std::make_unique<ToneProfiler>();
    matcher          = std::make_unique<ToneMatcher>();
    samplePlayer     = std::make_unique<SamplePlayer>();
    recordingPlayback = std::make_unique<AudioPlayback>();
    guitarOnlyPlayback = std::make_unique<AudioPlayback>();

    // ---- Row 1 buttons --------------------------------------------------
    addAndMakeVisible (profileButton);
    profileButton.setButtonText ("Start Profiling");
    profileButton.onClick = [this] { startProfiling(); };

    addAndMakeVisible (applyButton);
    applyButton.setButtonText ("Apply Profile");
    applyButton.onClick = [this] { applyProfile(); };
    applyButton.setEnabled (false);

    addAndMakeVisible (bypassButton);
    bypassButton.setButtonText ("Bypass");
    bypassButton.onClick = [this] { bypassProcessing(); };

    // ---- Row 2 buttons --------------------------------------------------
    addAndMakeVisible (listenDemoButton);
    listenDemoButton.setButtonText ("Listen to Sample Applied Demo");
    listenDemoButton.onClick = [this] { listenToSampleDemo(); };
    listenDemoButton.setEnabled (false);

    addAndMakeVisible (playRecordingButton);
    playRecordingButton.setButtonText ("Play Back Recording");
    playRecordingButton.onClick = [this] { playRecording(); };
    playRecordingButton.setEnabled (false);

    addAndMakeVisible (guitarOnlyButton);
    guitarOnlyButton.setButtonText ("Isolate Guitar (AI / Freq Filter)");
    guitarOnlyButton.onClick = [this] { playGuitarOnly(); };
    guitarOnlyButton.setEnabled (false);

    // ---- Status + level labels ------------------------------------------
    addAndMakeVisible (statusLabel);
    statusLabel.setText ("Requesting microphone access...", juce::dontSendNotification);
    statusLabel.setJustificationType (juce::Justification::centred);
    statusLabel.setFont (juce::Font (18.0f, juce::Font::bold));

    addAndMakeVisible (inputLevelLabel);
    inputLevelLabel.setText ("Input:  0.0 dB", juce::dontSendNotification);

    addAndMakeVisible (outputLevelLabel);
    outputLevelLabel.setText ("Output: 0.0 dB", juce::dontSendNotification);

    setSize (880, 680);

    juce::RuntimePermissions::request (
        juce::RuntimePermissions::recordAudio,
        [this] (bool granted)
        {
            juce::MessageManager::callAsync ([this, granted]()
            {
                if (granted)
                {
                    setAudioChannels (2, 2);
                    statusLabel.setText ("Ready — microphone active", juce::dontSendNotification);
                }
                else
                {
                    statusLabel.setText ("Microphone access denied — check System Settings › Privacy",
                                         juce::dontSendNotification);
                    profileButton.setEnabled (false);
                }
            });
        });

    startTimer (33);
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    profiler->prepare (sampleRate, samplesPerBlockExpected);
    matcher->prepare  (sampleRate, samplesPerBlockExpected);
    samplePlayer->prepare (sampleRate, samplesPerBlockExpected);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    auto* buffer     = bufferToFill.buffer;
    const int numSamples = bufferToFill.numSamples;

    // Measure input level before we might clear it
    {
        float sumSq = 0.0f;
        const float* ch0 = buffer->getReadPointer (0);
        for (int i = 0; i < numSamples; ++i) sumSq += ch0[i] * ch0[i];
        inputLevel.store (juce::Decibels::gainToDecibels (
                              std::sqrt (sumSq / numSamples), -100.0f));
    }

    switch (currentMode)
    {
        case ProcessingMode::Profiling:
            profiler->processBlock (*buffer, numSamples);
            bufferToFill.clearActiveBufferRegion(); // no mic feedback
            break;

        case ProcessingMode::Matching:
            if (currentProfile != nullptr)
                matcher->processBlock (*buffer, numSamples);
            break;

        case ProcessingMode::PreviewSample:
            buffer->clear();
            samplePlayer->fillBuffer (*buffer, numSamples);
            matcher->processBlock (*buffer, numSamples);
            break;

        case ProcessingMode::PlayingRecording:
            buffer->clear();
            recordingPlayback->fillBuffer (*buffer, numSamples);
            break;

        case ProcessingMode::PlayingGuitarOnly:
            buffer->clear();
            guitarOnlyPlayback->fillBuffer (*buffer, numSamples);
            break;

        case ProcessingMode::Bypass:
        default:
            bufferToFill.clearActiveBufferRegion(); // silence — no feedback
            break;
    }

    // Measure output level
    {
        float sumSq = 0.0f;
        const float* ch0 = buffer->getReadPointer (0);
        for (int i = 0; i < numSamples; ++i) sumSq += ch0[i] * ch0[i];
        outputLevel.store (juce::Decibels::gainToDecibels (
                               std::sqrt (sumSq / numSamples), -100.0f));
    }
}

void MainComponent::releaseResources()
{
    profiler->reset();
    matcher->reset();
    samplePlayer->reset();
    recordingPlayback->stop();
    guitarOnlyPlayback->stop();
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // Title
    g.setColour (juce::Colours::white);
    g.setFont (28.0f);
    g.drawText ("Guitar Sound Mimic", getLocalBounds().removeFromTop (50),
                juce::Justification::centred, true);

    // Instructions box
    const int boxX = 20, boxY = 58, boxW = getWidth() - 40, boxH = 185;
    g.setColour (juce::Colour (0xff2a2a3a));
    g.fillRoundedRectangle (static_cast<float>(boxX), static_cast<float>(boxY),
                            static_cast<float>(boxW), static_cast<float>(boxH), 6.0f);
    g.setColour (juce::Colours::lightgrey);
    g.drawRoundedRectangle (static_cast<float>(boxX), static_cast<float>(boxY),
                            static_cast<float>(boxW), static_cast<float>(boxH), 6.0f, 1.0f);

    g.setColour (juce::Colours::lightyellow);
    g.setFont (13.5f);
    const juce::String instructions =
        "HOW TO USE THIS APP\n\n"
        "1.  PROFILE A TONE   — Click \"Start Profiling\", play or sing the reference sound near the mic "
        "(or connect an audio interface and play a recording through it), then click \"Stop Profiling\".\n\n"
        "2.  VERIFY RECORDING  — Click \"Play Back Recording\" to hear exactly what the mic captured.  "
        "If you can't hear the tone clearly in the playback, re-profile with a clearer source.\n\n"
        "3.  ISOLATE GUITAR    — Click \"Isolate Guitar\" to attempt AI source separation via Demucs "
        "(install with: pip3 install demucs torch torchaudio).  If Demucs is not installed, a frequency "
        "filter is applied instead — note this cannot remove instruments sharing the guitar frequency range.\n\n"
        "4.  HEAR THE EFFECT   — Click \"Listen to Sample Applied Demo\" to hear a built-in guitar riff "
        "with your captured tone applied via EQ.\n\n"
        "5.  GO LIVE           — Connect your guitar, click \"Apply Profile\", and play — your signal "
        "will be shaped in real-time to match the profiled tone.";

    g.drawFittedText (instructions,
                      boxX + 10, boxY + 8, boxW - 20, boxH - 12,
                      juce::Justification::topLeft, 20);

    // Level meters
    const int meterY = boxY + boxH + 48;
    g.setColour (juce::Colours::darkgrey);
    g.fillRect (20, meterY, 380, 16);
    g.fillRect (20, meterY + 22, 380, 16);

    float inNorm  = juce::jmap (inputLevel.load(),  -60.0f, 0.0f, 0.0f, 1.0f);
    float outNorm = juce::jmap (outputLevel.load(), -60.0f, 0.0f, 0.0f, 1.0f);
    g.setColour (juce::Colours::limegreen);
    g.fillRect (20, meterY, static_cast<int>(380 * inNorm), 16);
    g.setColour (juce::Colours::dodgerblue);
    g.fillRect (20, meterY + 22, static_cast<int>(380 * outNorm), 16);
}

void MainComponent::resized()
{
    const int instrBoxH = 185;
    const int pad = 20;

    auto area = getLocalBounds().reduced (pad);
    area.removeFromTop (50);              // title
    area.removeFromTop (instrBoxH + 10); // instructions box

    // Status label
    statusLabel.setBounds (area.removeFromTop (32));
    area.removeFromTop (8);

    // Level meters (drawn in paint) + labels
    area.removeFromTop (50); // space for meter bars
    inputLevelLabel.setBounds  (area.removeFromTop (20));
    outputLevelLabel.setBounds (area.removeFromTop (20));
    area.removeFromTop (14);

    // Row 1: profiling controls
    {
        auto row = area.removeFromTop (46);
        const int sp = 8;
        const int bw = (row.getWidth() - sp * 2) / 3;
        profileButton.setBounds (row.removeFromLeft (bw));  row.removeFromLeft (sp);
        applyButton  .setBounds (row.removeFromLeft (bw));  row.removeFromLeft (sp);
        bypassButton .setBounds (row);
    }

    area.removeFromTop (10);

    // Row 2: diagnostic / listening tools
    {
        auto row = area.removeFromTop (46);
        const int sp = 8;
        const int bw = (row.getWidth() - sp * 2) / 3;
        listenDemoButton    .setBounds (row.removeFromLeft (bw));  row.removeFromLeft (sp);
        playRecordingButton .setBounds (row.removeFromLeft (bw));  row.removeFromLeft (sp);
        guitarOnlyButton    .setBounds (row);
    }
}

//==============================================================================
void MainComponent::timerCallback()
{
    inputLevelLabel .setText ("Input:  " + juce::String (inputLevel .load(), 1) + " dB",
                              juce::dontSendNotification);
    outputLevelLabel.setText ("Output: " + juce::String (outputLevel.load(), 1) + " dB",
                              juce::dontSendNotification);

    // Sample demo finished
    if (currentMode == ProcessingMode::PreviewSample && !samplePlayer->isPlaying())
    {
        currentMode = ProcessingMode::Bypass;
        listenDemoButton.setEnabled (true);
        statusLabel.setText ("Demo finished — profile ready", juce::dontSendNotification);
    }

    // Recording playback finished
    if (currentMode == ProcessingMode::PlayingRecording && !recordingPlayback->isPlaying())
    {
        currentMode = ProcessingMode::Bypass;
        playRecordingButton.setEnabled (true);
        statusLabel.setText ("Recording playback finished", juce::dontSendNotification);
    }

    // Guitar-only playback finished
    if (currentMode == ProcessingMode::PlayingGuitarOnly && !guitarOnlyPlayback->isPlaying())
    {
        currentMode = ProcessingMode::Bypass;
        guitarOnlyButton.setEnabled (true);
        statusLabel.setText ("Guitar-only playback finished", juce::dontSendNotification);
    }

    // Poll Demucs subprocess
    checkDemucsProgress();

    repaint();
}

//==============================================================================
void MainComponent::startProfiling()
{
    if (currentMode == ProcessingMode::Profiling)
    {
        // Stop and generate
        currentMode    = ProcessingMode::Bypass;
        currentProfile = profiler->generateProfile();

        if (currentProfile != nullptr)
        {
            // Store raw recording for playback
            recordingPlayback->loadSamples (profiler->getRecordedSamples(),
                                            profiler->getRecordedSampleRate());

            statusLabel.setText ("Profile generated! Use the buttons below to verify.",
                                 juce::dontSendNotification);
            profileButton      .setButtonText ("Start Profiling");
            applyButton        .setEnabled (true);
            listenDemoButton   .setEnabled (true);
            playRecordingButton.setEnabled (recordingPlayback->hasSamples());
            guitarOnlyButton   .setEnabled (recordingPlayback->hasSamples());
        }
        else
        {
            statusLabel.setText ("Not enough audio — profile 3+ seconds and try again",
                                 juce::dontSendNotification);
            profileButton.setButtonText ("Start Profiling");
        }
    }
    else
    {
        profiler->reset();
        currentMode = ProcessingMode::Profiling;
        statusLabel.setText ("Profiling… play reference audio near the mic now!",
                             juce::dontSendNotification);
        profileButton.setButtonText ("Stop Profiling");
        applyButton        .setEnabled (false);
        listenDemoButton   .setEnabled (false);
        playRecordingButton.setEnabled (false);
        guitarOnlyButton   .setEnabled (false);
    }
}

void MainComponent::applyProfile()
{
    if (currentProfile != nullptr)
    {
        matcher->setProfile (currentProfile);
        currentMode = ProcessingMode::Matching;
        statusLabel.setText ("Profile applied — play your guitar!", juce::dontSendNotification);
    }
}

void MainComponent::bypassProcessing()
{
    currentMode = ProcessingMode::Bypass;
    statusLabel.setText ("Bypassed", juce::dontSendNotification);
}

void MainComponent::listenToSampleDemo()
{
    if (currentProfile == nullptr) return;

    matcher->setProfile (currentProfile);
    samplePlayer->startPlaying();
    currentMode = ProcessingMode::PreviewSample;
    listenDemoButton.setEnabled (false);
    statusLabel.setText ("Playing demo riff with your tone applied…", juce::dontSendNotification);
}

void MainComponent::playRecording()
{
    if (!recordingPlayback->hasSamples()) return;

    recordingPlayback->startPlaying();
    currentMode = ProcessingMode::PlayingRecording;
    playRecordingButton.setEnabled (false);
    statusLabel.setText ("Playing back raw mic recording…", juce::dontSendNotification);
}

void MainComponent::playGuitarOnly()
{
    if (! recordingPlayback->hasSamples())
    {
        statusLabel.setText ("No recording yet — profile something first", juce::dontSendNotification);
        return;
    }

    if (guitarOnlyPlayback->hasSamples())
    {
        // Already processed — just play it again
        guitarOnlyPlayback->startPlaying();
        currentMode = ProcessingMode::PlayingGuitarOnly;
        guitarOnlyButton.setEnabled (false);
        statusLabel.setText ("Playing isolated guitar…", juce::dontSendNotification);
        return;
    }

    // Prefer Demucs AI separation; fall back to frequency filter if unavailable
    runDemucsAsync();
}

void MainComponent::applyGuitarBandpass()
{
    const auto& src = profiler->getRecordedSamples();
    if (src.empty()) return;

    const double sr = profiler->getRecordedSampleRate();

    // ---- biquad coefficient helpers ------------------------------------
    struct Biquad
    {
        float b0, b1, b2, a1, a2;
        float x1 = 0, x2 = 0, y1 = 0, y2 = 0;

        float process (float x) noexcept
        {
            float y = b0*x + b1*x1 + b2*x2 - a1*y1 - a2*y2;
            if (! std::isfinite (y)) y = 0.0f;
            x2 = x1; x1 = x;
            y2 = y1; y1 = y;
            return y;
        }

        static Biquad makeHighPass (double sampleRate, double freq)
        {
            const double w0    = 2.0 * juce::MathConstants<double>::pi * freq / sampleRate;
            const double alpha = std::sin (w0) / (2.0 * 0.7071); // Butterworth Q
            const double c     = std::cos (w0);
            const double a0inv = 1.0 / (1.0 + alpha);
            Biquad f;
            f.b0 = static_cast<float> ( (1.0 + c) * 0.5  * a0inv);
            f.b1 = static_cast<float> (-(1.0 + c)         * a0inv);
            f.b2 = static_cast<float> ( (1.0 + c) * 0.5  * a0inv);
            f.a1 = static_cast<float> (-2.0 * c            * a0inv);
            f.a2 = static_cast<float> ( (1.0 - alpha)      * a0inv);
            return f;
        }

        static Biquad makeLowPass (double sampleRate, double freq)
        {
            const double w0    = 2.0 * juce::MathConstants<double>::pi * freq / sampleRate;
            const double alpha = std::sin (w0) / (2.0 * 0.7071);
            const double c     = std::cos (w0);
            const double a0inv = 1.0 / (1.0 + alpha);
            Biquad f;
            f.b0 = static_cast<float> ((1.0 - c) * 0.5  * a0inv);
            f.b1 = static_cast<float> ( (1.0 - c)        * a0inv);
            f.b2 = static_cast<float> ((1.0 - c) * 0.5  * a0inv);
            f.a1 = static_cast<float> (-2.0 * c           * a0inv);
            f.a2 = static_cast<float> ( (1.0 - alpha)     * a0inv);
            return f;
        }
    };

    // Two-stage bandpass: HP at 80 Hz then LP at 5 kHz (applied twice each
    // for a steeper 4th-order roll-off)
    auto hp1 = Biquad::makeHighPass (sr, 80.0);
    auto hp2 = Biquad::makeHighPass (sr, 80.0);
    auto lp1 = Biquad::makeLowPass  (sr, 5000.0);
    auto lp2 = Biquad::makeLowPass  (sr, 5000.0);

    std::vector<float> out;
    out.reserve (src.size());

    for (float s : src)
    {
        s = hp1.process (s);
        s = hp2.process (s);
        s = lp1.process (s);
        s = lp2.process (s);
        out.push_back (s);
    }

    guitarOnlyPlayback->loadSamples (out, sr);
    guitarOnlyPlayback->startPlaying();
    currentMode = ProcessingMode::PlayingGuitarOnly;
    guitarOnlyButton.setEnabled (false);
    statusLabel.setText ("Playing guitar frequency band (80 Hz – 5 kHz)…",
                         juce::dontSendNotification);
}

void MainComponent::runDemucsAsync()
{
    // Save the profiling recording to a temp WAV so the Python script can read it
    tempRecordingFile = juce::File::getSpecialLocation (juce::File::tempDirectory)
                            .getChildFile ("gsm_recording.wav");
    tempGuitarFile    = juce::File::getSpecialLocation (juce::File::tempDirectory)
                            .getChildFile ("gsm_guitar_only.wav");

    if (! saveRecordingToWav (tempRecordingFile))
    {
        statusLabel.setText ("Could not write temp recording — falling back to frequency filter",
                             juce::dontSendNotification);
        applyGuitarBandpass();
        return;
    }

    // Walk up from the app to find the project's tools/ directory
    juce::File script;
    {
        juce::File dir = juce::File::getSpecialLocation (juce::File::currentApplicationFile);
        for (int i = 0; i < 12; ++i)
        {
            juce::File candidate = dir.getChildFile ("tools/separate_guitar.py");
            if (candidate.existsAsFile()) { script = candidate; break; }
            juce::File parent = dir.getParentDirectory();
            if (parent == dir) break;
            dir = parent;
        }
    }

    if (! script.existsAsFile())
    {
        statusLabel.setText ("Demucs script not found — falling back to frequency filter",
                             juce::dontSendNotification);
        applyGuitarBandpass();
        return;
    }

    // Run via a bash login shell so pyenv/homebrew python3 is on the PATH
    // (macOS GUI apps don't inherit the user's shell PATH)
    const juce::String shellCmd = "python3 "
        + script.getFullPathName().quoted()
        + " " + tempRecordingFile.getFullPathName().quoted()
        + " " + tempGuitarFile.getFullPathName().quoted();

    demucsProcess = std::make_unique<juce::ChildProcess>();
    if (demucsProcess->start (juce::StringArray { "/bin/bash", "-l", "-c", shellCmd }))
    {
        guitarOnlyButton.setEnabled (false);
        statusLabel.setText ("Running Demucs AI separation… (30–60 s) — please wait",
                             juce::dontSendNotification);
    }
    else
    {
        demucsProcess.reset();
        statusLabel.setText ("Could not start Python — falling back to frequency filter",
                             juce::dontSendNotification);
        applyGuitarBandpass();
    }
}

void MainComponent::checkDemucsProgress()
{
    if (demucsProcess == nullptr) return;

    if (demucsProcess->isRunning()) return;

    const int exitCode = demucsProcess->getExitCode();
    demucsProcess.reset();

    if (exitCode == 0 && tempGuitarFile.existsAsFile())
    {
        // Demucs succeeded — load the AI-separated guitar track
        loadGuitarOnlyWav (tempGuitarFile);
    }
    else
    {
        // Demucs not installed or failed — fall back to the frequency filter
        // and tell the user how to get the real thing
        statusLabel.setText ("Demucs not installed (exit " + juce::String (exitCode) + ") "
                             "— using frequency filter instead.  "
                             "For true isolation: pip3 install demucs torch torchaudio",
                             juce::dontSendNotification);
        applyGuitarBandpass();
    }
}

bool MainComponent::saveRecordingToWav (const juce::File& dest)
{
    dest.deleteFile();

    juce::WavAudioFormat wavFmt;
    auto stream = std::unique_ptr<juce::FileOutputStream> (dest.createOutputStream());
    if (stream == nullptr) return false;

    const auto& samples    = profiler->getRecordedSamples();
    const double sr        = profiler->getRecordedSampleRate();
    const int    numFrames = static_cast<int> (samples.size());

    auto* writer = wavFmt.createWriterFor (stream.get(), sr, 1, 16, {}, 0);
    if (writer == nullptr) return false;

    stream.release(); // writer owns stream now
    std::unique_ptr<juce::AudioFormatWriter> writerOwner (writer);

    const float* ptr = samples.data();
    return writerOwner->writeFromFloatArrays (&ptr, 1, numFrames);
}

void MainComponent::loadGuitarOnlyWav (const juce::File& src)
{
    juce::AudioFormatManager fmtMgr;
    fmtMgr.registerBasicFormats();

    std::unique_ptr<juce::AudioFormatReader> reader (fmtMgr.createReaderFor (src));
    if (reader == nullptr)
    {
        statusLabel.setText ("Could not load guitar-only WAV", juce::dontSendNotification);
        return;
    }

    const int numSamples = static_cast<int> (reader->lengthInSamples);
    juce::AudioBuffer<float> buf (1, numSamples);
    reader->read (&buf, 0, numSamples, 0, true, false);

    std::vector<float> data (static_cast<size_t> (numSamples));
    const float* src_ptr = buf.getReadPointer (0);
    std::copy (src_ptr, src_ptr + numSamples, data.begin());

    guitarOnlyPlayback->loadSamples (data, reader->sampleRate);

    statusLabel.setText ("Guitar isolated! Click \"Play Guitar Only\" to listen.",
                         juce::dontSendNotification);
    guitarOnlyButton.setEnabled (true);
}
