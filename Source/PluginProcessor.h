#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <array>
#include <atomic>

//==============================================================================
// LA PAUTA ANALYZER PRO - Procesador de audio
// El audio pasa sin modificarse; solo se copia al analizador.
//==============================================================================
class LaPautaAnalyzerAudioProcessor : public juce::AudioProcessor
{
public:
    static constexpr int fftOrder = 13;                 // 8192 muestras
    static constexpr int fftSize  = 1 << fftOrder;
    static constexpr int ringSize = 1 << 14;            // 16384 muestras

    // Tamaño estándar: 10 cm x 8 cm (a 96 ppp = 378 x 302 px)
    static constexpr int defaultWidth  = 378;
    static constexpr int defaultHeight = 302;

    LaPautaAnalyzerAudioProcessor();
    ~LaPautaAnalyzerAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "La Pauta Analyzer Pro"; }
    bool acceptsMidi() const override  { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // Copia las últimas n muestras (mono) para el analizador
    void copyLatest (float* dest, int n) const;

    // Tamaño de la ventana (se guarda en un archivo propio y con el proyecto)
    std::atomic<int> editorWidth  { defaultWidth };
    std::atomic<int> editorHeight { defaultHeight };

    void saveSizeToDisk() const;
    static bool loadSizeFromDisk (int& w, int& h);
    static juce::File getSizeFile();

private:
    std::array<std::atomic<float>, (size_t) ringSize> ring;
    std::atomic<int> writeIndex { 0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LaPautaAnalyzerAudioProcessor)
};
