
#include "ResonanceKnob.h"

ResonanceKnob::ResonanceKnob(UiBroadcaster& broadcaster_,
                             std::function<void(double)> setResFunc) noexcept
    : juce::Slider(juce::Slider::RotaryVerticalDrag, juce::Slider::NoTextBox)
    , broadcaster(broadcaster_)
{
    setNormalisableRange({0.01, 2.});
    onValueChange = [setResFunc, this]() { setResFunc(getValue()); };
    broadcaster.getValue<ValueIds::filterResonance>().addListener(*this);
}

ResonanceKnob::~ResonanceKnob() noexcept
{
    broadcaster.getValue<ValueIds::filterResonance>().removeListener(*this);
}

void ResonanceKnob::onBroadcastedValueChange(double newValue)
{
    setValue(newValue, juce::dontSendNotification);
}
