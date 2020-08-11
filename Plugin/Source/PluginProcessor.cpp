
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <memory>

class ParameterWithCallback : public juce::AudioParameterFloat
{
  public:
    using Callback = std::function<void(float)>;
    ParameterWithCallback(const juce::String& parameterName,
                          float min,
                          float max,
                          float defaultValue,
                          Callback callback_)
        : juce::AudioParameterFloat(parameterName, parameterName, min, max, defaultValue)
        , callback(callback_)
    {
    }

  protected:
    void valueChanged(float newValue) override
    {
        callback(newValue);
    }

  private:
    Callback callback;
};

FunFilterAudioProcessor::FunFilterAudioProcessor() noexcept
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
    addParameter(std::make_unique<ParameterWithCallback>(
                     "FilterResonance", 0.01f, 3.f, 1.5f,
                     [this](float value) { setFilterResonance(value); })
                     .release());
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
    nbSamplesLeftBeforeNextStep = 0;
    currentFrequencyIndex = 0;
    nbSamplesLeftBeforeNextStep = filterChoregraphyStepPeriodInSamples;
    broadcaster.setValue<ValueIds::filterCutoff>(frequencies[currentFrequencyIndex]);
    filter.setFilterCutoffFrequency(frequencies[currentFrequencyIndex]);
    filter.setSampleRate(newSampleRate);
}

void FunFilterAudioProcessor::releaseResources()
{
}

void FunFilterAudioProcessor::nextFilterFrequency() noexcept
{
    currentFrequencyIndex = (currentFrequencyIndex + 1) % frequencies.size();
    assert(currentFrequencyIndex < frequencies.size());
    filter.setFilterCutoffFrequency(frequencies[currentFrequencyIndex]);
    broadcaster.setValue<ValueIds::filterCutoff>(frequencies[currentFrequencyIndex]);
    nbSamplesLeftBeforeNextStep = filterChoregraphyStepPeriodInSamples;
}

bool FunFilterAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == layouts.getMainInputChannelSet();
}

double
FunFilterAudioProcessor::calculateChoregraphyPeriodInSamplesFromBpm(double bpm) noexcept
{
    constexpr auto secInOneMinute = 60;
    const auto songFreqHz = bpm / secInOneMinute;
    const auto songPeriodInSamples = sampleRate / songFreqHz;
    const auto choregraphyPeriodInSamples =
        songPeriodInSamples * choregraphyLengthInBeats;
    const auto filterChoregraphySteps = static_cast<double>(frequencies.size());
    return choregraphyPeriodInSamples / filterChoregraphySteps;
}

[[nodiscard]] juce::AudioProcessorParameter&
FunFilterAudioProcessor::getParameterFromName(const std::string_view paramName) noexcept
{
    auto& parameterList = getParameters();
    auto paramIt =
        std::find_if(std::begin(parameterList), std::end(parameterList),
                     [&paramName](const auto& parameter) {
                         constexpr auto maxCharacters = 30;
                         return parameter->getName(maxCharacters) == paramName.data();
                     });
    assert(paramIt != std::end(parameterList));
    return **paramIt;
}

void FunFilterAudioProcessor::processBlock(
    juce::AudioBuffer<float>& buffer, [[maybe_unused]] juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    const auto totalNumInputChannels = getTotalNumInputChannels();
    const auto totalNumOutputChannels = getTotalNumOutputChannels();

    auto* playHead = getPlayHead();
    if (playHead != nullptr)
    {
        juce::AudioPlayHead::CurrentPositionInfo info;
        if (playHead->getCurrentPosition(info))
        {
            const auto timeInSamples = info.timeInSamples;
            filterChoregraphyStepPeriodInSamples =
                calculateChoregraphyPeriodInSamplesFromBpm(info.bpm);
            currentFrequencyIndex =
                (timeInSamples
                 / static_cast<int64_t>(filterChoregraphyStepPeriodInSamples))
                % frequencies.size();
            nbSamplesLeftBeforeNextStep =
                timeInSamples
                % static_cast<int64_t>(filterChoregraphyStepPeriodInSamples);
            filter.setFilterCutoffFrequency(frequencies[currentFrequencyIndex]);
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
        const auto nbSamplesToProcess = std::min(
            static_cast<int>(nbSamplesLeftBeforeNextStep), nbSamplesLeftToProcess);

        filter.process(buffer, nbProcessedSamples, nbSamplesToProcess);

        nbProcessedSamples += nbSamplesToProcess;
        nbSamplesLeftBeforeNextStep -= nbSamplesToProcess;
        assert(nbSamplesLeftBeforeNextStep >= 0);
        assert(nbProcessedSamples <= buffer.getNumSamples());

        if (nbSamplesLeftBeforeNextStep == 0)
        {
            nextFilterFrequency();
        }
    }
}

bool FunFilterAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* FunFilterAudioProcessor::createEditor()
{
    return std::make_unique<FunFilterEditor>(*this, broadcaster).release();
}

void FunFilterAudioProcessor::getStateInformation([
    [maybe_unused]] juce::MemoryBlock& destData)
{
}

void FunFilterAudioProcessor::setStateInformation([[maybe_unused]] const void* data,
                                                  [[maybe_unused]] int sizeInBytes)
{
}

void FunFilterAudioProcessor::setFilterResonance(double resonance) noexcept
{
    broadcaster.setValue<ValueIds::filterResonance>(resonance);
    filter.setResonance(resonance);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return std::make_unique<FunFilterAudioProcessor>().release();
}
