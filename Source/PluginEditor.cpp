#include "PluginEditor.h"

LaPautaAnalyzerAudioProcessorEditor::LaPautaAnalyzerAudioProcessorEditor (LaPautaAnalyzerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), analyzer (p)
{
    addAndMakeVisible (analyzer);

    // Redimensionable: más pequeño o más grande (mínimo ~6.7 x 5.3 cm, máximo ~40 x 32 cm)
    setResizable (true, true);
    setResizeLimits (252, 201, 1512, 1208);

    // Tamaño estándar 10 cm x 8 cm (378 x 302 px) o el último tamaño guardado
    setSize (audioProcessor.editorWidth.load(), audioProcessor.editorHeight.load());
}

void LaPautaAnalyzerAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);
}

void LaPautaAnalyzerAudioProcessorEditor::resized()
{
    analyzer.setBounds (getLocalBounds());
    audioProcessor.editorWidth.store (getWidth());
    audioProcessor.editorHeight.store (getHeight());
}
