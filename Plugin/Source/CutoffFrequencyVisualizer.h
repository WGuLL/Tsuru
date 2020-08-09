
#include "JuceHeaderWrapper.h"
#include "UiBroadcaster.h"

/**
 * Shows the filter cutoff frequency on a graduated spectrum.
 */
class CutoffFrequencyVisualizer : public juce::Component,
                                  public UiBroadcastedValueListener
{
  public:
    CutoffFrequencyVisualizer(UiBroadcaster& broadcaster_) noexcept;
    ~CutoffFrequencyVisualizer() noexcept;

    void paint(juce::Graphics& g) override;

    void onBroadcastedValueChange(double value) override;

  private:
    /**
     * setColour has to be called first.
     */
    void drawFrequencyVerticalLine(int frequency, juce::Graphics& g) const;
    void drawFilterShape(int frequency, juce::Graphics& g) const;

    const juce::NormalisableRange<float> range;
    static constexpr std::array<int, 3> graduatedFrequencies{{100, 1000, 10000}};
    double cutoffValue{1000};
    UiBroadcaster& broadcaster;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CutoffFrequencyVisualizer)
};
