
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

void FunFilterEditor::paint(juce::Graphics& g)
{
    g.fillAll(darkSlateBlue);

    g.setColour(mediumSlateBlue);
    drawFrequencyVerticalLine(100, g);
    drawFrequencyVerticalLine(1000, g);
    drawFrequencyVerticalLine(10000, g);

    g.setColour(selectiveYellow);
    drawFrequencyVerticalLine(cutoffValue, g);
}

void FunFilterEditor::resized()
{
}
