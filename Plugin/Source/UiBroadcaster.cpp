
#include "UiBroadcaster.h"

void BroadcastedValue::setValue(double newValue) noexcept
{
    value.store(newValue);
    changed.clear();
}

void BroadcastedValue::notifyListenersIfNecessary() noexcept
{
    assert(juce::MessageManager::getInstance()->isThisTheMessageThread());
    if (!changed.test_and_set())
    {
        for (const auto& listener : listeners)
        {
            listener.get().onBroadcastedValueChange(value.load());
        }
    }
}

void BroadcastedValue::addListener(UiBroadcastedValueListener& listener) noexcept
{
    listeners.emplace_back(listener);
    listener.onBroadcastedValueChange(value);
}

void BroadcastedValue::removeListener(
    UiBroadcastedValueListener& listenerToRemove) noexcept
{
    listeners.erase(std::remove_if(begin(listeners), end(listeners),
                                   [&listenerToRemove](const auto& listener) {
                                       return std::addressof(listenerToRemove)
                                              == std::addressof(listener.get());
                                   }),
                    end(listeners));
}

UiBroadcaster::UiBroadcaster()
{
    startTimer(static_cast<int>(guiRefreshPeriod.count()));
}

void UiBroadcaster::timerCallback()
{
    for (auto& value : values)
    {
        value.notifyListenersIfNecessary();
    }
}
