
#include "PluginEditor.h"
#include "ColorPalette.h"
#include "PluginProcessor.h"
#include "UiBroadcaster.h"

FunFilterEditor::FunFilterEditor(FunFilterAudioProcessor& p,
                                 UiBroadcaster& broadcaster) noexcept
    : AudioProcessorEditor(&p)
    , audioProcessor(p)
    , cutoffVisualizer(broadcaster)
    , resonanceKnob(broadcaster, p.getParameterFromName("FilterResonance"))
{
    addAndMakeVisible(cutoffVisualizer);
    addAndMakeVisible(resonanceKnob);

    setSize(400, 300);
}

FunFilterEditor::~FunFilterEditor() = default;

void FunFilterEditor::paint(juce::Graphics& g)
{
    g.fillAll(ColorPalette::darkSlateBlue);
}

void FunFilterEditor::resized()
{
    auto areaLeftToFill = getLocalBounds();
    cutoffVisualizer.setBounds(areaLeftToFill.removeFromTop(proportionOfHeight(0.8f))
                                   .reduced(proportionOfWidth(0.05f)));
    resonanceKnob.setBounds(areaLeftToFill.expanded(proportionOfHeight(0.02f))
                                .translated(0, proportionOfHeight(-0.035f)));
}
