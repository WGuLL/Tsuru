
#include "ResonanceKnob.h"
#include "ColorPalette.h"

ResonanceKnob::ResonanceKnob(UiBroadcaster& broadcaster_,
                             juce::AudioProcessorParameter& parameter) noexcept
    : broadcaster(broadcaster_)
    , parameterNameLabel("", "Resonance")
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
        dynamic_cast<juce::AudioParameterFloat&>(parameter).range;
    knob.setNormalisableRange({static_cast<double>(parameterRange.getRange().getStart()),
                               static_cast<double>(parameterRange.getRange().getEnd())});
    knob.onValueChange = [parameterRange, &parameter, this]() {
        const auto normalizedParam =
            parameterRange.convertTo0to1(static_cast<float>(knob.getValue()));
        parameter.setValueNotifyingHost(normalizedParam);
    };
    knob.onDragStart = [&parameter, this]() { parameter.beginChangeGesture(); };
    knob.onDragEnd = [&parameter, this]() { parameter.endChangeGesture(); };
    broadcaster.getValue<ValueIds::filterResonance>().addListener(*this);
}

ResonanceKnob::~ResonanceKnob() noexcept
{
    broadcaster.getValue<ValueIds::filterResonance>().removeListener(*this);
}

void ResonanceKnob::onBroadcastedValueChange(double newValue)
{
    knob.setValue(newValue, juce::dontSendNotification);
}

void ResonanceKnob::resized()
{
    auto areaAvailable = getLocalBounds();
    parameterNameLabel.setBounds(areaAvailable.removeFromTop(proportionOfHeight(0.2f)));
    knob.setBounds(areaAvailable);
}
