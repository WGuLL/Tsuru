
#include "PluginEditor.h"
#include "PluginProcessor.h"
#include "UiBroadcaster.h"

FunFilterEditor::FunFilterEditor(FunFilterAudioProcessor& p,
                                 UiBroadcaster& broadcaster) noexcept
    : AudioProcessorEditor(&p)
    , audioProcessor(p)
    , cutoffVisualizer(broadcaster)
    , resonanceKnob(broadcaster,
                    [&p](const double resonance) { p.setFilterResonance(resonance); })
{
    addAndMakeVisible(cutoffVisualizer);
    addAndMakeVisible(resonanceKnob);

    setSize(400, 300);
}

FunFilterEditor::~FunFilterEditor() = default;

void FunFilterEditor::paint([[maybe_unused]] juce::Graphics& g)
{
}

void FunFilterEditor::resized()
{
    auto areaLeftToFill = getLocalBounds();
    cutoffVisualizer.setBounds(areaLeftToFill.removeFromTop(proportionOfHeight(0.8f)));
    resonanceKnob.setBounds(areaLeftToFill);
}
