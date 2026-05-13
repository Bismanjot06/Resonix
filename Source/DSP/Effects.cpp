#include "Effects.h"

namespace nebula::dsp
{
void Effects::prepare(double newSampleRate)
{
    sampleRate = juce::jmax(1.0, newSampleRate);
    delayBuffer.setSize(2, static_cast<int>(sampleRate * 2.0));
    delayBuffer.clear();
    writePosition = 0;
}

void Effects::setDrive(float newDrive)
{
    drive = juce::jlimit(0.0f, 1.0f, newDrive);
}

void Effects::setWet(float newWet)
{
    wet = juce::jlimit(0.0f, 1.0f, newWet);
}

void Effects::process(juce::AudioBuffer<float>& buffer)
{
    if (delayBuffer.getNumSamples() == 0)
        return;

    const auto numSamples = buffer.getNumSamples();
    const auto numChannels = juce::jmin(buffer.getNumChannels(), delayBuffer.getNumChannels());

    for (int sample = 0; sample < numSamples; ++sample)
    {
        for (int channel = 0; channel < numChannels; ++channel)
        {
            const auto dry = buffer.getSample(channel, sample);
            const auto delayed = delayBuffer.getSample(channel, writePosition);
            const auto saturated = std::tanh(dry * (1.0f + drive * 4.0f));
            const auto mixed = dry * (1.0f - wet) + (saturated * 0.8f + delayed * 0.2f) * wet;

            buffer.setSample(channel, sample, mixed);
            delayBuffer.setSample(channel, writePosition, mixed * 0.55f);
        }

        writePosition = (writePosition + 1) % delayBuffer.getNumSamples();
    }
}
}
