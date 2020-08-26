
#include "CutoffFrequencyVisualizer.h"
#include "ColorPalette.h"
#include "MathUtils.h"

CutoffFrequencyVisualizer::CutoffFrequencyVisualizer(UiBroadcaster& broadcaster_) noexcept
    : range(MathUtils::frequencyRange<float>())
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

void CutoffFrequencyVisualizer::drawFrequencyVerticalLine(float frequency,
                                                          juce::Graphics& g) const
{
    const auto linePosX = proportionOfWidth(range.convertTo0to1(frequency));
    g.drawVerticalLine(linePosX, 0.f, static_cast<float>(getHeight()));
    g.drawText(juce::String(frequency), linePosX + proportionOfWidth(0.02f),
               proportionOfHeight(0.7f), proportionOfWidth(0.1f),
               proportionOfHeight(0.1f), juce::Justification::centredLeft);
}

void CutoffFrequencyVisualizer::drawFilterShape(float frequency, juce::Graphics& g) const
{
    const auto w = static_cast<float>(getWidth ());
    const auto h = static_cast<float>(getHeight ());

    const auto targetFrequencyPosX = w * range.convertTo0to1(frequency);
    juce::Path p;

    p.startNewSubPath(0.f, h * 0.5f);
    p.quadraticTo(targetFrequencyPosX, h * 0.5f, targetFrequencyPosX,
                h *0.35f);
    p.quadraticTo(targetFrequencyPosX, h *0.4f,
                  targetFrequencyPosX + w * 0.1f, h);
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
    drawFilterShape(static_cast<float>(cutoffValue), g);
}
