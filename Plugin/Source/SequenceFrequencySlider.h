
#pragma once

#include "JuceHeaderWrapper.h"
#include "UiBroadcaster.h"

/**
 * Allows the user to edit the frequencies.
 */
class SequenceFrequencySlider : public juce::Slider, public UiBroadcastedValueListener
{
  public:
    SequenceFrequencySlider(BroadcastedValue& value_);
    ~SequenceFrequencySlider();
    /**
     * @See UiBroadcastedValueListener.
     */
    void onBroadcastedValueChange(double newValue) override;

  private:
    BroadcastedValue& value;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SequenceFrequencySlider)
};
