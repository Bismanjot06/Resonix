#include "Mixer.h"

namespace nebula::audio
{
void Mixer::setMasterVolume(float newVolume)
{
    masterVolume = juce::jlimit(0.0f, 1.0f, newVolume);
}

void Mixer::setMasterPan(float newPan)
{
    masterPan = juce::jlimit(-1.0f, 1.0f, newPan);
}

void Mixer::setChannelVolume(int channelIndex, float newVolume)
{
    if (juce::isPositiveAndBelow(channelIndex, static_cast<int>(channels.size())))
        channels[static_cast<size_t>(channelIndex)].volume = juce::jlimit(0.0f, 1.0f, newVolume);
}

void Mixer::setChannelPan(int channelIndex, float newPan)
{
    if (juce::isPositiveAndBelow(channelIndex, static_cast<int>(channels.size())))
        channels[static_cast<size_t>(channelIndex)].pan = juce::jlimit(-1.0f, 1.0f, newPan);
}

float Mixer::getMasterVolume() const
{
    return masterVolume;
}

float Mixer::getMasterPan() const
{
    return masterPan;
}

Mixer::ChannelState Mixer::getChannelState(int channelIndex) const
{
    if (juce::isPositiveAndBelow(channelIndex, static_cast<int>(channels.size())))
        return channels[static_cast<size_t>(channelIndex)];

    return {};
}

void Mixer::mixMono(juce::AudioBuffer<float>& buffer, int startSample, int numSamples, float input, int channelIndex) const
{
    const auto state = getChannelState(channelIndex);
    const auto volume = state.volume * masterVolume;
    const auto pan = juce::jlimit(-1.0f, 1.0f, state.pan + masterPan * 0.5f);
    const auto leftGain = 0.5f * (1.0f - pan);
    const auto rightGain = 0.5f * (1.0f + pan);

    for (int sample = 0; sample < numSamples; ++sample)
    {
        const auto value = input * volume;

        if (buffer.getNumChannels() > 0)
            buffer.addSample(0, startSample + sample, value * leftGain);

        if (buffer.getNumChannels() > 1)
            buffer.addSample(1, startSample + sample, value * rightGain);
    }
}

void Mixer::applyMaster(juce::AudioBuffer<float>& buffer) const
{
    buffer.applyGain(masterVolume);

    if (juce::approximatelyEqual(masterPan, 0.0f) || buffer.getNumChannels() < 2)
        return;

    const auto leftAttenuation = masterPan > 0.0f ? 1.0f - masterPan : 1.0f;
    const auto rightAttenuation = masterPan < 0.0f ? 1.0f + masterPan : 1.0f;

    buffer.applyGain(0, 0, buffer.getNumSamples(), leftAttenuation);
    buffer.applyGain(1, 0, buffer.getNumSamples(), rightAttenuation);
}
}
