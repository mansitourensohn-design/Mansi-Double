#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

DoubleMansiProcessor::DoubleMansiProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput ("Input", juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "STATE", createLayout())
{
    pTiming    = apvts.getRawParameterValue ("timing");
    pDetune    = apvts.getRawParameterValue ("detune");
    pVariation = apvts.getRawParameterValue ("variation");
    pWidth     = apvts.getRawParameterValue ("width");
    pTone      = apvts.getRawParameterValue ("tone");
    pMix       = apvts.getRawParameterValue ("mix");
    pOnly      = apvts.getRawParameterValue ("only");
    pThird     = apvts.getRawParameterValue ("third");
}

const std::vector<DoubleMansiProcessor::Preset>& DoubleMansiProcessor::presets()
{
    //                          Timing Detune Var  Breite  Ton     Doubles 3.Double
    static const std::vector<Preset> list {
        { "Natuerlich",          22.0f,  9.0f, 50.0f,  80.0f, 9000.0f, 60.0f, false },
        { "Tight Pop",           12.0f,  6.0f, 30.0f,  90.0f, 10000.0f, 55.0f, false },
        { "Breiter Refrain",     28.0f, 12.0f, 70.0f, 100.0f, 8000.0f, 70.0f, true  },
        { "Rock-Wall",           35.0f, 15.0f, 80.0f, 100.0f, 7000.0f, 80.0f, true  },
        { "Fluester-Doubles",    18.0f,  8.0f, 40.0f,  70.0f, 5000.0f, 40.0f, false },
        { "Choir-Stack",         40.0f, 20.0f, 90.0f, 100.0f, 6500.0f, 75.0f, true  },
        { "Lo-Fi Schatten",      30.0f, 10.0f, 60.0f,  60.0f, 3500.0f, 50.0f, true  },
    };
    return list;
}

const juce::String DoubleMansiProcessor::getProgramName (int index)
{
    if (! juce::isPositiveAndBelow (index, (int) presets().size())) return {};
    juce::String n (presets()[(size_t) index].name);
    return n.replace ("ue", juce::CharPointer_UTF8 ("\xc3\xbc"));
}

void DoubleMansiProcessor::setCurrentProgram (int index)
{
    if (! juce::isPositiveAndBelow (index, (int) presets().size())) return;
    currentPreset = index;
    const auto& pr = presets()[(size_t) index];

    auto set = [this] (const char* id, float value)
    {
        if (auto* param = apvts.getParameter (id))
            param->setValueNotifyingHost (param->convertTo0to1 (value));
    };
    set ("timing", pr.timing);
    set ("detune", pr.detune);
    set ("variation", pr.variation);
    set ("width", pr.width);
    set ("tone", pr.tone);
    set ("mix", pr.mix);
    set ("third", pr.third ? 1.0f : 0.0f);
}

juce::AudioProcessorValueTreeState::ParameterLayout DoubleMansiProcessor::createLayout()
{
    using namespace juce;
    std::vector<std::unique_ptr<RangedAudioParameter>> p;

    auto pct = AudioParameterFloatAttributes().withLabel ("%");

    p.push_back (std::make_unique<AudioParameterFloat> (ParameterID { "timing", 1 }, "Timing",
        NormalisableRange<float> (8.0f, 45.0f, 0.1f), 22.0f, AudioParameterFloatAttributes().withLabel ("ms")));
    p.push_back (std::make_unique<AudioParameterFloat> (ParameterID { "detune", 1 }, "Detune",
        NormalisableRange<float> (0.0f, 25.0f, 0.1f), 9.0f, AudioParameterFloatAttributes().withLabel ("ct")));
    p.push_back (std::make_unique<AudioParameterFloat> (ParameterID { "variation", 1 }, "Variation",
        NormalisableRange<float> (0.0f, 100.0f, 1.0f), 50.0f, pct));
    p.push_back (std::make_unique<AudioParameterFloat> (ParameterID { "width", 1 }, "Breite",
        NormalisableRange<float> (0.0f, 100.0f, 1.0f), 80.0f, pct));
    p.push_back (std::make_unique<AudioParameterFloat> (ParameterID { "tone", 1 }, "Ton",
        NormalisableRange<float> (2000.0f, 18000.0f, 1.0f, 0.4f), 9000.0f, AudioParameterFloatAttributes().withLabel ("Hz")));
    p.push_back (std::make_unique<AudioParameterFloat> (ParameterID { "mix", 1 }, "Doubles",
        NormalisableRange<float> (0.0f, 100.0f, 1.0f), 60.0f, pct));
    p.push_back (std::make_unique<AudioParameterBool> (ParameterID { "only", 1 }, "Nur Doubles", false));
    p.push_back (std::make_unique<AudioParameterBool> (ParameterID { "third", 1 }, "Drittes Double", false));

    return { p.begin(), p.end() };
}

bool DoubleMansiProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto in = layouts.getMainInputChannelSet();
    const auto out = layouts.getMainOutputChannelSet();
    if (out != juce::AudioChannelSet::stereo()) return false;
    return in == juce::AudioChannelSet::mono() || in == juce::AudioChannelSet::stereo();
}

void DoubleMansiProcessor::prepareToPlay (double sampleRate, int)
{
    voiceLeft.prepare (sampleRate, 1337);
    voiceRight.prepare (sampleRate, 4242);
    voiceCenter.prepare (sampleRate, 777);

    for (auto* s : { &timing, &detune, &variation, &width, &mix, &dryGain, &thirdGain })
        s->reset (sampleRate, 0.05);

    timing.setCurrentAndTargetValue (pTiming->load());
    detune.setCurrentAndTargetValue (pDetune->load());
    variation.setCurrentAndTargetValue (pVariation->load() * 0.01f);
    width.setCurrentAndTargetValue (pWidth->load() * 0.01f);
    mix.setCurrentAndTargetValue (pMix->load() * 0.01f);
    dryGain.setCurrentAndTargetValue (pOnly->load() > 0.5f ? 0.0f : 1.0f);
    thirdGain.setCurrentAndTargetValue (pThird->load() > 0.5f ? 1.0f : 0.0f);
}

void DoubleMansiProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const int numIn = getTotalNumInputChannels();
    const int numOut = getTotalNumOutputChannels();
    const int n = buffer.getNumSamples();
    if (numOut < 2) return;

    timing.setTargetValue (pTiming->load());
    detune.setTargetValue (pDetune->load());
    variation.setTargetValue (pVariation->load() * 0.01f);
    width.setTargetValue (pWidth->load() * 0.01f);
    mix.setTargetValue (pMix->load() * 0.01f);
    dryGain.setTargetValue (pOnly->load() > 0.5f ? 0.0f : 1.0f);
    thirdGain.setTargetValue (pThird->load() > 0.5f ? 1.0f : 0.0f);

    voiceLeft.setTone (pTone->load());
    voiceRight.setTone (pTone->load() * 0.9f); // rechte Stimme minimal dunkler
    voiceCenter.setTone (pTone->load() * 0.8f); // mittlere Stimme etwas weicher

    auto* L = buffer.getWritePointer (0);
    auto* R = buffer.getWritePointer (1);
    const bool stereoIn = numIn >= 2;

    float peakL = 0.0f, peakR = 0.0f;
    constexpr float quarterPi = juce::MathConstants<float>::pi * 0.25f;

    for (int i = 0; i < n; ++i)
    {
        const float inL = L[i];
        const float inR = stereoIn ? R[i] : L[i];
        const float mono = 0.5f * (inL + inR);

        const float tm = timing.getNextValue();
        const float dt = detune.getNextValue();
        const float va = variation.getNextValue();
        const float wd = width.getNextValue();
        const float mx = mix.getNextValue();
        const float dry = dryGain.getNextValue();
        const float th = thirdGain.getNextValue();

        // zwei Stimmen mit unterschiedlichem Timing und entgegengesetzter Verstimmung
        const float v1 = voiceLeft.process (mono, tm, +dt, va);
        const float v2 = voiceRight.process (mono, tm * 1.37f + 3.0f, -dt, va);

        // Panorama mit gleicher Lautheit: Stimme 1 nach links, Stimme 2 nach rechts
        const float a1 = (1.0f - wd) * quarterPi;          // 0 = ganz links
        const float a2 = (1.0f + wd) * quarterPi;          // pi/2 = ganz rechts
        // drittes Double: mittig, mit eigenem Timing und leichter Verstimmung nach oben
        float v3 = 0.0f;
        if (th > 0.0001f)
            v3 = th * voiceCenter.process (mono, tm * 0.6f + 11.0f, dt * 0.5f, va);
        else
            voiceCenter.process (mono, tm * 0.6f + 11.0f, dt * 0.5f, va); // weiterlaufen lassen, damit das Einblenden sauber ist

        // bei drei Stimmen etwas leiser mischen, damit die Gesamtlautstaerke gleich bleibt
        const float level = mx * 0.8f * (1.0f - 0.18f * th);
        const float centre = 0.7071f * 0.85f * v3;

        const float outL = inL * dry + level * (v1 * std::cos (a1) + v2 * std::cos (a2) + centre);
        const float outR = inR * dry + level * (v1 * std::sin (a1) + v2 * std::sin (a2) + centre);

        L[i] = outL;
        R[i] = outR;
        peakL = juce::jmax (peakL, std::abs (outL));
        peakR = juce::jmax (peakR, std::abs (outR));
    }

    meterLeft.store (juce::jmax (peakL, meterLeft.load() * 0.85f));
    meterRight.store (juce::jmax (peakR, meterRight.load() * 0.85f));
}

void DoubleMansiProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary (*xml, destData);
}

void DoubleMansiProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessorEditor* DoubleMansiProcessor::createEditor()
{
    return new DoubleMansiEditor (*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DoubleMansiProcessor();
}
