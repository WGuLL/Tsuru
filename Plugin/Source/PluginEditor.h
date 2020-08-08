
#pragma once

#include "PluginProcessor.h"
#include "UiBroadcaster.h"
#include <JuceHeader.h>

/**
 */
class FunFilterEditor : public juce::AudioProcessorEditor,
                        public UiBroadcastedValueListener
{
  public:
    FunFilterEditor(FunFilterAudioProcessor&, UiBroadcaster& broadcaster) noexcept;
    ~FunFilterEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void valueChanged(double value) override;

  private:
    /**
     * setColour has to be called first.
     */
    void drawFrequencyVerticalLine(int frequency, juce::Graphics& g) const;

    const juce::NormalisableRange<float> range;

    FunFilterAudioProcessor& audioProcessor;
    UiBroadcaster& broadcaster;

    double cutoffValue{0.};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FunFilterEditor)
};
