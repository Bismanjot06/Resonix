#include "Oscillator.h"

namespace nebula::audio
{
void Oscillator::prepare(double newSampleRate)
{
    sampleRate = juce::jmax(1.0, newSampleRate);
    reset();
}

void Oscillator::setWaveform(Waveform newWaveform)
{
    waveform = newWaveform;
}

void Oscillator::setFrequency(double newFrequencyHz)
{
    frequencyHz = juce::jlimit(20.0, 20000.0, newFrequencyHz);
}

void Oscillator::setGain(float newGain)
{
    gain = juce::jlimit(0.0f, 1.0f, newGain);
}

void Oscillator::reset()
{
    phase = 0.0;
}

void Oscillator::render(juce::AudioBuffer<float>& buffer, int startSample, int numSamples)
{
    if (gain <= 0.0f)
        return;

    const auto numChannels = buffer.getNumChannels();
    const auto phaseIncrement = juce::MathConstants<double>::twoPi * frequencyHz / sampleRate;

    for (int sample = 0; sample < numSamples; ++sample)
    {
        const auto value = waveform == Waveform::sine ? std::sin(phase) : (std::sin(phase) >= 0.0 ? 1.0 : -1.0);
        const auto output = static_cast<float>(value * gain * 0.25);

        for (int channel = 0; channel < numChannels; ++channel)
            buffer.addSample(channel, startSample + sample, output);

        phase += phaseIncrement;

        if (phase >= juce::MathConstants<double>::twoPi)
            phase -= juce::MathConstants<double>::twoPi;
    }
}
}
