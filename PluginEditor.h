#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "AnalyzerComponent.h"

//==============================================================================
// Ventana del plugin: tamaño estándar 10 cm x 8 cm, redimensionable.
//==============================================================================
class LaPautaAnalyzerAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit LaPautaAnalyzerAudioProcessorEditor (LaPautaAnalyzerAudioProcessor&);
    ~LaPautaAnalyzerAudioProcessorEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    LaPautaAnalyzerAudioProcessor& audioProcessor;
    AnalyzerComponent analyzer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LaPautaAnalyzerAudioProcessorEditor)
};
