#pragma once
#include <juce_dsp/juce_dsp.h>
#include <vector>

// Eine kuenstliche "zweite Aufnahme":
// leicht versetztes, schwankendes Timing + minimale Tonhoehenabweichung + eigener Klang.
class DoubleVoice
{
public:
    void prepare (double sampleRate, int seed);
    void reset();
    void setTone (float lowpassHz);

    // baseDelayMs: Grundversatz, detuneCents: feste Verstimmung,
    // variation: 0..1 wie stark Timing/Tonhoehe "menschlich" wandern
    float process (float input, float baseDelayMs, float detuneCents, float variation);

private:
    float readHermite (float delaySamples) const;
    float nextRandom (float& value, float& target, int& counter, int interval, float smoothing);

    std::vector<float> buffer;
    int writePos = 0;
    int bufferSize = 0;
    double sr = 44100.0;

    // Pitch-Shifter mit zwei ueberblendeten Leseköpfen
    float shifterPhase = 0.0f;
    float windowSamples = 0.0f;

    // langsam wandernde Zufallswerte fuer Timing, Tonhoehe und Lautstaerke
    juce::Random rng;
    float timeValue = 0, timeTarget = 0;   int timeCounter = 0;
    float pitchValue = 0, pitchTarget = 0; int pitchCounter = 0;
    float gainValue = 0, gainTarget = 0;   int gainCounter = 0;
    float smoothCoeff = 0.0f;

    juce::dsp::StateVariableTPTFilter<float> highpass, lowpass;
};
