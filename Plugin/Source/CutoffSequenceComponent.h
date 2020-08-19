
#pragma once

#include "JuceHeaderWrapper.h"
#include "PluginProcessor.h"
#include "UiBroadcaster.h"

namespace
{
[[nodiscard]] NormalisableRange<double> frequencyRange(double min, double max) noexcept
{
    constexpr auto convertFrom0To1Func = [](auto min, auto max, auto value) {
        return juce::mapToLog10(value, min, max);
    };
    constexpr auto convertTo0To1Func = [](auto min, auto max, auto value) {
        return juce::mapFromLog10(value, min, max);
    };
    return {min, max, convertFrom0To1Func, convertTo0To1Func};
}

} // namespace

class SequenceFrequencySlider : public juce::Slider, public UiBroadcastedValueListener
{
  public:
    SequenceFrequencySlider(BroadcastedValue& value_)
        : value(value_)
    {
        setNormalisableRange(frequencyRange(25, 15000));

        setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
        value.addListener(*this);
    }
    ~SequenceFrequencySlider()
    {
        value.removeListener(*this);
    }
    void onBroadcastedValueChange(double newValue) override
    {
        setValue(newValue);
    }

  private:
    BroadcastedValue& value;
};

/**
 * Shows and edits the different cutoff frequencies of the
 * sequence.
 */
class CutoffSequenceComponent : public juce::Component
{
  public:
    CutoffSequenceComponent(UiBroadcaster& broadcaster,
                            FunFilterAudioProcessor& processor)
    {
        initializeSlider<0>(processor, broadcaster);
        initializeSlider<1>(processor, broadcaster);
        initializeSlider<2>(processor, broadcaster);
        initializeSlider<3>(processor, broadcaster);

        for (auto i = size_t{0}; i < frequencySliders.size(); ++i)
        {
            addAndMakeVisible(frequencySliders[i].get());
        }
    }

    template <size_t stepIndex>
    void initializeSlider(FunFilterAudioProcessor& processor, UiBroadcaster& broadcaster)
    {
        assert(stepIndex < frequencySliders.size());
        frequencySliders[stepIndex] = std::make_unique<SequenceFrequencySlider>(
            broadcaster.getValue<static_cast<ValueIds>(
                stepIndex + static_cast<size_t>(ValueIds::step0Frequency))>());
        frequencySliders[stepIndex]->onValueChange = [this, &processor]() {
            const auto value = frequencySliders[stepIndex]->getValue();
            processor.setFilterStepFrequency<stepIndex>(value);
        };
    }

    void resized() override
    {
        auto areaAvailable = getLocalBounds();
        const auto sliderWidth =
            proportionOfWidth(1.f / static_cast<float>(frequencySliders.size()));
        for (auto& frequencySlider : frequencySliders)
        {
            frequencySlider->setBounds(areaAvailable.removeFromLeft(sliderWidth));
        }
    }

  private:
    std::array<std::unique_ptr<juce::Slider>, 4> frequencySliders;
};
