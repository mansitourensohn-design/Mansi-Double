cmake_minimum_required(VERSION 3.22)
project(DoubleMansi VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.4
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(DoubleMansi
    COMPANY_NAME "Mansi"
    PLUGIN_MANUFACTURER_CODE Mnsi
    PLUGIN_CODE Dbms
    FORMATS VST3 Standalone
    PRODUCT_NAME "Double Mansi"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    VST3_CATEGORIES Fx Delay
    COPY_PLUGIN_AFTER_BUILD FALSE)

juce_add_binary_data(DoubleMansiData SOURCES BebasNeue-Regular.ttf)

target_sources(DoubleMansi PRIVATE
    PluginProcessor.cpp
    PluginEditor.cpp
    DoubleVoice.cpp)

target_compile_definitions(DoubleMansi PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0)

target_link_libraries(DoubleMansi
    PRIVATE
        DoubleMansiData
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
