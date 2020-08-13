

#include "JuceHeaderWrapper.h"
#include "UiBroadcaster.h"

class RateKnob : public juce::Component, public UiBroadcastedValueListener
{
public:
    RateKnob(UiBroadcaster& broadcaster_,
    juce::AudioProcessorParameter& parameter) noexcept
    : broadcaster(broadcaster_)
    , parameterNameLabel("", "Rate")
    , knob(juce::Slider::RotaryVerticalDrag, juce::Slider::NoTextBox)
    {
        addAndMakeVisible(knob);
        addAndMakeVisible(parameterNameLabel);

        const auto& parameterRange =
            dynamic_cast<juce::AudioParameterChoice&>(parameter).getNormalisableRange ();
        knob.setNormalisableRange({static_cast<double>(parameterRange.start),
            static_cast<double>(parameterRange.end), static_cast<float>(parameterRange.interval)});
        knob.onValueChange = [parameterRange, &parameter, this]() {
            const auto normalizedParam =
                parameterRange.convertTo0to1(static_cast<float>(knob.getValue()));
            parameter.setValueNotifyingHost(normalizedParam);
        };
        knob.onDragStart = [&parameter, this]() { parameter.beginChangeGesture(); };
        knob.onDragEnd = [&parameter, this]() { parameter.endChangeGesture(); };
        broadcaster.getValue<ValueIds::sequenceDuration>().addListener(*this);
        
        
        
    }

      ~RateKnob() noexcept
      {
          broadcaster.getValue<ValueIds::sequenceDuration>().removeListener(*this);
      }

      void onBroadcastedValueChange(double newValue) override
      {
          knob.setValue(newValue, juce::dontSendNotification);
      }
    void paint (juce::Graphics& g)override
    {
        g.fillAll (juce::Colours::white);
    }

      void resized() override
    {
        auto areaAvailable = getLocalBounds();
        knob.setBounds(areaAvailable.removeFromTop(proportionOfHeight(0.8f)));
        parameterNameLabel.setBounds(areaAvailable.translated(0, proportionOfHeight(-0.1f)));
    }

    private:
      UiBroadcaster& broadcaster;
      juce::Label parameterNameLabel;
      juce::Slider knob;
};
