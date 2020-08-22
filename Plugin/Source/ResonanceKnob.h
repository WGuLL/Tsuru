
#pragma once

#include "JuceHeaderWrapper.h"
#include "UiBroadcaster.h"
/**
 * Allows the user to change the resonance
 * of the applied filter.
 */
class ResonanceKnob : public juce::Component, public UiBroadcastedValueListener
{
  public:
    ResonanceKnob(UiBroadcaster& broadcaster_,
                  juce::AudioProcessorParameter& parameter) noexcept;

    ~ResonanceKnob() noexcept;

    void onBroadcastedValueChange(double newValue) override;

    void resized() override;

  private:
    UiBroadcaster& broadcaster;
    juce::Label parameterNameLabel;
    juce::Slider knob;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ResonanceKnob)
};
