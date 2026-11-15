#include "PluginEditor.h"

LaPautaAnalyzerAudioProcessorEditor::LaPautaAnalyzerAudioProcessorEditor (LaPautaAnalyzerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), analyzer (p)
{
    addAndMakeVisible (analyzer);

    // Redimensionable: más pequeño o más grande (mínimo ~6.7 x 5.3 cm, máximo ~40 x 32 cm)
    setResizable (true, true);
    setResizeLimits (252, 201, 1512, 1208);

    // Último tamaño guardado (archivo propio) o tamaño estándar 10 cm x 8 cm (378 x 302 px)
    int w = 0, h = 0;
    if (! LaPautaAnalyzerAudioProcessor::loadSizeFromDisk (w, h))
    {
        w = audioProcessor.editorWidth.load();
        h = audioProcessor.editorHeight.load();
    }

    setSize (juce::jlimit (252, 1512, w), juce::jlimit (201, 1208, h));
}

LaPautaAnalyzerAudioProcessorEditor::~LaPautaAnalyzerAudioProcessorEditor()
{
    stopTimer();
    audioProcessor.saveSizeToDisk();
}

void LaPautaAnalyzerAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);
}

void LaPautaAnalyzerAudioProcessorEditor::resized()
{
    analyzer.setBounds (getLocalBounds());

    // Ignora tamaños inválidos (por ejemplo, al cerrar la ventana)
    if (getWidth() >= 252 && getHeight() >= 201)
    {
        audioProcessor.editorWidth.store (getWidth());
        audioProcessor.editorHeight.store (getHeight());
        startTimer (400);   // guarda al terminar de arrastrar
    }
}

void LaPautaAnalyzerAudioProcessorEditor::timerCallback()
{
    stopTimer();
    audioProcessor.saveSizeToDisk();
}
