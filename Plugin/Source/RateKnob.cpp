
#include "RateKnob.h"
#include "ColorPalette.h"

RateKnob::RateKnob(UiBroadcaster& broadcaster_,
                   juce::AudioProcessorParameter& parameter) noexcept
    : broadcaster(broadcaster_)
    , parameterNameLabel("", "Rate")
    , knob(juce::Slider::RotaryVerticalDrag, juce::Slider::NoTextBox)
{
    addAndMakeVisible(parameterNameLabel);
    parameterNameLabel.setJustificationType(juce::Justification::centredBottom);
    parameterNameLabel.setColour(juce::Label::textColourId,
                                 ColorPalette::mediumSlateBlue);

    addAndMakeVisible(knob);
    knob.setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId,
                   ColorPalette::mediumSlateBlue);
    knob.setColour(juce::Slider::ColourIds::rotarySliderFillColourId,
                   ColorPalette::selectiveYellow);
    knob.setColour(juce::Slider::ColourIds::thumbColourId,
                   juce::Colours::transparentWhite);

    const auto& parameterRange =
        dynamic_cast<juce::AudioParameterChoice&>(parameter).getNormalisableRange();
    knob.setNormalisableRange({static_cast<double>(parameterRange.start),
                               static_cast<double>(parameterRange.end),
                               static_cast<float>(parameterRange.interval)});
    knob.onValueChange = [parameterRange, &parameter, this]() {
        const auto normalizedParam =
            parameterRange.convertTo0to1(static_cast<float>(knob.getValue()));
        parameter.setValueNotifyingHost(normalizedParam);
    };
    knob.onDragStart = [&parameter]() { parameter.beginChangeGesture(); };
    knob.onDragEnd = [&parameter]() { parameter.endChangeGesture(); };
    broadcaster.getValue<ValueIds::sequenceDuration>().addListener(*this);
}

RateKnob::~RateKnob() noexcept
{
    broadcaster.getValue<ValueIds::sequenceDuration>().removeListener(*this);
}

void RateKnob::onBroadcastedValueChange(double newValue)
{
    knob.setValue(newValue, juce::dontSendNotification);
}

void RateKnob::resized()
{
    auto areaAvailable = getLocalBounds();
    parameterNameLabel.setBounds(areaAvailable.removeFromTop(proportionOfHeight(0.2f)));
    knob.setBounds(areaAvailable);
}
