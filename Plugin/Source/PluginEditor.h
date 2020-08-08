
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
    void drawFilterShape(int frequency, juce::Graphics& g) const;

    const juce::NormalisableRange<float> range;
    static constexpr std::array<int, 3> graduatedFrequencies{{100, 1000, 10000}};

    FunFilterAudioProcessor& audioProcessor;
    UiBroadcaster& broadcaster;

    double cutoffValue{1000};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FunFilterEditor)
};
