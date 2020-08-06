
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
    FunFilterEditor(FunFilterAudioProcessor&, UiBroadcaster& broadcaster);
    ~FunFilterEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void valueChanged(double value) override;

  private:
    FunFilterAudioProcessor& audioProcessor;
    UiBroadcaster& broadcaster;

    double cutoffValue{0.};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FunFilterEditor)
};
