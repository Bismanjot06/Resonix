#include "Knob.h"

namespace nebula::ui
{
Knob::Knob()
{
    setSliderStyle(juce::Slider::RotaryVerticalDrag);
    setTextBoxStyle(juce::Slider::TextBoxBelow, false, 72, 20);
    setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff00ff88));
    setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff556677));
    setColour(juce::Slider::thumbColourId, juce::Colour(0xffffffff));
    setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xff4a5866));
    setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff445566));
}
}
