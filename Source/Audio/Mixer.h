#pragma once

#include <JuceHeader.h>

#include "../Utils/Constants.h"

namespace nebula::audio
{
class Mixer
{
public:
    struct ChannelState
    {
        float volume = 1.0f;
        float pan = 0.0f;
    };

    void setMasterVolume(float newVolume);
    void setMasterPan(float newPan);
    void setChannelVolume(int channelIndex, float newVolume);
    void setChannelPan(int channelIndex, float newPan);
    float getMasterVolume() const;
    float getMasterPan() const;
    ChannelState getChannelState(int channelIndex) const;
    void mixMono(juce::AudioBuffer<float>& buffer, int startSample, int numSamples, float input, int channelIndex) const;
    void applyMaster(juce::AudioBuffer<float>& buffer) const;

private:
    float masterVolume = 0.85f;
    float masterPan = 0.0f;
    std::array<ChannelState, constants::padCount> channels {};
};
}
