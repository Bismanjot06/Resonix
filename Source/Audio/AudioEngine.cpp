#include "AudioEngine.h"

#include "../Utils/Helpers.h"

namespace nebula::audio
{
namespace
{
juce::AudioBuffer<float> makeClickSample(double sampleRate, float frequency, float amplitude, int lengthMs)
{
    const auto numSamples = juce::jmax(1, static_cast<int>(sampleRate * (lengthMs / 1000.0)));
    juce::AudioBuffer<float> buffer(1, numSamples);
    buffer.clear();

    double phase = 0.0;
    const auto phaseIncrement = juce::MathConstants<double>::twoPi * frequency / sampleRate;

    for (int sample = 0; sample < numSamples; ++sample)
    {
        const auto envelope = 1.0 - static_cast<double>(sample) / numSamples;
        buffer.setSample(0, sample, static_cast<float>(std::sin(phase) * amplitude * envelope));
        phase += phaseIncrement;
    }

    return buffer;
}
}

void AudioEngine::prepare(double newSampleRate, int maxBlockSize)
{
    sampleRate = juce::jmax(1.0, newSampleRate);
    blockSize = juce::jmax(1, maxBlockSize);

    oscillator.prepare(sampleRate);
    filter.setSampleRate(sampleRate);
    filter.setCutoff(12000.0f);
    effects.prepare(sampleRate);
    envelope.setSampleRate(sampleRate);
    envelope.setParameters(0.005f, 0.08f, 0.75f, 0.15f);

    loadSamples();
}

void AudioEngine::process(juce::AudioBuffer<float>& buffer)
{
    buffer.clear();

    const auto numSamples = buffer.getNumSamples();
    const auto deltaSeconds = static_cast<double>(numSamples) / sampleRate;

    sequencer.update(deltaSeconds);

    for (const auto track : sequencer.consumeTriggeredTracks())
        triggerPad(track, 0.85f);

    renderVoices(buffer);

    if (oscillatorEnabled)
        oscillator.render(buffer, 0, numSamples);

    for (int sample = 0; sample < numSamples; ++sample)
    {
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            const auto filtered = filter.process(buffer.getSample(channel, sample));
            buffer.setSample(channel, sample, filtered);
        }
    }

    effects.process(buffer);
    mixer.applyMaster(buffer);
}

void AudioEngine::triggerPad(int padIndex, float velocity)
{
    if (!juce::isPositiveAndBelow(padIndex, constants::padCount))
        return;

    const auto scoped = juce::ScopedLock(voiceLock);
    activeVoices.push_back(Voice { padIndex, 0, juce::jlimit(0.0f, 1.0f, velocity) });
}

void AudioEngine::setBpm(double newBpm)
{
    sequencer.setBpm(newBpm);
}

void AudioEngine::setMasterVolume(float newVolume)
{
    mixer.setMasterVolume(newVolume);
}

void AudioEngine::setMasterPan(float newPan)
{
    mixer.setMasterPan(newPan);
}

void AudioEngine::setPadVolume(int padIndex, float newVolume)
{
    mixer.setChannelVolume(padIndex, newVolume);
}

void AudioEngine::setPadPan(int padIndex, float newPan)
{
    mixer.setChannelPan(padIndex, newPan);
}

void AudioEngine::setOscillatorEnabled(bool shouldEnable)
{
    oscillatorEnabled = shouldEnable;
}

void AudioEngine::setOscillatorWaveform(Oscillator::Waveform waveform)
{
    oscillator.setWaveform(waveform);
}

void AudioEngine::setOscillatorFrequency(double frequencyHz)
{
    oscillator.setFrequency(frequencyHz);
}

void AudioEngine::setOscillatorGain(float gain)
{
    oscillator.setGain(gain);
}

void AudioEngine::toggleStep(int padIndex, int stepIndex)
{
    sequencer.toggleStep(padIndex, stepIndex);
}

bool AudioEngine::savePattern(const juce::File& destination) const
{
    if (destination.getParentDirectory().createDirectory().failed())
        return false;

    const auto json = juce::JSON::toString(sequencer.toVar(), true);
    return destination.replaceWithText(json);
}

bool AudioEngine::loadPattern(const juce::File& source)
{
    if (!source.existsAsFile())
        return false;

    const auto parsed = juce::JSON::parse(source);
    return sequencer.fromVar(parsed);
}

bool AudioEngine::exportWav(const juce::File& destination, int bars)
{
    const auto totalSamples = juce::jmax(1, static_cast<int>(bars * 4.0 * 60.0 / sequencer.getBpm() * sampleRate));
    auto rendered = renderOfflineBuffer(totalSamples);
    writeBufferToFile(rendered, destination);
    return destination.existsAsFile();
}

Sequencer& AudioEngine::getSequencer()
{
    return sequencer;
}

const Sequencer& AudioEngine::getSequencer() const
{
    return sequencer;
}

juce::StringArray AudioEngine::getPadNames() const
{
    return constants::padNames;
}

void AudioEngine::loadSamples()
{
    const auto assetFolder = helpers::resolveAssetFolder();

    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();

    const std::array<juce::String, constants::padCount> fileNames { "kick.wav", "snare.wav", "hats.wav", "clap.wav" };

    for (int padIndex = 0; padIndex < constants::padCount; ++padIndex)
    {
        const auto file = assetFolder.getChildFile(fileNames[static_cast<size_t>(padIndex)]);

        if (file.existsAsFile())
        {
            if (auto reader = std::unique_ptr<juce::AudioFormatReader>(formatManager.createReaderFor(file)))
            {
                padSamples[static_cast<size_t>(padIndex)].setSize(static_cast<int>(reader->numChannels), static_cast<int>(reader->lengthInSamples));
                reader->read(&padSamples[static_cast<size_t>(padIndex)], 0, static_cast<int>(reader->lengthInSamples), 0, true, true);
                continue;
            }
        }

        generateFallbackSample(padIndex);
    }
}

void AudioEngine::generateFallbackSample(int padIndex)
{
    switch (padIndex)
    {
        case 0: padSamples[0] = makeClickSample(sampleRate, 90.0, 0.95f, 120); break;
        case 1: padSamples[1] = makeClickSample(sampleRate, 220.0, 0.75f, 90); break;
        case 2: padSamples[2] = makeClickSample(sampleRate, 6000.0, 0.45f, 40); break;
        default: padSamples[3] = makeClickSample(sampleRate, 180.0, 0.65f, 70); break;
    }
}

void AudioEngine::renderVoices(juce::AudioBuffer<float>& buffer)
{
    const auto numSamples = buffer.getNumSamples();
    const auto scoped = juce::ScopedLock(voiceLock);

    std::vector<Voice> remainingVoices;
    remainingVoices.reserve(activeVoices.size());

    for (auto& voice : activeVoices)
    {
        const auto& sampleBuffer = padSamples[static_cast<size_t>(voice.padIndex)];

        for (int sample = 0; sample < numSamples; ++sample)
        {
            const auto sourceIndex = voice.position + sample;

            if (sourceIndex >= sampleBuffer.getNumSamples())
                break;

            const auto gain = getPadGain(voice.padIndex) * voice.velocity;
            const auto pan = getPadPan(voice.padIndex);
            const auto leftGain = 0.5f * (1.0f - pan);
            const auto rightGain = 0.5f * (1.0f + pan);

            const auto sampleValue = sampleBuffer.getSample(0, sourceIndex) * gain;

            if (buffer.getNumChannels() > 0)
                buffer.addSample(0, sample, sampleValue * leftGain);

            if (buffer.getNumChannels() > 1)
                buffer.addSample(1, sample, sampleValue * rightGain);
        }

        voice.position += numSamples;

        if (voice.position < sampleBuffer.getNumSamples())
            remainingVoices.push_back(voice);
    }

    activeVoices = std::move(remainingVoices);
}

juce::AudioBuffer<float> AudioEngine::renderOfflineBuffer(int totalSamples)
{
    juce::AudioBuffer<float> output(2, totalSamples);
    output.clear();

    auto sequencerCopy = sequencer;
    auto voices = activeVoices;

    int renderedSamples = 0;

    while (renderedSamples < totalSamples)
    {
        const auto block = juce::jmin(blockSize, totalSamples - renderedSamples);
        juce::AudioBuffer<float> temp(2, block);
        temp.clear();

        sequencerCopy.update(static_cast<double>(block) / sampleRate);

        for (const auto track : sequencerCopy.consumeTriggeredTracks())
            voices.push_back(Voice { track, 0, 1.0f });

        for (auto& voice : voices)
        {
            const auto& sampleBuffer = padSamples[static_cast<size_t>(voice.padIndex)];

            for (int sample = 0; sample < block; ++sample)
            {
                const auto sourceIndex = voice.position + sample;

                if (sourceIndex >= sampleBuffer.getNumSamples())
                    break;

                const auto sampleValue = sampleBuffer.getSample(0, sourceIndex) * getPadGain(voice.padIndex) * voice.velocity;
                temp.addSample(0, sample, sampleValue);

                if (temp.getNumChannels() > 1)
                    temp.addSample(1, sample, sampleValue);
            }

            voice.position += block;
        }

        if (oscillatorEnabled)
            oscillator.render(temp, 0, block);

        effects.process(temp);
        temp.applyGain(mixer.getMasterVolume());

        output.copyFrom(0, renderedSamples, temp, 0, 0, block);

        if (output.getNumChannels() > 1 && temp.getNumChannels() > 1)
            output.copyFrom(1, renderedSamples, temp, 1, 0, block);

        renderedSamples += block;
    }

    return output;
}

void AudioEngine::writeBufferToFile(const juce::AudioBuffer<float>& buffer, const juce::File& destination)
{
    if (destination.existsAsFile())
        destination.deleteFile();

    if (destination.getParentDirectory().createDirectory().failed())
        return;

    juce::WavAudioFormat wav;
    std::unique_ptr<juce::FileOutputStream> stream(destination.createOutputStream());

    if (stream == nullptr)
        return;

    if (auto writer = std::unique_ptr<juce::AudioFormatWriter>(wav.createWriterFor(stream.get(), sampleRate, static_cast<unsigned int>(buffer.getNumChannels()), 16, {}, 0)))
    {
        stream.release();
        writer->writeFromAudioSampleBuffer(buffer, 0, buffer.getNumSamples());
    }
}

float AudioEngine::getPadGain(int padIndex) const
{
    return mixer.getChannelState(padIndex).volume;
}

float AudioEngine::getPadPan(int padIndex) const
{
    return mixer.getChannelState(padIndex).pan;
}
}
