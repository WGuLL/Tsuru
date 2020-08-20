
#pragma once

#include "JuceHeaderWrapper.h"
#include "MathUtils.h"
#include "PluginProcessor.h"
#include "UiBroadcaster.h"

class SequenceFrequencySlider : public juce::Slider, public UiBroadcastedValueListener
{
  public:
    SequenceFrequencySlider(BroadcastedValue& value_)
        : value(value_)
    {
        setNormalisableRange(MathUtils::frequencyRange(25., 15000.));

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
                            FunFilterAudioProcessor& processor) noexcept;

    template <size_t stepIndex>
    void initializeSlider(FunFilterAudioProcessor& processor,
                          UiBroadcaster& broadcaster) noexcept
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

    void resized() override;

  private:
    std::array<std::unique_ptr<juce::Slider>, 4> frequencySliders;
};
