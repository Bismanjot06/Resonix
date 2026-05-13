#pragma once

#include "Constants.h"

namespace nebula::helpers
{
inline float clamp01(float value)
{
    return juce::jlimit(0.0f, 1.0f, value);
}

inline float mapSliderToPan(double normalizedValue)
{
    return juce::jlimit(-1.0f, 1.0f, static_cast<float>(normalizedValue * 2.0 - 1.0));
}

inline int waveformIndexFromName(const juce::String& name)
{
    return constants::waveformNames.indexOf(name);
}

inline juce::String waveformNameFromIndex(int index)
{
    return constants::waveformNames[index];
}

inline juce::File resolveAssetFolder()
{
    const auto cwd = juce::File::getCurrentWorkingDirectory();
    const auto direct = cwd.getChildFile("Source").getChildFile("Assets");

    if (direct.exists())
        return direct;

    const auto parent = cwd.getParentDirectory().getChildFile("Source").getChildFile("Assets");
    return parent.exists() ? parent : direct;
}
}
