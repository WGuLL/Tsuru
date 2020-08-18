
#pragma once

#include "JuceHeaderWrapper.h"

/**
 * Shows and edits the different cutoff frequencies of the
 * sequence.
 */
class CutoffSequenceComponent : public juce::Component
{
  public:
    CutoffSequenceComponent()
    {
        for (auto& frequencySlider : frequencySliders)
        {
            addAndMakeVisible(frequencySlider);
        }
    }

    void resized()
    {
        auto areaAvailable = getLocalBounds();
        const auto sliderWidth =
            proportionOfWidth(1.f / static_cast<float>(frequencySliders.size()));
        for (auto& frequencySlider : frequencySliders)
        {
            frequencySlider.setBounds(areaAvailable.removeFromLeft(sliderWidth));
        }
    }

  private:
    std::array<juce::Slider, 4> frequencySliders;
};
