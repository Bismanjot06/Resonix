#include "MainComponent.h"

#include "../Utils/Helpers.h"
#include <fstream>

namespace nebula::ui
{
MainComponent::MainComponent()
{
    DBG("MainComponent: constructor start");
    setSize(1240, 760);
    setOpaque(true);

    addAndMakeVisible(titleLabel);
    titleLabel.setText("Resonix", juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centredLeft);
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    
    juce::Font titleFont(juce::FontOptions(28.0f));
    titleFont = titleFont.boldened();
    titleLabel.setFont(titleFont);

    addAndMakeVisible(saveButton);
    addAndMakeVisible(loadButton);
    addAndMakeVisible(exportButton);
    addAndMakeVisible(playButton);

    DBG("MainComponent: added basic controls");

    for (auto* btn : {&saveButton, &loadButton, &exportButton, &playButton})
    {
        btn->setColour(juce::TextButton::buttonColourId, juce::Colour(0xff445566));
        btn->setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        btn->setColour(juce::TextButton::textColourOnId, juce::Colours::white);
        btn->setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff4a5866));
    }

    saveButton.onClick = [this] { handleSavePattern(); };
    loadButton.onClick = [this] { handleLoadPattern(); };
    exportButton.onClick = [this] { handleExportWav(); };
    playButton.onClick = [this]
    {
        auto& sequencer = engine.getSequencer();
        sequencer.setPlaying(!sequencer.isPlaying());
        playButton.setButtonText(sequencer.isPlaying() ? "Pause" : "Play");
    };

    waveformBox.addItemList(constants::waveformNames, 1);
    waveformBox.setSelectedId(1);
    waveformBox.onChange = [this]
    {
        engine.setOscillatorWaveform(static_cast<nebula::audio::Oscillator::Waveform>(waveformBox.getSelectedItemIndex()));
    };
    waveformBox.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff445566));
    waveformBox.setColour(juce::ComboBox::textColourId, juce::Colours::white);
    waveformBox.setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff4a5866));
    waveformBox.setColour(juce::ComboBox::focusedOutlineColourId, juce::Colour(0xff00ff88));
    addAndMakeVisible(waveformBox);

    auto makeKnob = [this](Knob& knob, const juce::String& name)
    {
        addAndMakeVisible(knob);
        knob.setName(name);
    };

    makeKnob(masterVolumeKnob, "Volume");
    makeKnob(masterPanKnob, "Pan");
    makeKnob(bpmKnob, "BPM");
    makeKnob(oscillatorFrequencyKnob, "Freq");
    makeKnob(oscillatorGainKnob, "Osc");

    masterVolumeKnob.setRange(0.0, 1.0, 0.001);
    masterVolumeKnob.setValue(0.85);
    masterVolumeKnob.onValueChange = [this] { engine.setMasterVolume(static_cast<float>(masterVolumeKnob.getValue())); };

    masterPanKnob.setRange(-1.0, 1.0, 0.001);
    masterPanKnob.setValue(0.0);
    masterPanKnob.onValueChange = [this] { engine.setMasterPan(static_cast<float>(masterPanKnob.getValue())); };

    bpmKnob.setRange(60.0, 180.0, 0.1);
    bpmKnob.setValue(constants::defaultBpm);
    bpmKnob.onValueChange = [this]
    {
        const auto bpm = bpmKnob.getValue();
        engine.setBpm(bpm);
    };

    oscillatorFrequencyKnob.setRange(55.0, 880.0, 0.1);
    oscillatorFrequencyKnob.setValue(220.0);
    oscillatorFrequencyKnob.onValueChange = [this] { engine.setOscillatorFrequency(oscillatorFrequencyKnob.getValue()); };

    oscillatorGainKnob.setRange(0.0, 1.0, 0.001);
    oscillatorGainKnob.setValue(0.35);
    oscillatorGainKnob.onValueChange = [this] { engine.setOscillatorGain(static_cast<float>(oscillatorGainKnob.getValue())); };

    addAndMakeVisible(timeline);

    for (int padIndex = 0; padIndex < constants::padCount; ++padIndex)
    {
        auto* pad = pads.add(new DrumPad(constants::padNames[padIndex]));
        addAndMakeVisible(pad);
        pad->onClick = [this, padIndex]
        {
            engine.triggerPad(padIndex, 1.0f);
            pads[padIndex]->flash();
        };
    }

    DBG("MainComponent: finished constructor and called resized");

    // Ensure all controls get valid bounds before the first frame.
    resized();

    // Dump runtime bounds to a file for debugging (helps diagnose blank UI)
    try
    {
        std::ofstream dbg("/tmp/resonix_ui_debug.txt");
        dbg << "MainComponent constructed\n";
        dbg << "size=" << getWidth() << "x" << getHeight() << "\n";
        dbg << "titleLabel bounds=" << titleLabel.getBounds().toString().toStdString() << "\n";
        dbg << "pads_count=" << pads.size() << "\n";
        for (int i = 0; i < pads.size(); ++i)
            dbg << "pad_" << i << " bounds=" << pads[i]->getBounds().toString().toStdString() << " visible=" << pads[i]->isShowing() << "\n";
        dbg << "timeline bounds=" << timeline.getBounds().toString().toStdString() << "\n";
        dbg.close();
    }
    catch (...) {}

    initAudioEngine();
    startTimerHz(30);
    DBG("MainComponent: constructor complete");
    setVisible(true);
    repaint();
}

MainComponent::~MainComponent()
{
}

void MainComponent::initAudioEngine()
{
    // Audio will be set up by MainWindow when it's ready with device info
    // For now, just prepare with default settings
    engine.prepare(sampleRate, blockSize);
    engine.setOscillatorFrequency(220.0);
    engine.setOscillatorGain(static_cast<float>(oscillatorGainKnob.getValue()));
    engine.setOscillatorWaveform(static_cast<nebula::audio::Oscillator::Waveform>(waveformBox.getSelectedItemIndex()));
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff333333));

    // Temporary debug overlay to ensure paint is executing and visible
    try
    {
        std::ofstream dbg("/tmp/resonix_paint_debug.txt");
        dbg << "paint called\n";
        dbg << "size=" << getWidth() << "x" << getHeight() << "\n";
        dbg.close();
    }
    catch (...) {}

    g.setColour(juce::Colours::red);
    g.fillRect(8, 8, 120, 40);
    g.setColour(juce::Colours::white);
    g.setFont(18.0f);
    g.drawSingleLineText("Resonix", 14, 36);
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds().reduced(18);
    auto top = bounds.removeFromTop(72);
    titleLabel.setBounds(top.removeFromLeft(280));

    auto transport = top.removeFromRight(420);
    saveButton.setBounds(transport.removeFromLeft(110).reduced(2));
    loadButton.setBounds(transport.removeFromLeft(110).reduced(2));
    exportButton.setBounds(transport.removeFromLeft(110).reduced(2));
    playButton.setBounds(transport.removeFromLeft(80).reduced(2));

    auto controls = bounds.removeFromTop(170);
    auto controlColumns = controls;
    const auto columnWidth = controlColumns.getWidth() / 5;

    masterVolumeKnob.setBounds(controlColumns.removeFromLeft(columnWidth).reduced(12));
    masterPanKnob.setBounds(controlColumns.removeFromLeft(columnWidth).reduced(12));
    bpmKnob.setBounds(controlColumns.removeFromLeft(columnWidth).reduced(12));
    oscillatorFrequencyKnob.setBounds(controlColumns.removeFromLeft(columnWidth).reduced(12));
    oscillatorGainKnob.setBounds(controlColumns.removeFromLeft(columnWidth).reduced(12));

    waveformBox.setBounds(bounds.removeFromTop(34).removeFromLeft(180));

    auto padArea = bounds.removeFromTop(130);
    const auto padWidth = padArea.getWidth() / constants::padCount;

    for (int padIndex = 0; padIndex < pads.size(); ++padIndex)
        pads[padIndex]->setBounds(padArea.removeFromLeft(padWidth).reduced(8));

    timeline.setBounds(bounds.reduced(0, 8));
}

void MainComponent::timerCallback()
{
    syncTimeline();
}

void MainComponent::handleSavePattern()
{
    juce::FileChooser chooser("Save pattern", juce::File(), "*.json");

    if (chooser.browseForFileToSave(true))
        engine.savePattern(chooser.getResult());
}

void MainComponent::handleLoadPattern()
{
    juce::FileChooser chooser("Load pattern", juce::File(), "*.json");

    if (chooser.browseForFileToOpen())
        engine.loadPattern(chooser.getResult());
}

void MainComponent::handleExportWav()
{
    juce::FileChooser chooser("Export WAV", juce::File(), "*.wav");

    if (chooser.browseForFileToSave(true))
        engine.exportWav(chooser.getResult(), constants::defaultExportBars);
}

void MainComponent::syncTimeline()
{
    const auto& sequencer = engine.getSequencer();
    timeline.setPattern(sequencer.getPattern());
    timeline.setCurrentStep(sequencer.getCurrentStep());
    playButton.setButtonText(sequencer.isPlaying() ? "Pause" : "Play");
}
}
