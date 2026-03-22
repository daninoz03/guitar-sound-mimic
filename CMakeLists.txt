cmake_minimum_required(VERSION 3.15)

project(GuitarSoundMimic VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# JUCE Setup
# You'll need to clone JUCE: git clone https://github.com/juce-framework/JUCE.git
# Or use CPM/FetchContent to download it automatically
add_subdirectory(JUCE)

# Main Application Target
juce_add_gui_app(GuitarSoundMimic
    COMPANY_NAME "YourCompany"
    BUNDLE_ID "com.guitarapp.GuitarSoundMimic"
    PRODUCT_NAME "Guitar Sound Mimic"
    VERSION 0.1.0
    MICROPHONE_PERMISSION_ENABLED TRUE
    MICROPHONE_PERMISSION_TEXT "Guitar Sound Mimic needs microphone access to listen to and profile guitar tones.")

target_sources(GuitarSoundMimic
    PRIVATE
        Source/Main.cpp
        Source/MainComponent.h
        Source/MainComponent.cpp
        Source/Audio/AudioEngine.h
        Source/Audio/AudioEngine.cpp
        Source/Audio/ToneProfiler.h
        Source/Audio/ToneProfiler.cpp
        Source/Audio/ToneMatcher.h
        Source/Audio/ToneMatcher.cpp
        Source/Audio/SamplePlayer.h
        Source/Audio/SamplePlayer.cpp
        Source/Audio/AudioPlayback.h
        Source/DSP/FrequencyAnalyzer.h
        Source/DSP/FrequencyAnalyzer.cpp)

target_compile_definitions(GuitarSoundMimic
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:GuitarSoundMimic,JUCE_PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:GuitarSoundMimic,JUCE_VERSION>")

juce_generate_juce_header(GuitarSoundMimic)

target_link_libraries(GuitarSoundMimic
    PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_dsp
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

# Test Runner Console Application
juce_add_console_app(GuitarSoundMimicTests
    PRODUCT_NAME "GuitarSoundMimic Tests")

target_sources(GuitarSoundMimicTests
    PRIVATE
        Tests/TestRunner.cpp
        Tests/ToneProfilerTests.cpp
        Tests/ToneMatcherTests.cpp
        Tests/FrequencyAnalyzerTests.cpp
        Tests/ToneProfileTests.cpp
        Source/Audio/AudioEngine.cpp
        Source/Audio/ToneProfiler.cpp
        Source/Audio/ToneMatcher.cpp
        Source/DSP/FrequencyAnalyzer.cpp)

target_compile_definitions(GuitarSoundMimicTests
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0)

target_link_libraries(GuitarSoundMimicTests
    PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_dsp
        juce::juce_events
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

