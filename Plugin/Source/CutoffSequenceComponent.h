
#pragma once

#include "JuceHeaderWrapper.h"
#include "MathUtils.h"
#include "PluginProcessor.h"
#include "SequenceFrequencySlider.h"
#include "UiBroadcaster.h"

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

        auto& parameter = processor.getParameterFromName(
            "Step " + std::to_string(stepIndex) + " frequency");
        const auto frequencyRange = MathUtils::frequencyRange(25.f, 15000.f);
        frequencySliders[stepIndex]->onValueChange = [this, frequencyRange,
                                                      &parameter]() {
            const auto value = frequencySliders[stepIndex]->getValue();
            parameter.setValueNotifyingHost(frequencyRange.convertTo0to1(value));
        };
    }

    void resized() override;

  private:
    std::array<std::unique_ptr<juce::Slider>, 4> frequencySliders;
};
