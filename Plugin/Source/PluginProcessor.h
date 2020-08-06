
#pragma once

#include "JuceHeaderWrapper.h"
#include "UiBroadcaster.h"

/**
 * Implements the callbacks coming from the DAW.
 */
class FunFilterAudioProcessor : public juce::AudioProcessor
{
  public:
    FunFilterAudioProcessor() noexcept;
    ~FunFilterAudioProcessor() noexcept final;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    [[nodiscard]] bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    [[nodiscard]] juce::AudioProcessorEditor* createEditor() override;
    [[nodiscard]] bool hasEditor() const override;

    [[nodiscard]] const juce::String getName() const override;

    [[nodiscard]] bool acceptsMidi() const override;
    [[nodiscard]] bool producesMidi() const override;
    [[nodiscard]] bool isMidiEffect() const override;
    [[nodiscard]] double getTailLengthSeconds() const override;

    [[nodiscard]] int getNumPrograms() override;
    [[nodiscard]] int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    [[nodiscard]] const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

  private:
    /**
     *  Updates the filters to the frequency value given by the smoothers.
     */
    void updateFiltersFrequency() noexcept;
    /**
     *  Changes the target value of the smoother to the next value
     *  given by frequency array.
     */
    void nextFilterFrequency() noexcept;
    /**
     *  Returns the number of sample between each nextFilterFrequency calls.
     */
    [[nodiscard]] int calculateChoregraphyPeriodInSamplesFromBpm(int bpm) noexcept;

    juce::SmoothedValue<double, juce::ValueSmoothingTypes::Multiplicative>
        smoothedFilterFrequency;
    static constexpr auto expectedNbChannels = 2;
    std::array<juce::IIRFilter, expectedNbChannels> filters;

    int nbSamplesLeftBeforeNextStep{0};
    double sampleRate{44100};
    size_t currentFrequencyIndex{0};
    int filterChoregraphyStepPeriod{0};

    UiBroadcaster broadcaster;

    static constexpr auto choregraphyLengthInBeats{2};
    static constexpr auto filterChoregraphyNbSteps{4};
    static constexpr std::array<double, filterChoregraphyNbSteps> frequencies{
        {300, 1500, 800, 3000}};
    static constexpr double q = 1.5;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FunFilterAudioProcessor)
};
