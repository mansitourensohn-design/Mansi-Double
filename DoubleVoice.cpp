#include "DoubleVoice.h"
#include <cmath>

void DoubleVoice::prepare (double sampleRate, int seed)
{
    sr = sampleRate;
    bufferSize = (int) std::ceil (sr * 0.25) + 8;   // 250 ms reichen fuer Versatz + Shifter-Fenster
    buffer.assign ((size_t) bufferSize, 0.0f);
    windowSamples = (float) (sr * 0.040);           // 40 ms Fenster
    smoothCoeff = 1.0f - std::exp (-1.0f / (float) (sr * 0.25)); // ~250 ms Glaettung
    rng.setSeed (seed);

    juce::dsp::ProcessSpec spec { sr, 512, 1 };
    highpass.prepare (spec);
    lowpass.prepare (spec);
    highpass.setType (juce::dsp::StateVariableTPTFilterType::highpass);
    lowpass.setType (juce::dsp::StateVariableTPTFilterType::lowpass);
    highpass.setCutoffFrequency (140.0f);
    lowpass.setCutoffFrequency (9000.0f);
    reset();
}

void DoubleVoice::reset()
{
    std::fill (buffer.begin(), buffer.end(), 0.0f);
    writePos = 0;
    shifterPhase = rng.nextFloat();
    timeValue = timeTarget = pitchValue = pitchTarget = gainValue = gainTarget = 0.0f;
    timeCounter = pitchCounter = gainCounter = 0;
    highpass.reset();
    lowpass.reset();
}

void DoubleVoice::setTone (float lowpassHz)
{
    lowpass.setCutoffFrequency (juce::jlimit (1000.0f, 20000.0f, lowpassHz));
}

float DoubleVoice::nextRandom (float& value, float& target, int& counter, int interval, float smoothing)
{
    if (--counter <= 0)
    {
        target = rng.nextFloat() * 2.0f - 1.0f;
        counter = interval + rng.nextInt (interval);
    }
    value += (target - value) * smoothing;
    return value;
}

float DoubleVoice::readHermite (float delaySamples) const
{
    float readPos = (float) writePos - delaySamples;
    while (readPos < 0.0f) readPos += (float) bufferSize;

    const int i1 = (int) readPos;
    const float f = readPos - (float) i1;
    const int i0 = (i1 - 1 + bufferSize) % bufferSize;
    const int i2 = (i1 + 1) % bufferSize;
    const int i3 = (i1 + 2) % bufferSize;

    const float y0 = buffer[(size_t) i0], y1 = buffer[(size_t) i1], y2 = buffer[(size_t) i2], y3 = buffer[(size_t) i3];
    const float c1 = 0.5f * (y2 - y0);
    const float c2 = y0 - 2.5f * y1 + 2.0f * y2 - 0.5f * y3;
    const float c3 = 0.5f * (y3 - y0) + 1.5f * (y1 - y2);
    return ((c3 * f + c2) * f + c1) * f + y1;
}

float DoubleVoice::process (float input, float baseDelayMs, float detuneCents, float variation)
{
    buffer[(size_t) writePos] = input;

    const int interval = (int) (sr * 0.35);
    const float t = nextRandom (timeValue, timeTarget, timeCounter, interval, smoothCoeff);
    const float p = nextRandom (pitchValue, pitchTarget, pitchCounter, interval, smoothCoeff);
    const float g = nextRandom (gainValue, gainTarget, gainCounter, interval * 2, smoothCoeff);

    // Timing: Grundversatz + bis zu +-7 ms Wandern
    const float delayMs = baseDelayMs + variation * 7.0f * t;
    const float baseDelay = juce::jmax (2.0f, delayMs * 0.001f * (float) sr);

    // Tonhoehe: feste Verstimmung + bis zu +-6 Cent Wandern
    const float cents = detuneCents + variation * 6.0f * p;
    const float ratio = std::exp2 (cents / 1200.0f);

    shifterPhase += (1.0f - ratio) / windowSamples;
    shifterPhase -= std::floor (shifterPhase);

    float phaseB = shifterPhase + 0.5f;
    phaseB -= std::floor (phaseB);

    const float wA = std::sin (juce::MathConstants<float>::pi * shifterPhase);
    const float wB = std::sin (juce::MathConstants<float>::pi * phaseB);

    const float out = readHermite (baseDelay + shifterPhase * windowSamples) * wA * wA
                    + readHermite (baseDelay + phaseB * windowSamples) * wB * wB;

    writePos = (writePos + 1) % bufferSize;

    // eigener Klang: etwas weniger Tiefen und Hoehen als das Original
    float y = highpass.processSample (0, out);
    y = lowpass.processSample (0, y);

    // leicht schwankende Lautstaerke (+-1.5 dB bei voller Variation)
    return y * juce::Decibels::decibelsToGain (variation * 1.5f * g);
}
