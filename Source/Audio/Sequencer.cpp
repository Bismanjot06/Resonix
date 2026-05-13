#include "Sequencer.h"

namespace nebula::audio
{
Sequencer::Sequencer()
{
    clearPattern();
}

Sequencer::Sequencer(const Sequencer& other)
{
    const auto scoped = juce::ScopedLock(other.lock);
    pattern = other.pattern;
    bpm = other.bpm;
    secondsPerStep = other.secondsPerStep;
    accumulator = other.accumulator;
    playing = other.playing;
    currentStep = other.currentStep;
}

Sequencer& Sequencer::operator=(const Sequencer& other)
{
    if (this != &other)
    {
        const auto scoped = juce::ScopedLock(other.lock);
        pattern = other.pattern;
        bpm = other.bpm;
        secondsPerStep = other.secondsPerStep;
        accumulator = other.accumulator;
        playing = other.playing;
        currentStep = other.currentStep;

        const auto selfScoped = juce::ScopedLock(lock);
        triggeredTracks.clear();
    }

    return *this;
}

void Sequencer::setBpm(double newBpm)
{
    const auto scoped = juce::ScopedLock(lock);
    bpm = juce::jlimit(40.0, 240.0, newBpm);
    secondsPerStep = 60.0 / bpm / 4.0;
}

double Sequencer::getBpm() const
{
    const auto scoped = juce::ScopedLock(lock);
    return bpm;
}

void Sequencer::setPlaying(bool shouldPlay)
{
    const auto scoped = juce::ScopedLock(lock);
    playing = shouldPlay;
}

bool Sequencer::isPlaying() const
{
    const auto scoped = juce::ScopedLock(lock);
    return playing;
}

void Sequencer::reset()
{
    const auto scoped = juce::ScopedLock(lock);
    currentStep = 0;
    accumulator = 0.0;
    triggeredTracks.clear();
}

void Sequencer::clearPattern()
{
    const auto scoped = juce::ScopedLock(lock);

    for (auto& track : pattern)
        track.fill(false);
}

void Sequencer::toggleStep(int trackIndex, int stepIndex)
{
    const auto scoped = juce::ScopedLock(lock);

    if (juce::isPositiveAndBelow(trackIndex, constants::padCount) && juce::isPositiveAndBelow(stepIndex, constants::sequenceSteps))
        pattern[static_cast<size_t>(trackIndex)][static_cast<size_t>(stepIndex)] = !pattern[static_cast<size_t>(trackIndex)][static_cast<size_t>(stepIndex)];
}

void Sequencer::setStep(int trackIndex, int stepIndex, bool enabled)
{
    const auto scoped = juce::ScopedLock(lock);

    if (juce::isPositiveAndBelow(trackIndex, constants::padCount) && juce::isPositiveAndBelow(stepIndex, constants::sequenceSteps))
        pattern[static_cast<size_t>(trackIndex)][static_cast<size_t>(stepIndex)] = enabled;
}

bool Sequencer::getStep(int trackIndex, int stepIndex) const
{
    const auto scoped = juce::ScopedLock(lock);

    if (juce::isPositiveAndBelow(trackIndex, constants::padCount) && juce::isPositiveAndBelow(stepIndex, constants::sequenceSteps))
        return pattern[static_cast<size_t>(trackIndex)][static_cast<size_t>(stepIndex)];

    return false;
}

int Sequencer::getCurrentStep() const
{
    const auto scoped = juce::ScopedLock(lock);
    return currentStep;
}

Sequencer::PatternGrid Sequencer::getPattern() const
{
    const auto scoped = juce::ScopedLock(lock);
    return pattern;
}

void Sequencer::setPattern(const PatternGrid& newPattern)
{
    const auto scoped = juce::ScopedLock(lock);
    pattern = newPattern;
}

void Sequencer::update(double deltaSeconds)
{
    const auto scoped = juce::ScopedLock(lock);

    if (!playing)
        return;

    accumulator += deltaSeconds;

    while (accumulator >= secondsPerStep)
    {
        accumulator -= secondsPerStep;
        currentStep = (currentStep + 1) % constants::sequenceSteps;

        for (int track = 0; track < constants::padCount; ++track)
        {
            if (pattern[static_cast<size_t>(track)][static_cast<size_t>(currentStep)])
                triggeredTracks.push_back(track);
        }
    }
}

std::vector<int> Sequencer::consumeTriggeredTracks()
{
    const auto scoped = juce::ScopedLock(lock);
    auto tracks = triggeredTracks;
    triggeredTracks.clear();
    return tracks;
}

juce::var Sequencer::toVar() const
{
    const auto scoped = juce::ScopedLock(lock);
    juce::DynamicObject::Ptr root = new juce::DynamicObject();
    root->setProperty("bpm", bpm);
    root->setProperty("playing", playing);
    root->setProperty("currentStep", currentStep);

    juce::Array<juce::var> tracks;

    for (const auto& row : pattern)
    {
        juce::Array<juce::var> steps;

        for (const auto enabled : row)
            steps.add(enabled);

        tracks.add(juce::var(steps));
    }

    root->setProperty("pattern", juce::var(tracks));
    return juce::var(root.get());
}

bool Sequencer::fromVar(const juce::var& state)
{
    if (!state.isObject())
        return false;

    const auto scoped = juce::ScopedLock(lock);
    auto* object = state.getDynamicObject();

    if (object == nullptr)
        return false;

    bpm = juce::jlimit(40.0, 240.0, static_cast<double>(object->getProperty("bpm")));
    secondsPerStep = 60.0 / bpm / 4.0;
    playing = static_cast<bool>(object->getProperty("playing"));
    currentStep = juce::jlimit(0, constants::sequenceSteps - 1, static_cast<int>(object->getProperty("currentStep")));

    if (auto* array = object->getProperty("pattern").getArray())
    {
        for (int track = 0; track < juce::jmin(constants::padCount, array->size()); ++track)
        {
            if (auto* trackArray = (*array)[track].getArray())
            {
                for (int step = 0; step < juce::jmin(constants::sequenceSteps, trackArray->size()); ++step)
                    pattern[static_cast<size_t>(track)][static_cast<size_t>(step)] = static_cast<bool>((*trackArray)[step]);
            }
        }
    }

    return true;
}
}
