
#include "JuceHeaderWrapper.h"
#include "UiBroadcaster.h"
/**
 * Allows the user to change the resonance
 * of the applied filter.
 */
class ResonanceKnob : public juce::Slider, public UiBroadcastedValueListener
{
public:
    ResonanceKnob (UiBroadcaster& broadcaster_, std::function<void(double)> setResFunc) : juce::Slider (juce::Slider::RotaryVerticalDrag, juce::Slider::NoTextBox), broadcaster(broadcaster_)
    {
        setNormalisableRange ({0.01, 2.});
        onValueChange (std::function<void()>([setResFunc, this](){setResFunc (getValue());
        }));
    }
    
     void valueChanged(double newValue) override
    {
        setValue (newValue, juce::dontSendNotification);
    }
    
private:
    UiBroadcaster& broadcaster;
};
