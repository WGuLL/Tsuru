
#include "CutoffSequenceComponent.h"

CutoffSequenceComponent::CutoffSequenceComponent(
    UiBroadcaster& broadcaster, FunFilterAudioProcessor& processor) noexcept
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

void CutoffSequenceComponent::resized()
{
    auto areaAvailable = getLocalBounds();
    const auto sliderWidth =
        proportionOfWidth(1.f / static_cast<float>(frequencySliders.size()));
    for (auto& frequencySlider : frequencySliders)
    {
        frequencySlider->setBounds(areaAvailable.removeFromLeft(sliderWidth));
    }
}
