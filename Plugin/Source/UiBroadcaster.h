
#pragma once

#include "JuceHeaderWrapper.h"

using namespace std::chrono_literals;

/**
 * Values to broadcast to the UI.
 */
enum class ValueIds : size_t
{
    filterCutoff,
    nbValues
};
/**
 * Interface of a value listener.
 */
class UiBroadcastedValueListener
{
  public:
    virtual ~UiBroadcastedValueListener() = default;
    /**
     * Called by the message thread when the value has changed.
     */
    virtual void valueChanged(double newValue) = 0;
};
/**
 * Implementation of the value notification.
 */
class BroadcastedValue
{
  public:
    /**
     * Can be called from the audio thread when the
     * corresponding value has changed.
     * The listeners will be notified on the message thread at the next
     * notifyListenersIfNecessary call.
     */
    void setValue(double newValue) noexcept;
    /**
     *  Must be called from the message thread.
     *  The listener will be notified if the value has changed.
     */
    void notifyListenersIfNecessary() noexcept;
    /**
     *  The given listener will be notified from the audio thread if the value has
     * changed.
     */
    void addListener(UiBroadcastedValueListener& listener) noexcept;
    /**
     *  Removes the given listener from the list of listener.
     */
    void removeListener(UiBroadcastedValueListener& listenerToRemove) noexcept;

  private:
    std::vector<std::reference_wrapper<UiBroadcastedValueListener>> listeners;
    std::atomic<double> value;
    std::atomic_flag changed;
};

/**
 *  Class allowing to broadcast values to the GUI
 *  independently from the audio thread.
 *  Values are represented in the ValuesToBroadcast enum.
 */
class UiBroadcaster : public juce::Timer
{
  public:
    static constexpr auto guiRefreshPeriod = 30ms;
    UiBroadcaster();

    template <ValueIds valueId>
    void setValue(double value) noexcept
    {
        values[static_cast<size_t>(valueId)].setValue(value);
    }

    template <ValueIds valueId>
    BroadcastedValue& getValue() noexcept
    {
        return values[static_cast<size_t>(valueId)];
    }

  private:
    void timerCallback() override;
    std::array<BroadcastedValue, static_cast<size_t>(ValueIds::nbValues)> values;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UiBroadcaster)
};
