#pragma once

#include <JuceHeader.h>

#include "Mixer.h"
#include "Oscillator.h"
#include "Sequencer.h"
#include "../DSP/Effects.h"
#include "../DSP/Envelope.h"
#include "../DSP/Filter.h"
#include "../Utils/Constants.h"

namespace nebula::audio
{
class AudioEngine
{
public:
    void prepare(double newSampleRate, int maxBlockSize);
    void process(juce::AudioBuffer<float>& buffer);
    void triggerPad(int padIndex, float velocity = 1.0f);
    void setBpm(double newBpm);
    void setMasterVolume(float newVolume);
    void setMasterPan(float newPan);
    void setPadVolume(int padIndex, float newVolume);
    void setPadPan(int padIndex, float newPan);
    void setOscillatorEnabled(bool shouldEnable);
    void setOscillatorWaveform(Oscillator::Waveform waveform);
    void setOscillatorFrequency(double frequencyHz);
    void setOscillatorGain(float gain);
    void toggleStep(int padIndex, int stepIndex);
    bool savePattern(const juce::File& destination) const;
    bool loadPattern(const juce::File& source);
    bool exportWav(const juce::File& destination, int bars);
    Sequencer& getSequencer();
    const Sequencer& getSequencer() const;
    juce::StringArray getPadNames() const;

private:
    struct Voice
    {
        int padIndex = 0;
        int position = 0;
        float velocity = 1.0f;
    };

    void loadSamples();
    void generateFallbackSample(int padIndex);
    void renderVoices(juce::AudioBuffer<float>& buffer);
    juce::AudioBuffer<float> renderOfflineBuffer(int totalSamples);
    void writeBufferToFile(const juce::AudioBuffer<float>& buffer, const juce::File& destination);
    float getPadGain(int padIndex) const;
    float getPadPan(int padIndex) const;

    double sampleRate = constants::defaultSampleRate;
    int blockSize = 512;
    std::array<juce::AudioBuffer<float>, constants::padCount> padSamples;
    std::vector<Voice> activeVoices;
    mutable juce::CriticalSection voiceLock;
    Mixer mixer;
    Oscillator oscillator;
    Sequencer sequencer;
    nebula::dsp::Filter filter;
    nebula::dsp::Envelope envelope;
    nebula::dsp::Effects effects;
    bool oscillatorEnabled = true;
};
}
