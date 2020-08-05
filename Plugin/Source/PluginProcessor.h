
#pragma once

#include "JuceHeaderWrapper.h"

/**
 */
class FunFilterAudioProcessor : public juce::AudioProcessor
{
  public:
    FunFilterAudioProcessor();
    ~FunFilterAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

  private:
    void updateFiltersFrequency() noexcept;

    void updateSmootherTargetFrequency() noexcept;

    juce::SmoothedValue<double, juce::ValueSmoothingTypes::Multiplicative>
        smoothedFilterFrequency;
    static constexpr auto expectedNbChannels = 2;
    std::array<juce::IIRFilter, expectedNbChannels> filters;

    int nbSamplesLeftBeforeNextStep = 0;
    double sampleRate = 44100;
    size_t currentFrequencyIndex{0};
    int filterChoregraphyStepPeriod{0};


    static constexpr std::array<double, 4> frequencies{{300, 1500, 800, 3000}};
    static constexpr double q = 1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FunFilterAudioProcessor)
};
