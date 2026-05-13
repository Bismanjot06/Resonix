#include "Envelope.h"

namespace nebula::dsp
{
void Envelope::setSampleRate(double newSampleRate)
{
    sampleRate = juce::jmax(1.0, newSampleRate);
}

void Envelope::setParameters(float attackSeconds, float decaySeconds, float sustainLevel, float releaseSeconds)
{
    attack = juce::jmax(0.001f, attackSeconds);
    decay = juce::jmax(0.001f, decaySeconds);
    sustain = juce::jlimit(0.0f, 1.0f, sustainLevel);
    release = juce::jmax(0.001f, releaseSeconds);
}

void Envelope::noteOn()
{
    stage = Stage::attack;
}

void Envelope::noteOff()
{
    if (stage != Stage::idle)
        stage = Stage::release;
}

float Envelope::process()
{
    switch (stage)
    {
        case Stage::idle:
            level = 0.0f;
            break;
        case Stage::attack:
            level += 1.0f / static_cast<float>(attack * sampleRate);
            if (level >= 1.0f)
            {
                level = 1.0f;
                stage = Stage::decay;
            }
            break;
        case Stage::decay:
            level -= (1.0f - sustain) / static_cast<float>(decay * sampleRate);
            if (level <= sustain)
            {
                level = sustain;
                stage = Stage::sustain;
            }
            break;
        case Stage::sustain:
            level = sustain;
            break;
        case Stage::release:
            level -= sustain / static_cast<float>(release * sampleRate);
            if (level <= 0.0f)
            {
                level = 0.0f;
                stage = Stage::idle;
            }
            break;
    }

    return juce::jlimit(0.0f, 1.0f, level);
}

bool Envelope::isIdle() const
{
    return stage == Stage::idle;
}
}
