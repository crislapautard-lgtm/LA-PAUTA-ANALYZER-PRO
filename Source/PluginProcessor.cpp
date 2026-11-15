#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
LaPautaAnalyzerAudioProcessor::LaPautaAnalyzerAudioProcessor()
    : AudioProcessor (BusesProperties()
                        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
{
    for (auto& s : ring)
        s.store (0.0f, std::memory_order_relaxed);

    int w = 0, h = 0;
    if (loadSizeFromDisk (w, h))
    {
        editorWidth.store (w);
        editorHeight.store (h);
    }
}

juce::File LaPautaAnalyzerAudioProcessor::getSizeFile()
{
    return juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory)
               .getChildFile ("La Pauta")
               .getChildFile ("Analyzer Pro")
               .getChildFile ("size.xml");
}

void LaPautaAnalyzerAudioProcessor::saveSizeToDisk() const
{
    const int w = editorWidth.load();
    const int h = editorHeight.load();

    if (w < 252 || h < 201)
        return;

    auto file = getSizeFile();
    file.getParentDirectory().createDirectory();

    juce::XmlElement xml ("Size");
    xml.setAttribute ("w", w);
    xml.setAttribute ("h", h);
    xml.writeTo (file);
}

bool LaPautaAnalyzerAudioProcessor::loadSizeFromDisk (int& w, int& h)
{
    auto file = getSizeFile();
    if (! file.existsAsFile())
        return false;

    if (auto xml = juce::XmlDocument::parse (file))
    {
        if (xml->hasTagName ("Size"))
        {
            w = xml->getIntAttribute ("w", 0);
            h = xml->getIntAttribute ("h", 0);

            if (w >= 252 && w <= 1512 && h >= 201 && h <= 1208)
                return true;
        }
    }
    return false;
}

void LaPautaAnalyzerAudioProcessor::prepareToPlay (double, int)
{
    for (auto& s : ring)
        s.store (0.0f, std::memory_order_relaxed);
    writeIndex.store (0);
}

void LaPautaAnalyzerAudioProcessor::releaseResources() {}

bool LaPautaAnalyzerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto& in  = layouts.getMainInputChannelSet();
    const auto& out = layouts.getMainOutputChannelSet();

    if (out != juce::AudioChannelSet::mono() && out != juce::AudioChannelSet::stereo())
        return false;

    return in == out;
}

void LaPautaAnalyzerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const int numChannels = juce::jmin (buffer.getNumChannels(), getTotalNumInputChannels());
    const int numSamples  = buffer.getNumSamples();

    if (numChannels <= 0)
        return;

    int w = writeIndex.load (std::memory_order_relaxed);

    for (int i = 0; i < numSamples; ++i)
    {
        float sum = 0.0f;
        for (int ch = 0; ch < numChannels; ++ch)
            sum += buffer.getReadPointer (ch)[i];

        ring[(size_t) (w & (ringSize - 1))].store (sum / (float) numChannels, std::memory_order_relaxed);
        ++w;
    }

    writeIndex.store (w & (ringSize - 1), std::memory_order_release);
    // El audio no se modifica: pasa directo a la salida.
}

void LaPautaAnalyzerAudioProcessor::copyLatest (float* dest, int n) const
{
    const int w = writeIndex.load (std::memory_order_acquire);

    for (int i = 0; i < n; ++i)
    {
        const int idx = (w - n + i) & (ringSize - 1);
        dest[i] = ring[(size_t) idx].load (std::memory_order_relaxed);
    }
}

juce::AudioProcessorEditor* LaPautaAnalyzerAudioProcessor::createEditor()
{
    return new LaPautaAnalyzerAudioProcessorEditor (*this);
}

void LaPautaAnalyzerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::XmlElement xml ("LaPautaAnalyzerPro");
    xml.setAttribute ("w", editorWidth.load());
    xml.setAttribute ("h", editorHeight.load());
    copyXmlToBinary (xml, destData);
}

void LaPautaAnalyzerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // Si el usuario ya guardó un tamaño propio, ese tiene prioridad
    int dw = 0, dh = 0;
    if (loadSizeFromDisk (dw, dh))
    {
        editorWidth.store (dw);
        editorHeight.store (dh);
        return;
    }

    if (auto xml = getXmlFromBinary (data, sizeInBytes))
    {
        if (xml->hasTagName ("LaPautaAnalyzerPro"))
        {
            editorWidth.store  (juce::jlimit (252, 1512, xml->getIntAttribute ("w", defaultWidth)));
            editorHeight.store (juce::jlimit (201, 1208, xml->getIntAttribute ("h", defaultHeight)));
        }
    }
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LaPautaAnalyzerAudioProcessor();
}
