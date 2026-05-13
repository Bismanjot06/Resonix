#pragma once

#include <JuceHeader.h>

#include "../Utils/Constants.h"

namespace nebula::dsp
{
class Filter
{
public:
    void setSampleRate(double newSampleRate);
    void setCutoff(float newCutoffHz);
    void reset();
    float process(float input);

private:
    double sampleRate = constants::defaultSampleRate;
    float cutoffHz = 12000.0f;
    float coefficient = 0.0f;
    float lastOutput = 0.0f;

    void updateCoefficient();
};
}
