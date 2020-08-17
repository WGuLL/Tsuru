
#pragma once

#include "CutoffFrequencyVisualizer.h"
#include "JuceHeaderWrapper.h"
#include "RateKnob.h"
#include "ResonanceKnob.h"

class UiBroadcaster;
class FunFilterAudioProcessor;
/**
 * Main component of the plugin.
 */
class FunFilterEditor : public juce::AudioProcessorEditor
{
  public:
    FunFilterEditor(FunFilterAudioProcessor&, UiBroadcaster& broadcaster) noexcept;
    ~FunFilterEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

  private:
    FunFilterAudioProcessor& audioProcessor;
    CutoffFrequencyVisualizer cutoffVisualizer;
    ResonanceKnob resonanceKnob;
    RateKnob rateKnob;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FunFilterEditor)
};
