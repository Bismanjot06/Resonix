#include "Filter.h"

namespace nebula::dsp
{
void Filter::setSampleRate(double newSampleRate)
{
    sampleRate = juce::jmax(1.0, newSampleRate);
    updateCoefficient();
}

void Filter::setCutoff(float newCutoffHz)
{
    cutoffHz = juce::jlimit(40.0f, 20000.0f, newCutoffHz);
    updateCoefficient();
}

void Filter::reset()
{
    lastOutput = 0.0f;
}

float Filter::process(float input)
{
    lastOutput = coefficient * input + (1.0f - coefficient) * lastOutput;
    return lastOutput;
}

void Filter::updateCoefficient()
{
    const auto x = std::exp(-2.0 * juce::MathConstants<double>::pi * cutoffHz / sampleRate);
    coefficient = static_cast<float>(1.0 - x);
}
}
