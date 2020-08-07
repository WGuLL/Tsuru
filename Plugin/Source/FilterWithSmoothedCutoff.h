
#pragma once

#include "JuceHeaderWrapper.h"

/**
 * Low pass filter with a cutoff frequency
 * that can be changed quickly without cracks in the audio.
 */
class FilterWithSmoothedCutoff
{
  public:
    /**
     * Notified when the plugin's sample rate changes.
     */
    void setSampleRate(double newSampleRate) noexcept;
    /**
     *  Process the given audio buffer.
     */
    void process(juce::AudioBuffer<float>& inputAudioBuffer,
                 int startSample,
                 int numSample) noexcept;
    /**
     *  Cutoff frequency of the filter.
     */
    void setFilterCutoffFrequency(double newFrequency) noexcept;

  private:
    void updateFiltersFrequency() noexcept;

    juce::SmoothedValue<double, juce::ValueSmoothingTypes::Multiplicative>
        smoothedFilterFrequency;
    double sampleRate{0};
    static constexpr double q = 1.5;
    static constexpr std::chrono::duration<double> rampSize{0.01};
    static constexpr auto expectedNbChannels = 2;
    std::array<juce::IIRFilter, expectedNbChannels> filters;
};
