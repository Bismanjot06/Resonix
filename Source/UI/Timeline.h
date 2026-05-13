#pragma once

#include <JuceHeader.h>

#include "../Audio/Sequencer.h"

namespace nebula::ui
{
class Timeline : public juce::Component
{
public:
    void setPattern(const nebula::audio::Sequencer::PatternGrid& newPattern);
    void setCurrentStep(int newStep);
    void paint(juce::Graphics& g) override;

private:
    nebula::audio::Sequencer::PatternGrid pattern {};
    int currentStep = 0;
};
}
