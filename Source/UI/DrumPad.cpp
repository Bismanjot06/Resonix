#include "DrumPad.h"

namespace nebula::ui
{
DrumPad::DrumPad(const juce::String& labelText)
    : juce::TextButton(labelText)
{
    setClickingTogglesState(false);
    setColour(juce::TextButton::buttonColourId, juce::Colour(0xff445566));
    setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff4a5866));
    refreshAppearance();
}

void DrumPad::flash()
{
    flashed = true;
    refreshAppearance();
    juce::Timer::callAfterDelay(120, [safe = juce::Component::SafePointer<DrumPad>(this)]
    {
        if (safe != nullptr)
        {
            safe->flashed = false;
            safe->refreshAppearance();
        }
    });
}

void DrumPad::setSelected(bool shouldSelect)
{
    selected = shouldSelect;
    refreshAppearance();
}

void DrumPad::refreshAppearance()
{
    auto base = juce::Colour(0xff445566);
    auto accent = juce::Colour(0xff00ff88);

    if (selected)
        base = juce::Colour(0xff556677);

    if (flashed)
        base = accent.withAlpha(0.95f);

    setColour(juce::TextButton::buttonColourId, base);
}
}
