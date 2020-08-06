
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
    virtual void valueChanged(double newValue) = 0;
};
/**
 * Implementation of the value notification.
 */
class BroadcastedValue
{
  public:
    void setValue(double newValue)
    {
        value.store(newValue);
        changed.clear();
    }

    void notifyListenersIfNecessary() noexcept
    {
        if (!changed.test_and_set())
        {
            for (const auto& listener : listeners)
            {
                listener.get().valueChanged(value.load());
            }
        }
    }

    void addListener(UiBroadcastedValueListener& listener) noexcept
    {
        listeners.emplace_back(listener);
    }

    void removeListener(UiBroadcastedValueListener& listenerToRemove) noexcept
    {
        listeners.erase(std::remove_if(begin(listeners), end(listeners),
                                       [&listenerToRemove](const auto& listener) {
                                           return std::addressof(listenerToRemove)
                                                  == std::addressof(listener.get());
                                       }),
                        end(listeners));
        listeners.emplace_back(listenerToRemove);
    }

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

    UiBroadcaster()
    {
        startTimer(static_cast<int>(guiRefreshPeriod.count()));
    }

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

    void timerCallback() override
    {
        for (auto& value : values)
        {
            value.notifyListenersIfNecessary();
        }
    }

  private:
    std::array<BroadcastedValue, static_cast<size_t>(ValueIds::nbValues)> values;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UiBroadcaster)
};
