
#include "SequenceFrequencySlider.h"
#include "MathUtils.h"

SequenceFrequencySlider::SequenceFrequencySlider(BroadcastedValue& value_)
    : value(value_)
{
    setNormalisableRange(MathUtils::frequencyRange(25., 15000.));

    setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
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
