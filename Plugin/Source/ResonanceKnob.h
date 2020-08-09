
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
                  std::function<void(double)> setResFunc) noexcept;

    ~ResonanceKnob() noexcept;

    void onBroadcastedValueChange(double newValue) override;

  private:
    UiBroadcaster& broadcaster;
};
