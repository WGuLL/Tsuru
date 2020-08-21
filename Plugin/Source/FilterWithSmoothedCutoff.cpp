
#include "FilterWithSmoothedCutoff.h"

FilterWithSmoothedCutoff::FilterWithSmoothedCutoff() noexcept
{
    constexpr auto defaultFrequency = 300;
    smoothedFilterFrequency.setCurrentAndTargetValue(defaultFrequency);
    setSampleRate(sampleRate);
}

void FilterWithSmoothedCutoff::setSampleRate(double newSampleRate) noexcept
{
    sampleRate = newSampleRate;
    const auto currentTargetFrequency = smoothedFilterFrequency.getTargetValue();
    smoothedFilterFrequency.reset(sampleRate, rampSize.count());
    smoothedFilterFrequency.setCurrentAndTargetValue(currentTargetFrequency);
    for (auto& filter : filters)
    {
        filter.reset();
        filter.setCoefficients(
            juce::IIRCoefficients::makeLowPass(sampleRate, currentTargetFrequency, q));
    }
}

void FilterWithSmoothedCutoff::updateFiltersFrequency() noexcept
{
    for (auto& filter : filters)
    {
        filter.setCoefficients(juce::IIRCoefficients::makeLowPass(
            sampleRate, smoothedFilterFrequency.getNextValue(), q));
    }
}

void FilterWithSmoothedCutoff::process(juce::AudioBuffer<float>& inputAudioBuffer,
                                       int startSample,
                                       int numSamples) noexcept
{
    for (auto sampleIndex = startSample; sampleIndex < startSample + numSamples;
         ++sampleIndex)
    {
        updateFiltersFrequency();
        for (auto channel = 0; channel < std::min(inputAudioBuffer.getNumChannels(),
                                                  static_cast<int>(filters.size()));
             ++channel)
        {
            const auto outputSample = filters[channel].processSingleSampleRaw(
                inputAudioBuffer.getSample(channel, sampleIndex));
            assert(!std::isnan(outputSample));
            inputAudioBuffer.setSample(channel, sampleIndex, outputSample);
        }
    }
}

void FilterWithSmoothedCutoff::setFilterCutoffFrequency(double newFrequency) noexcept
{
    constexpr auto min = 25.;
    constexpr auto max = 16000.;
    assert(std::clamp(newFrequency, min, max) == newFrequency);
    smoothedFilterFrequency.setTargetValue(newFrequency);
}

void FilterWithSmoothedCutoff::setResonance(double value) noexcept
{
    constexpr auto min = 0.;
    constexpr auto max = 3.;
    assert(std::clamp(value, min, max) == value);
    q = value;
}
