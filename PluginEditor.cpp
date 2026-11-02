#include "PluginEditor.h"
#include "BinaryData.h"

namespace
{
    juce::ColourGradient brandGradient (juce::Rectangle<float> r)
    {
        // wie der Button "Portfolio ansehen": Hellblau -> Lavendel -> Rosa
        juce::ColourGradient g (Palette::sky, r.getX(), r.getCentreY(),
                                Palette::pink, r.getRight(), r.getCentreY(), false);
        g.addColour (0.5, Palette::lavender);
        return g;
    }

    juce::Font uiFont (float h, bool bold = false)
    {
        return juce::Font (juce::FontOptions (h, bold ? juce::Font::bold : juce::Font::plain));
    }
}

//==============================================================================
MansiLookAndFeel::MansiLookAndFeel()
{
    display = juce::Typeface::createSystemTypefaceFor (BinaryData::BebasNeueRegular_ttf,
                                                       BinaryData::BebasNeueRegular_ttfSize);
    setColour (juce::Slider::textBoxTextColourId, Palette::text);
    setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour (juce::Label::textColourId, Palette::text);
    setColour (juce::ComboBox::textColourId, Palette::text);
    setColour (juce::PopupMenu::backgroundColourId, juce::Colour (0xff1a181d));
    setColour (juce::PopupMenu::textColourId, Palette::text);
    setColour (juce::PopupMenu::highlightedBackgroundColourId, Palette::lavender.withAlpha (0.35f));
    setColour (juce::PopupMenu::highlightedTextColourId, juce::Colours::white);
}

juce::Label* MansiLookAndFeel::createSliderTextBox (juce::Slider& s)
{
    auto* l = LookAndFeel_V4::createSliderTextBox (s);
    l->setFont (uiFont (13.0f));
    l->setColour (juce::Label::textColourId, Palette::muted);
    l->setColour (juce::Label::outlineColourId, juce::Colours::transparentBlack);
    l->setColour (juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    l->setJustificationType (juce::Justification::centred);
    return l;
}

void MansiLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int w, int h,
                                         float pos, float startAngle, float endAngle, juce::Slider&)
{
    auto bounds = juce::Rectangle<float> ((float) x, (float) y, (float) w, (float) h).reduced (6.0f);
    const float size = juce::jmin (bounds.getWidth(), bounds.getHeight());
    auto r = bounds.withSizeKeepingCentre (size, size);
    const float stroke = 5.0f;
    const float radius = size * 0.5f - stroke;
    const auto c = r.getCentre();
    const float angle = startAngle + pos * (endAngle - startAngle);

    // Spur
    juce::Path track;
    track.addCentredArc (c.x, c.y, radius, radius, 0.0f, startAngle, endAngle, true);
    g.setColour (Palette::track);
    g.strokePath (track, juce::PathStrokeType (stroke, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Wert mit Verlauf + weichem Glow
    juce::Path value;
    value.addCentredArc (c.x, c.y, radius, radius, 0.0f, startAngle, angle, true);
    auto grad = brandGradient (r);
    g.setGradientFill (grad);
    g.setOpacity (0.25f);
    g.strokePath (value, juce::PathStrokeType (stroke + 6.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    g.setOpacity (1.0f);
    g.setGradientFill (grad);
    g.strokePath (value, juce::PathStrokeType (stroke, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Knopf
    const float knobR = radius - stroke - 5.0f;
    auto knob = juce::Rectangle<float> (knobR * 2.0f, knobR * 2.0f).withCentre (c);
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff2a272e), c.x, knob.getY(),
                                             juce::Colour (0xff121114), c.x, knob.getBottom(), false));
    g.fillEllipse (knob);
    g.setColour (juce::Colours::white.withAlpha (0.12f));
    g.drawEllipse (knob, 1.0f);

    // Zeiger
    juce::Path pointer;
    pointer.addRoundedRectangle (-1.5f, -knobR + 5.0f, 3.0f, knobR * 0.45f, 1.5f);
    g.setColour (Palette::text);
    g.fillPath (pointer, juce::AffineTransform::rotation (angle).translated (c));
}

void MansiLookAndFeel::drawToggleButton (juce::Graphics& g, juce::ToggleButton& b, bool highlighted, bool)
{
    // Pill-Button wie auf der Website: aus = Outline, an = Verlauf
    auto r = b.getLocalBounds().toFloat().reduced (1.5f);
    const float corner = r.getHeight() * 0.5f;

    if (b.getToggleState())
    {
        g.setGradientFill (brandGradient (r));
        g.fillRoundedRectangle (r, corner);
        g.setColour (juce::Colour (0xff141316));
    }
    else
    {
        g.setColour (juce::Colours::white.withAlpha (highlighted ? 0.08f : 0.03f));
        g.fillRoundedRectangle (r, corner);
        g.setColour (juce::Colours::white.withAlpha (0.22f));
        g.drawRoundedRectangle (r, corner, 1.2f);
        g.setColour (Palette::text);
    }

    g.setFont (uiFont (14.0f, true).withExtraKerningFactor (0.12f));
    g.drawText (b.getButtonText(), r, juce::Justification::centred);
}

void MansiLookAndFeel::drawComboBox (juce::Graphics& g, int w, int h, bool, int, int, int, int, juce::ComboBox& box)
{
    auto r = juce::Rectangle<float> (0, 0, (float) w, (float) h).reduced (1.5f);
    const float corner = r.getHeight() * 0.5f;
    g.setColour (juce::Colours::white.withAlpha (box.isMouseOver (true) ? 0.08f : 0.03f));
    g.fillRoundedRectangle (r, corner);
    g.setGradientFill (brandGradient (r));
    g.drawRoundedRectangle (r, corner, 1.4f);

    // kleiner Pfeil
    const float ax = r.getRight() - 22.0f, ay = r.getCentreY();
    juce::Path arrow;
    arrow.addTriangle (ax - 5.0f, ay - 2.5f, ax + 5.0f, ay - 2.5f, ax, ay + 3.5f);
    g.setColour (Palette::text);
    g.fillPath (arrow);
}

void MansiLookAndFeel::positionComboBoxText (juce::ComboBox& box, juce::Label& label)
{
    label.setBounds (16, 1, box.getWidth() - 44, box.getHeight() - 2);
    label.setFont (getComboBoxFont (box));
    label.setJustificationType (juce::Justification::centredLeft);
}

juce::Font MansiLookAndFeel::getComboBoxFont (juce::ComboBox&) { return uiFont (14.0f, true); }
juce::Font MansiLookAndFeel::getPopupMenuFont() { return uiFont (15.0f); }

//==============================================================================
DoubleMansiEditor::DoubleMansiEditor (DoubleMansiProcessor& p)
    : AudioProcessorEditor (p), processor (p)
{
    setLookAndFeel (&lnf);

    addKnob (0, "timing",    "TIMING");
    addKnob (1, "detune",    "DETUNE");
    addKnob (2, "variation", "VARIATION");
    addKnob (3, "width",     "BREITE");
    addKnob (4, "tone",      "TON");
    addKnob (5, "mix",       "DOUBLES");

    addAndMakeVisible (onlyButton);
    onlyButton.setClickingTogglesState (true);
    onlyAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (processor.apvts, "only", onlyButton);

    addAndMakeVisible (thirdButton);
    thirdButton.setClickingTogglesState (true);
    thirdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (processor.apvts, "third", thirdButton);

    // Presets
    for (int i = 0; i < processor.getNumPrograms(); ++i)
        presetBox.addItem (processor.getProgramName (i), i + 1);
    presetBox.setTextWhenNothingSelected ("PRESETS");
    presetBox.setSelectedId (processor.getCurrentProgram() + 1, juce::dontSendNotification);
    presetBox.onChange = [this]
    {
        const int id = presetBox.getSelectedId();
        if (id <= 0 || applyingPreset) return;
        applyingPreset = true;
        processor.setCurrentProgram (id - 1);
        presetDirty = false;
        applyingPreset = false;
    };
    addAndMakeVisible (presetBox);

    // wenn ein Regler bewegt wird, ist es kein reines Preset mehr
    for (auto* param : processor.getParameters())
        param->addListener (this);

   #if DM_HAS_LOGO
    logo = juce::ImageCache::getFromMemory (BinaryData::logo_png, BinaryData::logo_pngSize);
   #endif

    setSize (720, 520);
    startTimerHz (30);
}

DoubleMansiEditor::~DoubleMansiEditor()
{
    stopTimer();
    for (auto* param : processor.getParameters())
        param->removeListener (this);
    setLookAndFeel (nullptr);
}

void DoubleMansiEditor::addKnob (int i, const juce::String& id, const juce::String& label)
{
    auto& k = knobs[i];
    k.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    k.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 90, 20);
    k.setRotaryParameters (juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    k.setDoubleClickReturnValue (true, processor.apvts.getParameter (id)->convertFrom0to1 (
                                           processor.apvts.getParameter (id)->getDefaultValue()));
    k.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    k.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    k.setColour (juce::Slider::textBoxTextColourId, Palette::muted);
    addAndMakeVisible (k);
    attachments[i] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (processor.apvts, id, k);

    const auto unit = processor.apvts.getParameter (id)->getLabel();
    k.setTextValueSuffix (" " + unit);
    if (id == "tone") k.setNumDecimalPlacesToDisplay (0);

    auto& l = labels[i];
    l.setText (label, juce::dontSendNotification);
    l.setJustificationType (juce::Justification::centred);
    l.setFont (uiFont (13.0f, true).withExtraKerningFactor (0.35f));
    l.setColour (juce::Label::textColourId, Palette::text);
    addAndMakeVisible (l);
}

void DoubleMansiEditor::timerCallback()
{
    meterL = juce::jmax (processor.meterLeft.load(), meterL * 0.88f);
    meterR = juce::jmax (processor.meterRight.load(), meterR * 0.88f);
    repaint (meterArea);

    if (presetDirty.exchange (false) && ! applyingPreset && presetBox.getSelectedId() != 0)
    {
        // pruefen, ob die Regler noch exakt dem gewaehlten Preset entsprechen
        const auto& pr = DoubleMansiProcessor::presets()[(size_t) presetBox.getSelectedId() - 1];
        auto val = [this] (const char* id) { return processor.apvts.getRawParameterValue (id)->load(); };
        const bool same = std::abs (val ("timing") - pr.timing) < 0.05f
                       && std::abs (val ("detune") - pr.detune) < 0.05f
                       && std::abs (val ("variation") - pr.variation) < 0.5f
                       && std::abs (val ("width") - pr.width) < 0.5f
                       && std::abs (val ("tone") - pr.tone) < 1.0f
                       && std::abs (val ("mix") - pr.mix) < 0.5f
                       && ((val ("third") > 0.5f) == pr.third);
        if (! same)
            presetBox.setText (presetBox.getText().upToFirstOccurrenceOf (" *", false, false) + " *", juce::dontSendNotification);
    }
}

void DoubleMansiEditor::paint (juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat();

    // dunkler Hintergrund mit Verlauf
    g.setGradientFill (juce::ColourGradient (Palette::bgTop, 0, 0, Palette::bgBottom, 0, b.getBottom(), false));
    g.fillAll();

    // weiche Farbschimmer
    auto glow = [&g] (juce::Colour col, float cx, float cy, float rad)
    {
        g.setGradientFill (juce::ColourGradient (col.withAlpha (0.16f), cx, cy,
                                                 col.withAlpha (0.0f), cx + rad, cy, true));
        g.fillEllipse (cx - rad, cy - rad, rad * 2.0f, rad * 2.0f);
    };
    glow (Palette::lavender, b.getWidth() * 0.15f, 40.0f, 260.0f);
    glow (Palette::pink, b.getWidth() * 0.9f, b.getHeight() * 0.95f, 280.0f);
    glow (Palette::sky, b.getWidth() * 0.85f, 30.0f, 180.0f);

    // Titel im Verlaufs-Schriftzug (mit Logo davor, falls vorhanden)
    const juce::String title ("DOUBLE MANSI");
    const auto titleFont = juce::Font (juce::FontOptions (lnf.display).withHeight (64.0f)).withExtraKerningFactor (0.04f);
    const float textW = juce::GlyphArrangement::getStringWidth (titleFont, title);
    const float logoH = 66.0f;
    const float logoW = logo.isValid() ? logoH * (float) logo.getWidth() / (float) juce::jmax (1, logo.getHeight()) : 0.0f;
    const float gap = logo.isValid() ? 16.0f : 0.0f;
    const float startX = (b.getWidth() - (logoW + gap + textW)) * 0.5f;

    if (logo.isValid())
        g.drawImage (logo, juce::Rectangle<float> (startX, 20.0f, logoW, logoH), juce::RectanglePlacement::centred);

    auto titleArea = juce::Rectangle<float> (startX + logoW + gap, 22.0f, textW + 4.0f, 64.0f);
    g.setFont (titleFont);
    {
        juce::ColourGradient tg (Palette::lavender, titleArea.getX(), 0, Palette::sky, titleArea.getRight(), 0, false);
        tg.addColour (0.62, Palette::pink);
        g.setGradientFill (tg);
    }
    g.drawText (title, titleArea, juce::Justification::centredLeft);

    // Untertitel mit weiter Laufweite und Punkten
    g.setFont (uiFont (13.0f, true).withExtraKerningFactor (0.55f));
    auto sub = juce::Rectangle<float> (0.0f, 90.0f, b.getWidth(), 20.0f);
    g.setColour (Palette::text);
    g.drawText (juce::CharPointer_UTF8 ("VOCAL  \xe2\x80\xa2  DOUBLER  \xe2\x80\xa2  STACK"), sub, juce::Justification::centred);

    // Panel hinter den Reglern
    auto panel = juce::Rectangle<float> (24.0f, 126.0f, b.getWidth() - 48.0f, 310.0f);
    g.setColour (Palette::panel.withAlpha (0.7f));
    g.fillRoundedRectangle (panel, 22.0f);
    g.setColour (juce::Colours::white.withAlpha (0.07f));
    g.drawRoundedRectangle (panel, 22.0f, 1.0f);

    // Pegelanzeige L/R
    auto m = meterArea.toFloat();
    if (! m.isEmpty())
    {
        const float barH = 6.0f;
        for (int ch = 0; ch < 2; ++ch)
        {
            auto bar = juce::Rectangle<float> (m.getX(), m.getY() + ch * (barH + 5.0f), m.getWidth(), barH);
            g.setColour (Palette::track);
            g.fillRoundedRectangle (bar, barH * 0.5f);
            const float lvl = juce::jlimit (0.0f, 1.0f, ch == 0 ? meterL : meterR);
            const float db = juce::Decibels::gainToDecibels (lvl, -60.0f);
            const float frac = juce::jmap (db, -60.0f, 0.0f, 0.0f, 1.0f);
            if (frac > 0.0f)
            {
                g.setGradientFill (brandGradient (bar));
                g.fillRoundedRectangle (bar.withWidth (juce::jmax (barH, bar.getWidth() * frac)), barH * 0.5f);
            }
        }
    }

    // Fusszeile
    g.setColour (Palette::muted);
    g.setFont (uiFont (11.0f).withExtraKerningFactor (0.3f));
    g.drawText ("by MANSI", getLocalBounds().removeFromBottom (24).reduced (28, 0), juce::Justification::centredRight);
}

void DoubleMansiEditor::resized()
{
    auto area = juce::Rectangle<int> (24, 126, getWidth() - 48, 310).reduced (16, 18);
    const int rowH = area.getHeight() / 2;
    const int colW = area.getWidth() / 3;

    for (int i = 0; i < numKnobs; ++i)
    {
        auto cell = juce::Rectangle<int> (area.getX() + (i % 3) * colW, area.getY() + (i / 3) * rowH, colW, rowH);
        labels[i].setBounds (cell.removeFromTop (20));
        knobs[i].setBounds (cell.reduced (30, 4));
    }

    auto bottom = juce::Rectangle<int> (24, 452, getWidth() - 48, 36);
    onlyButton.setBounds (bottom.removeFromLeft (160));
    bottom.removeFromLeft (12);
    thirdButton.setBounds (bottom.removeFromLeft (140));
    bottom.removeFromLeft (12);
    presetBox.setBounds (bottom.removeFromLeft (200));
    bottom.removeFromLeft (20);
    meterArea = bottom.withSizeKeepingCentre (juce::jmin (150, bottom.getWidth()), 17);
}
