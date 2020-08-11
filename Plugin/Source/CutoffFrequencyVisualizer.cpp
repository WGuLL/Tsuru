
#include "CutoffFrequencyVisualizer.h"
#include "ColorPalette.h"

namespace
{
[[nodiscard]] NormalisableRange<float> frequencyRange(float min, float max) noexcept
{
    constexpr auto convertFrom0To1Func = [](auto min, auto max, auto value) {
        return juce::mapToLog10(value, min, max);
    };
    constexpr auto convertTo0To1Func = [](auto min, auto max, auto value) {
        return juce::mapFromLog10(value, min, max);
    };
    return {min, max, convertFrom0To1Func, convertTo0To1Func};
}
} // namespace

CutoffFrequencyVisualizer::CutoffFrequencyVisualizer(UiBroadcaster& broadcaster_) noexcept
    : range(frequencyRange(25.f, 20000.f))
    , broadcaster(broadcaster_)
{
    broadcaster.getValue<ValueIds::filterCutoff>().addListener(*this);
}

CutoffFrequencyVisualizer::~CutoffFrequencyVisualizer() noexcept
{
    broadcaster.getValue<ValueIds::filterCutoff>().removeListener(*this);
}

void CutoffFrequencyVisualizer::onBroadcastedValueChange(double value)
{
    cutoffValue = value;
    repaint();
}

void CutoffFrequencyVisualizer::drawFrequencyVerticalLine(int frequency,
                                                          juce::Graphics& g) const
{
    const auto linePosX = proportionOfWidth(range.convertTo0to1(frequency));
    g.drawVerticalLine(linePosX, 0, getHeight());
    g.drawText(juce::String(frequency), linePosX + proportionOfWidth(0.02f),
               proportionOfHeight(0.7f), proportionOfWidth(0.1f),
               proportionOfHeight(0.1f), juce::Justification::centredLeft);
}

void CutoffFrequencyVisualizer::drawFilterShape(int frequency, juce::Graphics& g) const
{
    const auto targetFrequencyPosX = proportionOfWidth(range.convertTo0to1(frequency));
    juce::Path p;
    p.startNewSubPath(0.f, proportionOfHeight(0.5f));
    p.quadraticTo(targetFrequencyPosX, proportionOfHeight(0.5f), targetFrequencyPosX,
                  proportionOfHeight(0.35f));
    p.quadraticTo(targetFrequencyPosX, proportionOfHeight(0.4f),
                  targetFrequencyPosX + proportionOfWidth(0.1f), getHeight());
    constexpr auto strokeThickness{2};
    g.strokePath(p, juce::PathStrokeType(strokeThickness));
}

void CutoffFrequencyVisualizer::paint(juce::Graphics& g)
{
    g.setColour(ColorPalette::mediumSlateBlue);
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(getHeight() * 0.01f),
                           getHeight() * 0.04f, getHeight() * 0.02f);
    for (const auto& graduatedFrequency : graduatedFrequencies)
    {
        drawFrequencyVerticalLine(graduatedFrequency, g);
    }

    g.setColour(ColorPalette::selectiveYellow);
    drawFilterShape(cutoffValue, g);
}
