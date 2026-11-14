#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_dsp/juce_dsp.h>
#include <array>
#include <cmath>
#include "PluginProcessor.h"

//==============================================================================
// Analizador de espectro: barras de 1/6 de octava (25 Hz - 18 kHz),
// línea amarilla = picos, línea blanca = promedio. Fondo negro.
//==============================================================================
class AnalyzerComponent : public juce::Component, private juce::Timer
{
public:
    static constexpr int fftSize  = LaPautaAnalyzerAudioProcessor::fftSize;
    static constexpr int numBands = 58;
    static constexpr float minDb  = -55.0f;
    static constexpr float maxDb  = 0.0f;

    explicit AnalyzerComponent (LaPautaAnalyzerAudioProcessor& p)
        : processor (p),
          fft (LaPautaAnalyzerAudioProcessor::fftOrder),
          window ((size_t) fftSize, juce::dsp::WindowingFunction<float>::hann)
    {
        bars.fill (minDb);
        peaks.fill (minDb);
        avgs.fill (minDb);
        fftData.fill (0.0f);

        setOpaque (true);
        startTimerHz (30);
    }

    ~AnalyzerComponent() override { stopTimer(); }

    //==========================================================================
    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::black);

        const float s = juce::jlimit (0.6f, 4.0f, (float) getWidth() / 378.0f);
        const float mL = 28.0f * s, mR = 28.0f * s, mT = 20.0f * s, mB = 18.0f * s;

        auto plot = juce::Rectangle<float> (mL, mT,
                                            (float) getWidth()  - mL - mR,
                                            (float) getHeight() - mT - mB);
        if (plot.getWidth() < 10.0f || plot.getHeight() < 10.0f)
            return;

        auto xFor = [&] (float f)
        {
            return plot.getX() + plot.getWidth() * (std::log10 (f / 20.0f) / 3.0f);
        };
        auto yFor = [&] (float db)
        {
            return plot.getY() + plot.getHeight() * (maxDb - db) / (maxDb - minDb);
        };

        // Título
        g.setColour (juce::Colour (0xff8a8f94));
        g.setFont (11.0f * s);
        g.drawText ("LA PAUTA ANALYZER PRO", juce::Rectangle<float> (mL, 2.0f * s, plot.getWidth(), mT - 3.0f * s),
                    juce::Justification::centredLeft, false);

        // Cuadrícula vertical (frecuencia)
        const float gridFreqs[] = { 20, 30, 40, 50, 60, 70, 80, 90, 100, 200, 300, 400, 500, 600, 700, 800, 900,
                                    1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000,
                                    10000, 20000 };
        for (float f : gridFreqs)
        {
            const bool major = (f == 20 || f == 50 || f == 100 || f == 200 || f == 500 || f == 1000
                                || f == 2000 || f == 5000 || f == 10000 || f == 20000);
            g.setColour (juce::Colour (major ? 0xff2a2d30 : 0xff17191b));
            const float x = xFor (f);
            g.drawVerticalLine ((int) x, plot.getY(), plot.getBottom());
        }

        // Cuadrícula horizontal (dB) + etiquetas a ambos lados
        g.setFont (10.0f * s);
        for (int db = -5; db >= -50; db -= 5)
        {
            const float y = yFor ((float) db);
            g.setColour (juce::Colour (0xff202326));
            g.drawHorizontalLine ((int) y, plot.getX(), plot.getRight());

            g.setColour (juce::Colour (0xff9aa0a6));
            g.drawText (juce::String (db), juce::Rectangle<float> (0.0f, y - 6.0f * s, mL - 4.0f * s, 12.0f * s),
                        juce::Justification::centredRight, false);
            g.drawText (juce::String (db), juce::Rectangle<float> (plot.getRight() + 4.0f * s, y - 6.0f * s, mR - 4.0f * s, 12.0f * s),
                        juce::Justification::centredLeft, false);
        }

        // Etiquetas de frecuencia
        struct Lbl { float f; const char* t; };
        const Lbl labels[] = { { 25, "25" }, { 50, "50" }, { 100, "100" }, { 200, "200" }, { 500, "500" },
                               { 1000, "1k" }, { 2000, "2k" }, { 5000, "5k" }, { 10000, "10k" }, { 20000, "20k" } };
        g.setColour (juce::Colour (0xff9aa0a6));
        for (auto& l : labels)
        {
            const float x = xFor (l.f);
            g.drawText (l.t, juce::Rectangle<float> (x - 18.0f * s, plot.getBottom() + 2.0f * s, 36.0f * s, 13.0f * s),
                        juce::Justification::centred, false);
        }

        // Marco del área
        g.setColour (juce::Colour (0xff3a3e42));
        g.drawRect (plot, 1.0f);

        // Todo lo siguiente se recorta al área del gráfico
        juce::Graphics::ScopedSaveState state (g);
        g.reduceClipRegion (plot.toNearestInt());

        // Barras
        const float ratio = std::pow (2.0f, 1.0f / 12.0f);
        for (int i = 0; i < numBands; ++i)
        {
            const float fc = bandCentre (i);
            const float x0 = xFor (fc / ratio) + 0.5f;
            const float x1 = xFor (fc * ratio) - 0.5f;
            const float y  = yFor (juce::jlimit (minDb, maxDb, bars[(size_t) i]));

            g.setColour (juce::Colour (0xffdfe4e8).withAlpha (0.85f));
            g.fillRect (juce::Rectangle<float> (x0, y, juce::jmax (1.0f, x1 - x0), plot.getBottom() - y));
        }

        // Líneas: promedio (blanca) y picos (amarilla)
        auto buildPath = [&] (const std::array<float, numBands>& v)
        {
            juce::Path p;
            std::array<juce::Point<float>, numBands> pts;
            for (int i = 0; i < numBands; ++i)
                pts[(size_t) i] = { xFor (bandCentre (i)), yFor (juce::jlimit (minDb, maxDb, v[(size_t) i])) };

            p.startNewSubPath (pts[0]);
            for (int i = 1; i < numBands; ++i)
            {
                auto mid = (pts[(size_t) i - 1] + pts[(size_t) i]) * 0.5f;
                p.quadraticTo (pts[(size_t) i - 1], mid);
            }
            p.lineTo (pts[numBands - 1]);
            return p;
        };

        const juce::PathStrokeType stroke (1.6f * s, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);

        g.setColour (juce::Colours::white.withAlpha (0.95f));
        g.strokePath (buildPath (avgs), stroke);

        g.setColour (juce::Colour (0xffd4a62f));
        g.strokePath (buildPath (peaks), stroke);
    }

private:
    //==========================================================================
    static float bandCentre (int i) { return 25.0f * std::pow (2.0f, (float) i / 6.0f); }

    void timerCallback() override
    {
        analyse();
        repaint();
    }

    void analyse()
    {
        const double sr = processor.getSampleRate() > 0.0 ? processor.getSampleRate() : 44100.0;
        const float binHz = (float) (sr / (double) fftSize);

        processor.copyLatest (fftData.data(), fftSize);
        window.multiplyWithWindowingTable (fftData.data(), (size_t) fftSize);
        std::fill (fftData.begin() + fftSize, fftData.end(), 0.0f);
        fft.performFrequencyOnlyForwardTransform (fftData.data(), true);

        // Seno de amplitud 1 con ventana Hann -> pico de N/4
        const float norm = 4.0f / (float) fftSize;
        const float ratio = std::pow (2.0f, 1.0f / 12.0f);
        const int maxBin = fftSize / 2 - 1;

        for (int i = 0; i < numBands; ++i)
        {
            const float fc = bandCentre (i);
            const float lo = fc / ratio;
            const float hi = fc * ratio;

            int b0 = (int) std::ceil (lo / binHz);
            int b1 = juce::jmin ((int) std::floor (hi / binHz), maxBin);

            float power = 0.0f;
            if (b0 >= 1 && b1 >= b0)
            {
                for (int b = b0; b <= b1; ++b)
                {
                    const float m = fftData[(size_t) b] * norm;
                    power += m * m;
                }
            }
            else
            {
                // Banda más angosta que un bin: interpolamos
                const float pos = juce::jmin (fc / binHz, (float) maxBin - 1.0f);
                const int i0 = (int) pos;
                const float frac = pos - (float) i0;
                const float m = (fftData[(size_t) i0] * (1.0f - frac) + fftData[(size_t) i0 + 1] * frac) * norm;
                power = m * m;
            }

            const float db = juce::jlimit (minDb - 10.0f, maxDb + 10.0f,
                                           10.0f * std::log10 (juce::jmax (power, 1.0e-12f)));

            // Barras: ataque rápido, caída suave
            auto& bar = bars[(size_t) i];
            bar += (db - bar) * (db > bar ? 0.55f : 0.12f);

            // Picos: retención con caída lenta
            auto& pk = peaks[(size_t) i];
            pk = juce::jmax (bar, pk - 0.25f);

            // Promedio lento
            auto& av = avgs[(size_t) i];
            av += (db - av) * 0.04f;
        }
    }

    //==========================================================================
    LaPautaAnalyzerAudioProcessor& processor;
    juce::dsp::FFT fft;
    juce::dsp::WindowingFunction<float> window;

    std::array<float, (size_t) fftSize * 2> fftData;
    std::array<float, numBands> bars, peaks, avgs;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnalyzerComponent)
};
