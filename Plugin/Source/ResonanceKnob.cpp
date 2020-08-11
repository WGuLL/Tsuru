
#include "ResonanceKnob.h"

ResonanceKnob::ResonanceKnob(UiBroadcaster& broadcaster_,
                             juce::AudioProcessorParameter& parameter) noexcept
    : juce::Slider(juce::Slider::RotaryVerticalDrag, juce::Slider::NoTextBox)
    , broadcaster(broadcaster_)
{
    const auto& parameterRange =
        dynamic_cast<juce::AudioParameterFloat&>(parameter).range;
    setNormalisableRange({static_cast<double>(parameterRange.getRange().getStart()),
                          static_cast<double>(parameterRange.getRange().getEnd())});
    onValueChange = [parameterRange, &parameter, this]() {
        const auto normalizedParam =
            parameterRange.convertTo0to1(static_cast<float>(getValue()));
        parameter.setValueNotifyingHost(normalizedParam);
    };
    onDragStart = [&parameter, this]() { parameter.beginChangeGesture(); };
    onDragEnd = [&parameter, this]() { parameter.endChangeGesture(); };
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
