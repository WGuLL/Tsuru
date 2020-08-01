
#include "PluginProcessor.h"
#include "PluginEditor.h"

FunFilterAudioProcessor::FunFilterAudioProcessor()
     : AudioProcessor (BusesProperties()
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                       )
{
}

FunFilterAudioProcessor::~FunFilterAudioProcessor() = default;

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

void FunFilterAudioProcessor::setCurrentProgram ([[maybe_unused]] int index)
{
}

const juce::String FunFilterAudioProcessor::getProgramName ([[maybe_unused]] int index)
{
    return {};
}

void FunFilterAudioProcessor::changeProgramName ([[maybe_unused]] int index, [[maybe_unused]]  const juce::String& newName)
{
}

void FunFilterAudioProcessor::prepareToPlay ([[maybe_unused]] double sampleRate, [[maybe_unused]] int samplesPerBlock)
{
}

void FunFilterAudioProcessor::releaseResources()
{
}

bool FunFilterAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == layouts.getMainInputChannelSet();
}

void FunFilterAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, [[maybe_unused]] juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
}

bool FunFilterAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* FunFilterAudioProcessor::createEditor()
{
    return new FunFilterEditor (*this);
}

void FunFilterAudioProcessor::getStateInformation ( [[maybe_unused]] juce::MemoryBlock& destData)
{
}

void FunFilterAudioProcessor::setStateInformation ([[maybe_unused]] const void* data,[[maybe_unused]]  int sizeInBytes)
{
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FunFilterAudioProcessor();
}
