#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include "PluginProcessor.h"

namespace Palette
{
    // Farben aus dem Website-Look: Lavendel, Rosa, Hellblau auf fast Schwarz
    const juce::Colour lavender { 0xffb8a9dc };
    const juce::Colour pink     { 0xffe9a9b8 };
    const juce::Colour sky      { 0xff86d3ec };
    const juce::Colour bgTop    { 0xff1c1a1f };
    const juce::Colour bgBottom { 0xff0b0b0d };
    const juce::Colour panel    { 0xff17161a };
    const juce::Colour track    { 0xff2b2930 };
    const juce::Colour text     { 0xfff2eff4 };
    const juce::Colour muted    { 0xff8d8893 };
}

class MansiLookAndFeel : public juce::LookAndFeel_V4
{
public:
    MansiLookAndFeel();
    void drawRotarySlider (juce::Graphics&, int x, int y, int w, int h,
                           float pos, float startAngle, float endAngle, juce::Slider&) override;
    void drawToggleButton (juce::Graphics&, juce::ToggleButton&, bool highlighted, bool down) override;
    juce::Label* createSliderTextBox (juce::Slider&) override;
    void drawComboBox (juce::Graphics&, int w, int h, bool down, int, int, int, int, juce::ComboBox&) override;
    void positionComboBoxText (juce::ComboBox&, juce::Label&) override;
    juce::Font getComboBoxFont (juce::ComboBox&) override;
    juce::Font getPopupMenuFont() override;

    juce::Typeface::Ptr display;   // Bebas Neue (OFL)
};

class DoubleMansiEditor : public juce::AudioProcessorEditor,
                          private juce::Timer,
                          private juce::AudioProcessorParameter::Listener
{
public:
    explicit DoubleMansiEditor (DoubleMansiProcessor&);
    ~DoubleMansiEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void parameterValueChanged (int, float) override { presetDirty = true; }
    void parameterGestureChanged (int, bool) override {}
    void addKnob (int index, const juce::String& id, const juce::String& label);

    DoubleMansiProcessor& processor;
    MansiLookAndFeel lnf;

    static constexpr int numKnobs = 6;
    juce::Slider knobs[numKnobs];
    juce::Label labels[numKnobs];
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachments[numKnobs];

    juce::ToggleButton onlyButton { "NUR DOUBLES" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> onlyAttachment;
    juce::ToggleButton thirdButton { "3. DOUBLE" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> thirdAttachment;

    juce::ComboBox presetBox;
    std::atomic<bool> presetDirty { false };
    bool applyingPreset = false;

    juce::Image logo;

    float meterL = 0.0f, meterR = 0.0f;
    juce::Rectangle<int> meterArea;
};
