
#pragma once

#include "JuceHeaderWrapper.h"
#include "UiBroadcaster.h"

/**
 *  Modifies the duration of the filter choregraphy.
 */
class RateKnob : public juce::Component, public UiBroadcastedValueListener
{
  public:
    RateKnob(UiBroadcaster& broadcaster_,
             juce::AudioProcessorParameter& parameter) noexcept;
    ~RateKnob() noexcept;
    void onBroadcastedValueChange(double newValue) override;
    void resized() override;

  private:
    UiBroadcaster& broadcaster;
    juce::Label parameterNameLabel;
    juce::Slider knob;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RateKnob)
};
