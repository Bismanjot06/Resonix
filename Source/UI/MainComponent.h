#pragma once

#include <JuceHeader.h>

#include "../Audio/AudioEngine.h"
#include "DrumPad.h"
#include "Knob.h"
#include "Timeline.h"

namespace nebula::ui
{
class MainComponent : public juce::Component,
                      private juce::Timer
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    nebula::audio::AudioEngine& getAudioEngine() { return engine; }

private:
    void timerCallback() override;
    void handleSavePattern();
    void handleLoadPattern();
    void handleExportWav();
    void syncTimeline();
    void initAudioEngine();

    nebula::audio::AudioEngine engine;
    double sampleRate = 44100.0;
    int blockSize = 512;
    juce::OwnedArray<DrumPad> pads;
    Knob masterVolumeKnob;
    Knob masterPanKnob;
    Knob bpmKnob;
    Knob oscillatorFrequencyKnob;
    Knob oscillatorGainKnob;
    juce::ComboBox waveformBox;
    juce::TextButton saveButton { "Save Pattern" };
    juce::TextButton loadButton { "Load Pattern" };
    juce::TextButton exportButton { "Export WAV" };
    juce::TextButton playButton { "Play" };
    Timeline timeline;
    juce::Label titleLabel;
};
}
