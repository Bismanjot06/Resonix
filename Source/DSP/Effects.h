#pragma once

#include <JuceHeader.h>

#include "../Utils/Constants.h"

namespace nebula::dsp
{
class Effects
{
public:
    void prepare(double newSampleRate);
    void setDrive(float newDrive);
    void setWet(float newWet);
    void process(juce::AudioBuffer<float>& buffer);

private:
    double sampleRate = constants::defaultSampleRate;
    float drive = 0.2f;
    float wet = 0.15f;
    juce::AudioBuffer<float> delayBuffer;
    int writePosition = 0;
};
}
