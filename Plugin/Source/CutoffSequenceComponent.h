
#pragma once

#include "JuceHeaderWrapper.h"
#include "MathUtils.h"
#include "PluginProcessor.h"
#include "SequenceFrequencySlider.h"
#include "UiBroadcaster.h"
#include <cassert>

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
        const auto frequencyRange = MathUtils::frequencyRange<float>();
        frequencySliders[stepIndex]->onValueChange = [this, frequencyRange,
                                                      &parameter]() {
            const auto value = static_cast<float>(frequencySliders[stepIndex]->getValue());
            parameter.setValueNotifyingHost(frequencyRange.convertTo0to1(value));
        };
        frequencySliders[stepIndex]->onDragStart = [&parameter]() {
            parameter.beginChangeGesture();
        };
        frequencySliders[stepIndex]->onDragEnd = [&parameter]() {
            parameter.endChangeGesture();
        };
    }

    void resized() override;

  private:
    juce::Label titleLabel;
    std::array<std::unique_ptr<juce::Slider>, 4> frequencySliders;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CutoffSequenceComponent)
};
