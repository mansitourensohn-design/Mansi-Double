#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "DoubleVoice.h"

class DoubleMansiProcessor : public juce::AudioProcessor
{
public:
    DoubleMansiProcessor();
    ~DoubleMansiProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    using AudioProcessor::processBlock;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Double Mansi"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.15; }

    // Presets
    struct Preset { const char* name; float timing, detune, variation, width, tone, mix; bool third; };
    static const std::vector<Preset>& presets();
    int getNumPrograms() override { return (int) presets().size(); }
    int getCurrentProgram() override { return currentPreset; }
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();
    juce::AudioProcessorValueTreeState apvts;

    // fuer die Pegelanzeige im Editor
    std::atomic<float> meterLeft { 0.0f }, meterRight { 0.0f };

private:
    DoubleVoice voiceLeft, voiceRight, voiceCenter;
    juce::SmoothedValue<float> timing, detune, variation, width, mix, dryGain, thirdGain;
    int currentPreset = 0;

    std::atomic<float>* pTiming = nullptr;
    std::atomic<float>* pDetune = nullptr;
    std::atomic<float>* pVariation = nullptr;
    std::atomic<float>* pWidth = nullptr;
    std::atomic<float>* pTone = nullptr;
    std::atomic<float>* pMix = nullptr;
    std::atomic<float>* pOnly = nullptr;
    std::atomic<float>* pThird = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DoubleMansiProcessor)
};
