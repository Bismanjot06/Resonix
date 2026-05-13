#pragma once

#include <JuceHeader.h>

#include "../Utils/Constants.h"

namespace nebula::dsp
{
class Envelope
{
public:
    void setSampleRate(double newSampleRate);
    void setParameters(float attackSeconds, float decaySeconds, float sustainLevel, float releaseSeconds);
    void noteOn();
    void noteOff();
    float process();
    bool isIdle() const;

private:
    enum class Stage
    {
        idle,
        attack,
        decay,
        sustain,
        release
    };

    double sampleRate = constants::defaultSampleRate;
    float attack = 0.01f;
    float decay = 0.15f;
    float sustain = 0.7f;
    float release = 0.2f;
    float level = 0.0f;
    Stage stage = Stage::idle;
};
}
