
#include "PluginEditor.h"
#include "PluginProcessor.h"

FunFilterEditor::FunFilterEditor(FunFilterAudioProcessor& p)
    : AudioProcessorEditor(&p)
    , audioProcessor(p)
{
    setSize(400, 300);
}

FunFilterEditor::~FunFilterEditor() = default;

void FunFilterEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    g.drawFittedText("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void FunFilterEditor::resized()
{
}
