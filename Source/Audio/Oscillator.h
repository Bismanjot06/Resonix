#pragma once

#include <JuceHeader.h>

#include "../Utils/Constants.h"

namespace nebula::audio
{
class Oscillator
{
public:
    enum class Waveform
    {
        sine,
        square
    };

    void prepare(double newSampleRate);
    void setWaveform(Waveform newWaveform);
    void setFrequency(double newFrequencyHz);
    void setGain(float newGain);
    void reset();
    void render(juce::AudioBuffer<float>& buffer, int startSample, int numSamples);

private:
    double sampleRate = constants::defaultSampleRate;
    double frequencyHz = 220.0;
    float gain = 0.0f;
    double phase = 0.0;
    Waveform waveform = Waveform::sine;
};
}
