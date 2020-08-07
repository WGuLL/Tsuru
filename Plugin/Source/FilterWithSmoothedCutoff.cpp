
#include "FilterWithSmoothedCutoff.h"

void FilterWithSmoothedCutoff::setSampleRate(double newSampleRate) noexcept
{
    sampleRate = newSampleRate;
    const auto currentTargetFrequency = smoothedFilterFrequency.getTargetValue();
    smoothedFilterFrequency.reset(sampleRate, rampSize);
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
    smoothedFilterFrequency.setTargetValue(newFrequency);
}
