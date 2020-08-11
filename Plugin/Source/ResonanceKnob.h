
#include "JuceHeaderWrapper.h"
#include "UiBroadcaster.h"
/**
 * Allows the user to change the resonance
 * of the applied filter.
 */
class ResonanceKnob : public juce::Slider, public UiBroadcastedValueListener
{
  public:
    ResonanceKnob(UiBroadcaster& broadcaster_,
                  juce::AudioProcessorParameter& parameter) noexcept;

    ~ResonanceKnob() noexcept;

    void onBroadcastedValueChange(double newValue) override;

  private:
    UiBroadcaster& broadcaster;
};
