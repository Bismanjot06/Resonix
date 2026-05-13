#include "Timeline.h"

namespace nebula::ui
{
void Timeline::setPattern(const nebula::audio::Sequencer::PatternGrid& newPattern)
{
    pattern = newPattern;
    repaint();
}

void Timeline::setCurrentStep(int newStep)
{
    currentStep = newStep;
    repaint();
}

void Timeline::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff373f4f));

    auto area = getLocalBounds().toFloat().reduced(8.0f);
    const auto rowHeight = area.getHeight() / static_cast<float>(constants::padCount);
    const auto columnWidth = area.getWidth() / static_cast<float>(constants::sequenceSteps);

    g.setColour(juce::Colour(0xff4a5866));
    g.drawRoundedRectangle(area, 10.0f, 2.0f);

    for (int row = 0; row < constants::padCount; ++row)
    {
        for (int column = 0; column < constants::sequenceSteps; ++column)
        {
            const auto cell = juce::Rectangle<float>(
                area.getX() + column * columnWidth + 3.0f,
                area.getY() + row * rowHeight + 3.0f,
                columnWidth - 6.0f,
                rowHeight - 6.0f);

            const bool enabled = pattern[static_cast<size_t>(row)][static_cast<size_t>(column)];
            const bool current = currentStep == column;

            g.setColour(enabled ? juce::Colour(0xff00ff88) : juce::Colour(0xff4a5866));
            g.fillRoundedRectangle(cell, 4.0f);
            g.setColour(juce::Colours::white);
            g.drawRoundedRectangle(cell, 4.0f, 1.0f);

            if (current)
            {
                g.setColour(juce::Colour(0xffffffff).withAlpha(0.35f));
                g.drawRoundedRectangle(cell.expanded(1.0f), 5.0f, 1.4f);
            }
        }
    }
}
}
