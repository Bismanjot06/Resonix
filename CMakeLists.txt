cmake_minimum_required(VERSION 3.22)

project(Resonix VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.8
)

FetchContent_MakeAvailable(juce)

juce_add_gui_app(Resonix
    COMPANY_NAME "Nebula"
    PRODUCT_NAME "Resonix"
)

juce_generate_juce_header(Resonix)

target_sources(Resonix PRIVATE
    Source/Main.cpp
    Source/Audio/AudioEngine.cpp
    Source/Audio/Mixer.cpp
    Source/Audio/Oscillator.cpp
    Source/Audio/Sequencer.cpp
    Source/DSP/Effects.cpp
    Source/DSP/Envelope.cpp
    Source/DSP/Filter.cpp
    Source/UI/DrumPad.cpp
    Source/UI/Knob.cpp
    Source/UI/MainComponent.cpp
    Source/UI/Timeline.cpp
)

target_compile_definitions(Resonix PRIVATE
    JUCE_MODAL_LOOPS_PERMITTED=1
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
)

target_include_directories(Resonix PRIVATE Source)

target_link_libraries(Resonix PRIVATE
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_data_structures
    juce::juce_dsp
    juce::juce_gui_basics
    juce::juce_gui_extra
)
