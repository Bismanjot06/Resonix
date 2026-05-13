#pragma once

#include <JuceHeader.h>

namespace nebula::ui
{
class DrumPad : public juce::TextButton
{
public:
    explicit DrumPad(const juce::String& labelText);

    void flash();
    void setSelected(bool shouldSelect);

private:
    void refreshAppearance();

    bool selected = false;
    bool flashed = false;
};
}
