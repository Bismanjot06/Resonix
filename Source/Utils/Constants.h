#pragma once

#include <JuceHeader.h>

namespace nebula::constants
{
inline constexpr double defaultSampleRate = 44100.0;
inline constexpr double defaultBpm = 120.0;
inline constexpr int padCount = 4;
inline constexpr int sequenceSteps = 16;
inline constexpr int defaultExportBars = 4;

inline const juce::StringArray padNames { "Kick", "Snare", "Hi-Hat", "Clap" };
inline const juce::StringArray waveformNames { "Sine", "Square" };
}
