cmake_minimum_required(VERSION 3.22)

project(LaPautaAnalyzerPro VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.4
    GIT_SHALLOW ON
)

FetchContent_MakeAvailable(JUCE)

juce_add_plugin(LaPautaAnalyzerPro
    COMPANY_NAME "La Pauta"
    BUNDLE_ID com.lapauta.lapautaanalyzerpro
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
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/AnalyzerComponent.h
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

# JUCE creates a separate VST3 target. Install its actual generated bundle.
if(NOT TARGET LaPautaAnalyzerPro_VST3)
    message(FATAL_ERROR
        "JUCE did not create LaPautaAnalyzerPro_VST3. "
        "Check that FORMATS includes VST3 and JUCE configured correctly."
    )
endif()

get_target_property(LAPAUTA_VST3_BUNDLE
    LaPautaAnalyzerPro_VST3
    JUCE_PLUGIN_ARTEFACT_FILE
)

if(NOT LAPAUTA_VST3_BUNDLE)
    message(FATAL_ERROR
        "JUCE_PLUGIN_ARTEFACT_FILE is missing on LaPautaAnalyzerPro_VST3."
    )
endif()

install(
    DIRECTORY "${LAPAUTA_VST3_BUNDLE}/"
    DESTINATION "VST3/La Pauta Analyzer Pro.vst3"
    COMPONENT LaPautaVST3
)

# CPack + NSIS generates the Windows Setup.exe.
# NSIS must be installed on the build machine (e.g. GitHub Actions).
set(CPACK_PACKAGE_NAME "LaPautaAnalyzerPro")
set(CPACK_PACKAGE_VENDOR "La Pauta")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
    "La Pauta Analyzer Pro - VST3 audio analyzer"
)
set(CPACK_PACKAGE_INSTALL_DIRECTORY "La Pauta Analyzer Pro")
set(CPACK_GENERATOR "NSIS")
set(CPACK_NSIS_DISPLAY_NAME "La Pauta Analyzer Pro")
set(CPACK_NSIS_PACKAGE_NAME "La Pauta Analyzer Pro")
set(CPACK_NSIS_INSTALL_ROOT "$COMMONFILES64")
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
set(CPACK_NSIS_MODIFY_PATH OFF)
set(CPACK_PACKAGE_FILE_NAME "LaPautaAnalyzerPro_Setup")

include(CPack)
