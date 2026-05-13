#pragma once

#include <JuceHeader.h>

#include "../Utils/Constants.h"

namespace nebula::audio
{
class Sequencer
{
public:
    using PatternGrid = std::array<std::array<bool, constants::sequenceSteps>, constants::padCount>;

    Sequencer();
    Sequencer(const Sequencer& other);
    Sequencer& operator=(const Sequencer& other);

    void setBpm(double newBpm);
    double getBpm() const;
    void setPlaying(bool shouldPlay);
    bool isPlaying() const;
    void reset();
    void clearPattern();
    void toggleStep(int trackIndex, int stepIndex);
    void setStep(int trackIndex, int stepIndex, bool enabled);
    bool getStep(int trackIndex, int stepIndex) const;
    int getCurrentStep() const;
    PatternGrid getPattern() const;
    void setPattern(const PatternGrid& newPattern);
    void update(double deltaSeconds);
    std::vector<int> consumeTriggeredTracks();
    juce::var toVar() const;
    bool fromVar(const juce::var& state);

private:
    mutable juce::CriticalSection lock;
    PatternGrid pattern {};
    std::vector<int> triggeredTracks;
    double bpm = constants::defaultBpm;
    double secondsPerStep = 60.0 / constants::defaultBpm / 4.0;
    double accumulator = 0.0;
    bool playing = true;
    int currentStep = 0;
};
}
