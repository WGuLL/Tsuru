
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <memory>

FunFilterAudioProcessor::FunFilterAudioProcessor() noexcept
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
}

FunFilterAudioProcessor::~FunFilterAudioProcessor() noexcept = default;

const juce::String FunFilterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FunFilterAudioProcessor::acceptsMidi() const
{
    return false;
}

bool FunFilterAudioProcessor::producesMidi() const
{
    return false;
}

bool FunFilterAudioProcessor::isMidiEffect() const
{
    return false;
}

double FunFilterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FunFilterAudioProcessor::getNumPrograms()
{
    return 1;
}

int FunFilterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FunFilterAudioProcessor::setCurrentProgram([[maybe_unused]] int index)
{
}

const juce::String FunFilterAudioProcessor::getProgramName([[maybe_unused]] int index)
{
    return {};
}

void FunFilterAudioProcessor::changeProgramName(
    [[maybe_unused]] int index, [[maybe_unused]] const juce::String& newName)
{
}

void FunFilterAudioProcessor::prepareToPlay(double newSampleRate,
                                            [[maybe_unused]] int samplesPerBlock)
{
    sampleRate = newSampleRate;
    smoothedFilterFrequency.reset(newSampleRate, 0.001);
    smoothedFilterFrequency.setCurrentAndTargetValue(frequencies[currentFrequencyIndex]);
    for (auto& filter : filters)
    {
        filter.reset();
        filter.setCoefficients(juce::IIRCoefficients::makeLowPass(
            sampleRate, frequencies[currentFrequencyIndex], q));
    }
}

void FunFilterAudioProcessor::releaseResources()
{
}

void FunFilterAudioProcessor::nextFilterFrequency() noexcept
{
    currentFrequencyIndex = (currentFrequencyIndex + 1) % frequencies.size();
    assert(currentFrequencyIndex < frequencies.size());
    smoothedFilterFrequency.setTargetValue(frequencies[currentFrequencyIndex]);
    nbSamplesLeftBeforeNextStep = filterChoregraphyStepPeriod;
}

void FunFilterAudioProcessor::updateFiltersFrequency() noexcept
{
    assert(currentFrequencyIndex < frequencies.size());
    for (auto& filter : filters)
    {
        filter.setCoefficients(juce::IIRCoefficients::makeLowPass(
            sampleRate, smoothedFilterFrequency.getNextValue(), q));
    }
}

bool FunFilterAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == layouts.getMainInputChannelSet();
}

void FunFilterAudioProcessor::processBlock(
    juce::AudioBuffer<float>& buffer, [[maybe_unused]] juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    const auto totalNumInputChannels = getTotalNumInputChannels();
    const auto totalNumOutputChannels = getTotalNumOutputChannels();

    auto* playHead = getPlayHead();
    juce::AudioPlayHead::CurrentPositionInfo info;
    if (playHead->getCurrentPosition(info))
    {
        const auto bpm = info.bpm;
        constexpr auto secInOneMinute = 60;
        const auto songFreqHz = bpm / secInOneMinute;
        const auto songPeriodInSamples = sampleRate / songFreqHz;
        const auto filterChoregraphySteps = static_cast<double>(frequencies.size());
        const auto newFilterChoregraphyStepPeriod =
            static_cast<int>(songPeriodInSamples / filterChoregraphySteps);
        if (newFilterChoregraphyStepPeriod != filterChoregraphyStepPeriod)
        {
            filterChoregraphyStepPeriod = newFilterChoregraphyStepPeriod;
            nbSamplesLeftBeforeNextStep = filterChoregraphyStepPeriod;
        }
    }

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    {
        buffer.clear(i, 0, buffer.getNumSamples());
    }

    auto nbProcessedSamples{0};
    while (nbProcessedSamples < buffer.getNumSamples())
    {
        const auto nbSamplesLeftToProcess = buffer.getNumSamples() - nbProcessedSamples;
        const auto nbSamplesToProcess =
            std::min(nbSamplesLeftBeforeNextStep, nbSamplesLeftToProcess);

        if (nbSamplesLeftBeforeNextStep == 0)
        {
            nextFilterFrequency();
        }

        for (auto sampleIndex = nbProcessedSamples;
             sampleIndex < nbProcessedSamples + nbSamplesToProcess; ++sampleIndex)
        {
            updateFiltersFrequency();
            for (auto channel = 0; channel < std::min(totalNumInputChannels,
                                                      static_cast<int>(filters.size()));
                 ++channel)
            {
                const auto outputSample = filters[channel].processSingleSampleRaw(
                    buffer.getSample(channel, sampleIndex));
                buffer.setSample(channel, sampleIndex, outputSample);
            }
        }
        nbProcessedSamples += nbSamplesToProcess;
        nbSamplesLeftBeforeNextStep -= nbSamplesToProcess;
        assert(nbSamplesLeftBeforeNextStep >= 0);
        assert(nbProcessedSamples <= buffer.getNumSamples());
    }
}

bool FunFilterAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* FunFilterAudioProcessor::createEditor()
{
    return std::make_unique<FunFilterEditor>(*this).release();
}

void FunFilterAudioProcessor::getStateInformation([
    [maybe_unused]] juce::MemoryBlock& destData)
{
}

void FunFilterAudioProcessor::setStateInformation([[maybe_unused]] const void* data,
                                                  [[maybe_unused]] int sizeInBytes)
{
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return std::make_unique<FunFilterAudioProcessor>().release();
}
