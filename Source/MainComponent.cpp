#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    profiler         = std::make_unique<ToneProfiler>();
    matcher          = std::make_unique<ToneMatcher>();
    samplePlayer     = std::make_unique<SamplePlayer>();
    recordingPlayback = std::make_unique<AudioPlayback>();
    guitarOnlyPlayback = std::make_unique<AudioPlayback>();

    // Apply custom look and feel to all buttons
    for (auto* btn : { &profileButton, &applyButton, &bypassButton,
                       &listenDemoButton, &playRecordingButton, &guitarOnlyButton })
        btn->setLookAndFeel (&lf);

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

    // ---- Status label ---------------------------------------------------
    addAndMakeVisible (statusLabel);
    statusLabel.setText ("Requesting microphone access...", juce::dontSendNotification);
    statusLabel.setJustificationType (juce::Justification::centred);
    statusLabel.setFont (juce::Font (14.0f, juce::Font::bold));
    statusLabel.setColour (juce::Label::textColourId, juce::Colour (0xffe8e8f2));

    // Level labels are drawn manually in paint() using atomic values
    inputLevelLabel .setVisible (false);
    outputLevelLabel.setVisible (false);

    setSize (900, 680);

    juce::RuntimePermissions::request (
        juce::RuntimePermissions::recordAudio,
        [this] (bool granted)
        {
            juce::MessageManager::callAsync ([this, granted]()
            {
                if (granted)
                {
                    setAudioChannels (2, 2);
                    statusLabel.setText ("Ready - microphone active", juce::dontSendNotification);
                }
                else
                {
                    statusLabel.setText ("Microphone access denied - check System Settings > Privacy",
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
    const int W = getWidth();

    // =========================================================
    // Background
    // =========================================================
    g.fillAll (juce::Colour (0xff0b0b14));

    // Soft radial glow at top centre
    juce::ColourGradient topGlow (juce::Colour (0xff1c1c38), W * 0.5f, 0.0f,
                                  juce::Colour (0xff0b0b14), W * 0.5f, 180.0f, true);
    g.setGradientFill (topGlow);
    g.fillRect (0, 0, W, 180);

    // =========================================================
    // Title bar  (y = 0 .. 65)
    // =========================================================
    g.setColour (juce::Colour (0xfff5a623));
    g.setFont (juce::Font (30.0f, juce::Font::bold));
    g.drawText ("GUITAR SOUND MIMIC", 0, 8, W, 36, juce::Justification::centred);

    g.setColour (juce::Colour (0xff7070a0));
    g.setFont (juce::Font (12.5f));
    g.drawText ("Real-time tone profiling & matching", 0, 42, W, 18, juce::Justification::centred);

    // Amber divider line
    g.setColour (juce::Colour (0xfff5a623).withAlpha (0.35f));
    g.drawLine (W * 0.22f, 63.0f, W * 0.78f, 63.0f, 1.0f);

    // =========================================================
    // Instructions box  (y = 67 .. 67+kInstrH)
    // =========================================================
    const int bx = 20, by = 67, bw = W - 40, bh = 265;

    // Box background with subtle gradient
    juce::ColourGradient boxGrad (juce::Colour (0xff13132a), (float)bx, (float)by,
                                  juce::Colour (0xff0e0e1e), (float)bx, (float)(by + bh), false);
    g.setGradientFill (boxGrad);
    g.fillRoundedRectangle ((float)bx, (float)by, (float)bw, (float)bh, 8.0f);
    g.setColour (juce::Colour (0xff2e2e50));
    g.drawRoundedRectangle ((float)bx + 0.5f, (float)by + 0.5f,
                            (float)bw - 1.0f, (float)bh - 1.0f, 8.0f, 1.0f);

    // "HOW TO USE" header inside box
    g.setColour (juce::Colour (0xfff5a623));
    g.setFont (juce::Font (10.5f, juce::Font::bold));
    g.drawText ("HOW TO USE", bx + 14, by + 9, 90, 14, juce::Justification::left);
    g.setColour (juce::Colour (0xff2e2e50));
    g.drawLine ((float)(bx + 100), (float)(by + 16), (float)(bx + bw - 14), (float)(by + 16), 1.0f);

    // Instruction steps — em-dashes replaced with colons for reliable rendering
    g.setColour (juce::Colour (0xffbbbbd8));
    g.setFont (juce::Font (12.5f));
    const juce::String instructions =
        "1.  PROFILE A TONE: Click \"Start Profiling\", play reference audio near the mic "
        "(or connect an audio interface), then click \"Stop Profiling\".\n\n"
        "2.  VERIFY RECORDING: Click \"Play Back Recording\" to hear exactly what was captured. "
        "Re-profile if the audio is unclear.\n\n"
        "3.  ISOLATE GUITAR: Click \"Isolate Guitar\" to strip other instruments. Uses Demucs AI "
        "if installed (pip3 install demucs torch torchaudio), otherwise applies a bandpass filter.\n\n"
        "4.  HEAR THE EFFECT: Click \"Listen to Sample Applied Demo\" to hear a built-in guitar "
        "riff with your profiled tone applied via EQ.\n\n"
        "5.  GO LIVE: Connect your guitar, click \"Apply Profile\", and play in real-time to match "
        "the profiled tone.";

    g.drawFittedText (instructions,
                      bx + 14, by + 26, bw - 28, bh - 34,
                      juce::Justification::topLeft, 25);

    // =========================================================
    // Level meters section  (y = 395 .. 453)
    // =========================================================
    const int mx = 20, my = 395, mw = W - 40;
    const int barH = 18, barGap = 20;
    const int labelW = 72;

    // Section label
    g.setColour (juce::Colour (0xff5050a0));
    g.setFont (juce::Font (10.0f, juce::Font::bold));
    g.drawText ("LEVELS", mx, my, 60, 12, juce::Justification::left);
    g.setColour (juce::Colour (0xff1e1e38));
    g.drawLine ((float)(mx + 52), (float)(my + 6), (float)(mx + mw), (float)(my + 6), 1.0f);

    // --- Input bar ---
    const int inBarY  = my + 16;
    const int outBarY = inBarY + barH + barGap;
    const int barX    = mx + labelW;
    const int barW    = mw - labelW;

    g.setColour (juce::Colour (0xff0e0e1e));
    g.fillRoundedRectangle ((float)barX, (float)inBarY, (float)barW, (float)barH, 3.0f);

    float inNorm  = juce::jlimit (0.0f, 1.0f, juce::jmap (inputLevel.load(),  -60.0f, 0.0f, 0.0f, 1.0f));
    float outNorm = juce::jlimit (0.0f, 1.0f, juce::jmap (outputLevel.load(), -60.0f, 0.0f, 0.0f, 1.0f));

    if (inNorm > 0.0f)
    {
        juce::ColourGradient inGrad (juce::Colour (0xff1eaa4a), (float)barX, 0.0f,
                                     juce::Colour (0xffe8c020), (float)(barX + barW), 0.0f, false);
        g.setGradientFill (inGrad);
        g.fillRoundedRectangle ((float)barX, (float)inBarY,
                                barW * inNorm, (float)barH, 3.0f);
    }

    // --- Output bar ---
    g.setColour (juce::Colour (0xff0e0e1e));
    g.fillRoundedRectangle ((float)barX, (float)outBarY, (float)barW, (float)barH, 3.0f);

    if (outNorm > 0.0f)
    {
        juce::ColourGradient outGrad (juce::Colour (0xff2060c8), (float)barX, 0.0f,
                                      juce::Colour (0xff8040d8), (float)(barX + barW), 0.0f, false);
        g.setGradientFill (outGrad);
        g.fillRoundedRectangle ((float)barX, (float)outBarY,
                                barW * outNorm, (float)barH, 3.0f);
    }

    // Labels and dB values
    g.setFont (juce::Font (11.5f, juce::Font::bold));
    g.setColour (juce::Colour (0xff7070a0));
    g.drawText ("INPUT",  mx, inBarY,  labelW - 4, barH, juce::Justification::left);
    g.drawText ("OUTPUT", mx, outBarY, labelW - 4, barH, juce::Justification::left);

    g.setColour (juce::Colour (0xffe8e8f2));
    g.setFont (juce::Font (10.5f));
    g.drawText (juce::String (inputLevel .load(), 1) + " dB",
                barX + 4, inBarY,  barW - 8, barH, juce::Justification::centredRight);
    g.drawText (juce::String (outputLevel.load(), 1) + " dB",
                barX + 4, outBarY, barW - 8, barH, juce::Justification::centredRight);

    // Row separators above button rows
    g.setColour (juce::Colour (0xff1e1e38));
    g.drawLine (20.0f, 492.0f, (float)(W - 20), 492.0f, 1.0f);
    g.drawLine (20.0f, 558.0f, (float)(W - 20), 558.0f, 1.0f);

    // Row labels
    g.setColour (juce::Colour (0xff5050a0));
    g.setFont (juce::Font (10.0f, juce::Font::bold));
    g.drawText ("PROFILING CONTROLS",  20, 484, 200, 10, juce::Justification::left);
    g.drawText ("LISTENING TOOLS",     20, 550, 200, 10, juce::Justification::left);
}

void MainComponent::resized()
{
    const int W   = getWidth();
    const int pad = 20;
    const int sp  = 8;     // button spacing

    // Status label — sits between instructions box and meters
    statusLabel.setBounds (pad, 342, W - 2 * pad, 34);

    // Button row 1: y=496, h=50  (25px below output meter bottom at ~467)
    {
        const int y  = 496;
        const int bh = 50;
        const int bw = (W - 2 * pad - sp * 2) / 3;
        profileButton.setBounds (pad,               y, bw, bh);
        applyButton  .setBounds (pad + bw + sp,     y, bw, bh);
        bypassButton .setBounds (pad + (bw + sp)*2, y, W - 2*pad - (bw + sp)*2, bh);
    }

    // Button row 2: y=562, h=50
    {
        const int y  = 562;
        const int bh = 50;
        const int bw = (W - 2 * pad - sp * 2) / 3;
        listenDemoButton    .setBounds (pad,               y, bw, bh);
        playRecordingButton .setBounds (pad + bw + sp,     y, bw, bh);
        guitarOnlyButton    .setBounds (pad + (bw + sp)*2, y, W - 2*pad - (bw + sp)*2, bh);
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
        statusLabel.setText ("Demo finished - profile ready", juce::dontSendNotification);
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
            statusLabel.setText ("Not enough audio - profile 3+ seconds and try again",
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
        statusLabel.setText ("Profile applied - play your guitar!", juce::dontSendNotification);
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
        statusLabel.setText ("No recording yet - profile something first", juce::dontSendNotification);
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
        statusLabel.setText ("Could not write temp recording - falling back to frequency filter",
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
        statusLabel.setText ("Demucs script not found - falling back to frequency filter",
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
        statusLabel.setText ("Running Demucs AI separation... (30-60 s) - please wait",
                             juce::dontSendNotification);
    }
    else
    {
        demucsProcess.reset();
        statusLabel.setText ("Could not start Python - falling back to frequency filter",
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
                             "- using frequency filter instead.  "
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
