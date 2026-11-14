cmake_minimum_required(VERSION 3.22)

project(LaPautaAnalyzerPro VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.4
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(JUCE)

# Solo VST3 para Windows
juce_add_plugin(LaPautaAnalyzerPro
    COMPANY_NAME "La Pauta"
    PLUGIN_MANUFACTURER_CODE LaPa
    PLUGIN_CODE LpAn
    FORMATS VST3
    PRODUCT_NAME "La Pauta Analyzer Pro"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    VST3_CATEGORIES Analyzer Fx
)

target_sources(LaPautaAnalyzerPro PRIVATE
    PluginProcessor.cpp
    PluginProcessor.h
    PluginEditor.cpp
    PluginEditor.h
    AnalyzerComponent.h
)

target_compile_definitions(LaPautaAnalyzerPro PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(LaPautaAnalyzerPro
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
