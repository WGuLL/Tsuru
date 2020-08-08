
#include "PluginEditor.h"
#include "PluginProcessor.h"

namespace
{
const juce::Colour darkSlateBlue(61, 52, 139);
const juce::Colour mediumSlateBlue(118, 120, 237);
const juce::Colour selectiveYellow(247, 184, 1);
const juce::Colour tangerine(241, 135, 1);
const juce::Colour orangePantone(243, 91, 4);
[[nodiscard]] NormalisableRange<float>
frequencyRange(float min, float max, float interval) noexcept
{
    return {min, max, interval, 1.f / std::log2(1.f + std::sqrt(max / min))};
}
} // namespace

FunFilterEditor::FunFilterEditor(FunFilterAudioProcessor& p, UiBroadcaster& b) noexcept
    : AudioProcessorEditor(&p)
    , range(frequencyRange(25, 20000, 1))
    , audioProcessor(p)
    , broadcaster(b)
{
    broadcaster.getValue<ValueIds::filterCutoff>().addListener(*this);
    setSize(400, 300);
}

FunFilterEditor::~FunFilterEditor()
{
    broadcaster.getValue<ValueIds::filterCutoff>().removeListener(*this);
}

void FunFilterEditor::valueChanged(double value)
{
    cutoffValue = value;
    repaint();
}

void FunFilterEditor::drawFrequencyVerticalLine(int frequency, juce::Graphics& g) const
{
    const auto linePosX = proportionOfWidth(range.convertTo0to1(frequency));
    g.drawVerticalLine(linePosX, 0, getHeight());
    g.drawText(juce::String(frequency), linePosX, proportionOfHeight(0.7f),
               proportionOfWidth(0.1f), proportionOfHeight(0.1f),
               juce::Justification::centredLeft);
}

void FunFilterEditor::drawFilterShape(int frequency, juce::Graphics& g) const
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

void FunFilterEditor::paint(juce::Graphics& g)
{
    g.fillAll(darkSlateBlue);

    g.setColour(mediumSlateBlue);
    g.drawRoundedRectangle(getLocalBounds().toFloat(), getHeight() * 0.05f,
                           getHeight() * 0.05f);
    for (const auto& graduatedFrequency : graduatedFrequencies)
    {
        drawFrequencyVerticalLine(graduatedFrequency, g);
    }

    g.setColour(selectiveYellow);
    drawFilterShape(cutoffValue, g);
}

void FunFilterEditor::resized()
{
}
