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

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();
    juce::AudioProcessorValueTreeState apvts;

    // fuer die Pegelanzeige im Editor
    std::atomic<float> meterLeft { 0.0f }, meterRight { 0.0f };

private:
    DoubleVoice voiceLeft, voiceRight;
    juce::SmoothedValue<float> timing, detune, variation, width, mix, dryGain;

    std::atomic<float>* pTiming = nullptr;
    std::atomic<float>* pDetune = nullptr;
    std::atomic<float>* pVariation = nullptr;
    std::atomic<float>* pWidth = nullptr;
    std::atomic<float>* pTone = nullptr;
    std::atomic<float>* pMix = nullptr;
    std::atomic<float>* pOnly = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DoubleMansiProcessor)
};
