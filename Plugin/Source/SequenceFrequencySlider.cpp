
#include "SequenceFrequencySlider.h"
#include "ColorPalette.h"
#include "MathUtils.h"

SequenceFrequencySlider::SequenceFrequencySlider(BroadcastedValue& value_)
    : juce::Slider(juce::Slider::SliderStyle::LinearVertical,
                   juce::Slider::TextEntryBoxPosition::NoTextBox)
    , value(value_)
{
    setNormalisableRange(MathUtils::frequencyRange(25., 15000.));
    setColour(juce::Slider::ColourIds::thumbColourId, juce::Colours::transparentWhite);
    setColour(juce::Slider::ColourIds::trackColourId, ColorPalette::selectiveYellow);
    setColour(juce::Slider::ColourIds::backgroundColourId, ColorPalette::mediumSlateBlue);


    value.addListener(*this);
}

SequenceFrequencySlider::~SequenceFrequencySlider()
{
    value.removeListener(*this);
}

void SequenceFrequencySlider::onBroadcastedValueChange(double newValue)
{
    setValue(newValue, juce::dontSendNotification);
}
