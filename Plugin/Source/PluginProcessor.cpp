
#include "PluginProcessor.h"
#include "MathUtils.h"
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
    ParameterWithCallback(const juce::String& parameterName,
                          juce::NormalisableRange<float> range,
                          float defaultValue,
                          Callback callback_)
        : juce::AudioParameterFloat(parameterName, parameterName, range, defaultValue)
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

class ChoiceParameterWithCallback : public juce::AudioParameterChoice
{
  public:
    using Callback = std::function<void(int)>;
    ChoiceParameterWithCallback(const juce::String& parameterName,
                                const juce::StringArray& choices,
                                int defaultIndex,
                                Callback callback_)
        : juce::AudioParameterChoice(parameterName, parameterName, choices, defaultIndex)
        , callback(callback_)
    {
    }

  protected:
    void valueChanged(int newValue) override
    {
        callback(newValue);
    }

  private:
    Callback callback;
};

namespace
{
template <size_t index>
std::unique_ptr<ParameterWithCallback>
createFrequencyStepParameter(double defaultValue, FunFilterAudioProcessor& processor)
{
    processor.setFilterStepFrequency<index>(defaultValue);
    return std::make_unique<ParameterWithCallback>(
        "Step " + std::to_string(index) + " frequency",
        MathUtils::frequencyRange<float>(), static_cast<float>(defaultValue),
        [&processor](float value) { processor.setFilterStepFrequency<index>(value); });
}
} // namespace


FunFilterAudioProcessor::FunFilterAudioProcessor() noexcept
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
    addParameter(std::make_unique<ParameterWithCallback>(
                     "FilterResonance", 0.01f, 3.f, 1.5f,
                     [this](float value) { setFilterResonance(value); })
                     .release());
    addParameter(
        std::make_unique<ChoiceParameterWithCallback>(
            "Rate", juce::StringArray{"1/8", "1/4", "1/2", "1", "2", "4", "8"}, 4,
            [this](int value)
            {
                broadcaster.setValue<ValueIds::sequenceDuration>(
                    static_cast<double>(value));
                setSequenceDuration(std::pow(2, static_cast<double>(value + 1) / 8));
            })
            .release());

    addParameter(createFrequencyStepParameter<0>(frequencies[0], *this).release());
    addParameter(createFrequencyStepParameter<1>(frequencies[1], *this).release());
    addParameter(createFrequencyStepParameter<2>(frequencies[2], *this).release());
    addParameter(createFrequencyStepParameter<3>(frequencies[3], *this).release());
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
                     [&paramName](const auto& parameter)
                     {
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

    auto* playHeadPtr = getPlayHead();
    if (playHead != nullptr)
    {
        const auto positionOpt = playHeadPtr->getPosition();
        if (positionOpt.hasValue())
        {
            const auto timeInSamples = positionOpt->getTimeInSeconds();
            filterChoregraphyStepPeriodInSamples =
                calculateChoregraphyPeriodInSamplesFromBpm(positionOpt->getBpm());
            currentFrequencyIndex =
                (timeInSamples
                 / static_cast<int64_t>(filterChoregraphyStepPeriodInSamples))
                % frequencies.size();
            nbSamplesLeftBeforeNextStep = static_cast<double>(
                timeInSamples
                % static_cast<int64_t>(filterChoregraphyStepPeriodInSamples));
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

void FunFilterAudioProcessor::getStateInformation(
    [[maybe_unused]] juce::MemoryBlock& destData)
{
    juce::XmlElement mainElement("TsuruPreset");
    for (const auto& parameter : getParameters())
    {
        auto xmlDataPtr = std::make_unique<juce::XmlElement>("Parameter");
        constexpr auto maxCharacters = 30;
        xmlDataPtr->setAttribute("Name", parameter->getName(maxCharacters));
        xmlDataPtr->setAttribute("NormalizedValue", parameter->getValue());
        mainElement.addChildElement(xmlDataPtr.release());
    }
    copyXmlToBinary(mainElement, destData);
}

void FunFilterAudioProcessor::setStateInformation([[maybe_unused]] const void* data,
                                                  [[maybe_unused]] int sizeInBytes)
{
    const auto xmlDataPtr = getXmlFromBinary(data, sizeInBytes);
    for (const auto& parameter : getParameters())
    {
        constexpr auto maxCharacters = 30;
        const auto parameterName = parameter->getName(maxCharacters);
        const auto* parameterXmlNode =
            xmlDataPtr->getChildByAttribute("Name", parameterName);
        if (parameterXmlNode != nullptr)
        {
            const auto paramValue =
                parameterXmlNode->getDoubleAttribute("NormalizedValue");
            parameter->setValue(static_cast<float>(paramValue));
        }
        else
        {
            juce::Logger::getCurrentLogger()->writeToLog("The value of the parameter "
                                                         + parameterName.quoted()
                                                         + " could not be restored.");
        }
    }
}

void FunFilterAudioProcessor::setSequenceDuration(double sequenceDurationInBeats) noexcept
{
    choregraphyLengthInBeats = sequenceDurationInBeats;
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
