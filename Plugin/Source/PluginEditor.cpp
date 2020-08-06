
#include "PluginEditor.h"
#include "PluginProcessor.h"

namespace
{
const juce::Colour charcoal(55, 63, 81);
const juce::Colour cadetBlue(88, 164, 176);
const juce::Colour lightSteelBlue(169, 188, 208);
const juce::Colour gainsBoro(216, 219, 226);
const juce::Colour pastelPink(218, 164, 154);
} // namespace

FunFilterEditor::FunFilterEditor(FunFilterAudioProcessor& p, UiBroadcaster& b)
    : AudioProcessorEditor(&p)
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

void FunFilterEditor::paint(juce::Graphics& g)
{
    g.fillAll(charcoal);
    g.setColour(pastelPink);
    g.drawText(std::to_string(cutoffValue), getLocalBounds(),
               juce::Justification::centred);
}

void FunFilterEditor::resized()
{
}
